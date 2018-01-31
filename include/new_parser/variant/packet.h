#ifndef PACKET__HEADER_FILE
#define PACKET__HEADER_FILE
#include "variant.h"

template <unsigned I, class T, class U, class... Left>
	struct get_index: get_index<I + 1, T, Left...>
		{};
template <unsigned I, class T, class... Left>
	struct get_index<I, T, T, Left...>
		{ static constexpr unsigned value = I; };

template <unsigned I, class T, class... Args>
	struct get_type_by_index: get_type_by_index<I - 1, Args...>
		{};
template <class T, class... Args>
	struct get_type_by_index<0, T, Args...>
		{ using type = T; };

template <class KeyTy, unsigned I, KeyTy X, KeyTy Y, KeyTy... Args>
	struct index_of_helper: index_of_helper<KeyTy, I + 1, X, Args...>
		{};
template <class KeyTy, unsigned I, KeyTy X, KeyTy... Args>
	struct index_of_helper<KeyTy, I, X, X, Args...>
		{ static constexpr unsigned value = I; };
template <class KeyTy, KeyTy X, KeyTy... Args>
	struct index_of: index_of_helper<KeyTy, 0, X, Args...>
		{};

template <template <typename T> class Hash,
		class KeyTy,
		template <KeyTy U, KeyTy... Left> class Fail,
		template <KeyTy> class Reflection,
		class... T>
	class basic_packet;
template <template <typename T> class Hash,
		class KeyTy,
		template <KeyTy U, KeyTy... Left> class Fail,
		template <KeyTy> class Reflection,
		template <class, KeyTy> class F,
		class... T,
		KeyTy... K>
	class basic_packet<Hash, KeyTy, Fail, Reflection, F<T, K>...>
	{
		template <KeyTy X, KeyTy... Y>
			struct is_unique: is_unique<Y...>
			{};
		template <KeyTy X, KeyTy... Y>
			struct is_unique<X, X, Y...>: std::false_type
			{};
		template <KeyTy X>
			struct is_unique<X>: std::true_type
			{};
		using check_valid = typename std::enable_if<make_and<
				is_unique<K...>::value,
				!std::is_function<T>::value...,
				!std::is_void<T>::value...,
				!std::is_array<T>::value...,
				!std::is_reference<T>::value...
			>::value>::type;
		template <class X, class... Args>
		friend struct packet_iterator;
		template <class U, KeyTy I>
			struct unique
				{ U value; };
		template <KeyTy X, KeyTy Y, KeyTy... Args>
			struct contains
			{ static constexpr bool value = X == Y ?
					true : contains<X, Args...>::value; };
		template <KeyTy X, KeyTy Y>
			struct contains<X, Y>
			{ static constexpr bool value = X == Y; };
		template <KeyTy X>
			struct get_map
			{ using type = typename get_type_by_index<index_of<KeyTy, X, K...>::value, T...>::type; };
		
		using variant_type = basic_variant<Hash, err_handler, unique<T, K>...>;
		template <KeyTy X, KeyTy... Args>
			struct packet_iterator
			{
				static bool make_match(variant_type& var,
					std::function<void(typename get_map<X>::type&)>&& func,
					std::function<void(typename get_map<Args>::type&)>&&... call_back)
				{
					if (var.is<typename get_map<X>::type>())
					{
						auto& get_data = var.get<unique<typename get_map<X>::type, X>>().value;
						Reflection<X>::execute(get_data);
						func(get_data);
						return true;
					}
					return packet_iterator<Args...>::make_match(var,
						std::forward<std::function<void(typename get_map<Args>::type&)>>(call_back)...);
				}
			};
		template <KeyTy X>
			struct packet_iterator<X>
			{
				static bool make_match(variant_type& var,
					std::function<void(typename get_map<X>::type&)>&& func)
				{
					if (var.is<typename get_map<X>::type>())
					{
						auto& get_data = var.get<unique<typename get_map<X>::type, X>>().value;
						Reflection<X>::execute(get_data);
						func(get_data);
						return true;
					}
					return false;
				}
			};
		variant_type var;
	public:
		template <KeyTy X>
			bool is() const
			{ return var.is<unique<typename get_map<X>::type, X>>(); }
		template <KeyTy X>
			typename std::add_lvalue_reference<
				typename get_map<X>::type>::type get()
			{ if (is<X>()) {
					auto& get_data = var.get<unique<typename get_map<X>::type, X>>().value;
					Reflection<X>::execute(get_data);
					return get_data;
				} throw Fail<X>(); }
		template <KeyTy... X, typename = typename
				std::enable_if<make_and<
					is_unique<X...>::value,
					contains<X, K...>::value...
				>::value>::type>
			void make_match(std::function<void(
					typename get_map<X>::type&)>&&... call_back)
			{ if (!packet_iterator<X...>::make_match(var,
					std::forward<std::function<void(typename get_map<X>::type&)>>(call_back)...))
				throw Fail<X...>(); }
		bool empty() const { return var.empty(); }
		void reset() { var.reset(); }
	private:
		template <KeyTy X, KeyTy... Args>
			KeyTy get_key_type() const
			{ return is<X>() ? X : get_key_type<Args...>(); }
	public:
		KeyTy type() const { return get_key_type<K...>(); }
		template <KeyTy X, class... Args>
			static basic_packet create(Args&&... args)
			{
				using V = typename get_type_by_index<index_of<KeyTy, X, K...>::value, T...>::type;
				basic_packet result;
				result.var = unique<V, X>({V(std::forward<Args>(args)...)});
				return result;
			}
	};
template <class K, template <K, K...> class F, template <K> class Reflection, class... T>
	using custom_packet = basic_packet<type_hash, K, F, Reflection, T...>;
template <unsigned I, unsigned... Left>
	struct packet_handler: std::bad_cast
		{};
template <unsigned I>
	struct packet_reflection
		{
			template <typename... Args>
			static void execute(Args&&... args){}
		};
template <class... T>
	using packet = custom_packet<unsigned, packet_handler, packet_reflection, T...>;
template <class T, unsigned K>
	struct binding
		{};

#endif
