#ifndef __CUSTOM_VARIANT__HEADER_FILE
#define __CUSTOM_VARIANT__HEADER_FILE
#include <typeindex>
#include <utility>
#include <functional>

template <template <typename T> class F, typename T, typename... Args>
struct max_value
{
	static constexpr typename F<T>::value_type value = F<T>::value > max_value<F, Args...>::value ?
			F<T>::value : max_value<F, Args...>::value;
};

template <template <typename T> class F, typename T>
struct max_value<F, T>
{
	static constexpr typename F<T>::value_type value = F<T>::value;
};

template <bool X, bool... Y>
struct make_and
{
	static constexpr bool value = X && make_and<Y...>::value;
};

template <bool X>
struct make_and<X>
{
	static constexpr bool value = X;
};

template <typename T, typename U, typename... Args>
struct contains
{
	static constexpr bool value = std::is_same<T, U>::value ?
		true : contains<T, Args...>::value;
};

template <typename T, typename U>
struct contains<T, U>
{
	static constexpr bool value = std::is_same<T, U>::value;
};

template <typename X, typename... Y>
struct is_unique
{
	static constexpr bool value = !contains<X, Y...>::value && is_unique<Y...>::value;
};

template <typename X>
struct is_unique<X>
{
	static constexpr bool value = true;
};

template <typename T>
struct size_of
{
	using value_type = unsigned;
	static constexpr value_type value = sizeof(T);
};

template <typename T>
struct align_of
{
	using value_type = unsigned;
	static constexpr value_type value = std::alignment_of<T>::value;
};

template <template <typename T> class Hash,
	template <typename U, typename... Left> class Fail,
	typename... Args>
class basic_variant;

template <typename T, typename... Args>
struct variant_helper
{
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void destroy(
			basic_variant<Hash, Fail, Y...>& var);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void copy(
			basic_variant<Hash, Fail, Y...>& dest,
			const basic_variant<Hash, Fail, Y...>& src);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void move(
			basic_variant<Hash, Fail, Y...>& dest,
			basic_variant<Hash, Fail, Y...>&& src);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void copy_assign(
			basic_variant<Hash, Fail, Y...>& dest,
			const basic_variant<Hash, Fail, Y...>& src);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void move_assign(
			basic_variant<Hash, Fail, Y...>& dest,
			basic_variant<Hash, Fail, Y...>&& src);
};

template <typename T>
struct variant_helper<T>
{
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void destroy(
			basic_variant<Hash, Fail, Y...>& var);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void copy(
			basic_variant<Hash, Fail, Y...>& dest,
			const basic_variant<Hash, Fail, Y...>& src);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void move(
			basic_variant<Hash, Fail, Y...>& dest,
			basic_variant<Hash, Fail, Y...>&& src);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void copy_assign(
			basic_variant<Hash, Fail, Y...>& dest,
			const basic_variant<Hash, Fail, Y...>& src);
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static void move_assign(
			basic_variant<Hash, Fail, Y...>& dest,
			basic_variant<Hash, Fail, Y...>&& src);
};

template <typename T, typename... Args>
struct variant_iterator
{
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static bool make_match(basic_variant<Hash, Fail, Y...>& var,
			std::function<void(T&)>&& func,
			std::function<void(Args&)>&&... call_back);
};

template <typename T>
struct variant_iterator<T>
{
	template <template <typename X> class Hash,
			template <typename U, typename... Left> class Fail,
			typename... Y>
		static bool make_match(basic_variant<Hash, Fail, Y...>& var,
			std::function<void(T&)>&& func);
};

template <template <typename T> class Hash,
	template <typename U, typename... Left> class Fail,
	typename... Args>
class basic_variant final
{
	using check_valid = typename std::enable_if<make_and<
			is_unique<Args...>::value,
			!std::is_function<Args>::value...,
			!std::is_void<Args>::value...,
			!std::is_array<Args>::value...,
			!std::is_reference<Args>::value...
		>::value>::type;
	using helper = variant_helper<Args...>;
	template <typename X, typename... Y>
	friend struct variant_helper;
	template <typename X, typename... Y>
	friend struct variant_iterator;
	typename std::aligned_storage<max_value<size_of, Args...>::value,
		max_value<align_of, Args...>::value>::type buffer;
	typename Hash<void>::value_type m_type;
public:
	basic_variant(): m_type(Hash<void>::value) {}
	~basic_variant()
	{
		if (m_type != Hash<void>::value) helper::destroy(*this);
	}
	// copy/move ctor (std::forward)
	template <typename T,
		typename = typename std::enable_if<contains<typename std::remove_reference<T>::type, Args...>::value>::type>
	basic_variant(T&& v): m_type(Hash<T>::value)
	{
		using U = typename std::remove_reference<T>::type;
		new(&buffer) U(std::forward<T>(v));
	}
	basic_variant(basic_variant&& v): m_type(v.m_type)
	{
		helper::move(*this, std::move(v));
	}
	basic_variant(const basic_variant& v): m_type(v.m_type)
	{
		helper::copy(*this, std::move(v));
	}
	template <typename T,
		typename = typename std::enable_if<contains<typename std::remove_reference<T>::type, Args...>::value>::type>
	basic_variant& operator=(T&& v)
	{
		using U = typename std::remove_reference<T>::type;
		if (m_type == Hash<U>::value)
		{	// v and this are of the same type, call the corresponding assign operator
			reinterpret_cast<U&>(buffer) = std::forward<T>(v);
		}
		else
		{	// destroy the old variant, and call the corresponding ctor
			if (m_type != Hash<void>::value) helper::destroy(*this);
			new(&buffer) U(std::forward<T>(v));
			m_type = Hash<U>::value;
		}
		return *this;
	}
	basic_variant& operator=(const basic_variant& v)
	{
		if (v.m_type == m_type)
		{	// v and this are of the same type, use copy assign operator
			helper::copy_assign(*this, v);
		}
		else
		{	// type not same, call this dtor and use copy ctor
			if (m_type != Hash<void>::value) helper::destroy(*this);
			helper::copy(*this, v); m_type = v.m_type;
		}
		return *this;
	}
	basic_variant& operator=(basic_variant&& v)
	{
		if (v.m_type == m_type)
		{	// v and this are of the same type, use move assign operator
			helper::move_assign(*this, std::move(v));
		}
		else
		{	// type not same, call this dtor and use move ctor
			if (m_type != Hash<void>::value) helper::destroy(*this);
			helper::move(*this, std::move(v)); m_type = v.m_type;
		}
		return *this;
	}
public:
	template <typename T> bool is() const
	{
		return m_type == Hash<T>::value;
	}
	template <typename T> T& get()
	{
		if (is<T>()) return reinterpret_cast<T&>(buffer);
			Fail<T>::execute();
	}
	template <typename... Y,
		typename = typename std::enable_if<make_and<
				is_unique<Y...>::value,
				contains<Y, Args...>::value...
			>::value>::type>
	void make_match(std::function<void(Y&)>&&... call_back)
	{
		if (!variant_iterator<Y...>::make_match(*this,
				std::forward<std::function<void(Y&)>>(call_back)...))
			Fail<Y...>::execute();
	}
	typename Hash<void>::value_type type() const
	{
		return m_type;
	}
	bool empty() const
	{
		return m_type == Hash<void>::value;
	}
	void reset()
	{
		if (m_type != Hash<void>::value) helper::destroy(*this);
	}
};

template <typename T, typename... Args>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T, Args...>::destroy(
		basic_variant<Hash, Fail, Y...>& var)
	{
		if (var.m_type == Hash<T>::value)
			reinterpret_cast<T&>(var.buffer).~T();
		else
			variant_helper<Args...>::destroy(var);
	}
	// copy ctor
template <typename T, typename... Args>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T, Args...>::copy(
		basic_variant<Hash, Fail, Y...>& dest,
		const basic_variant<Hash, Fail, Y...>& src)
	{
		if (src.m_type == Hash<T>::value)
			new(&dest.buffer) T(reinterpret_cast<const T&>(src.buffer));
		else
			variant_helper<Args...>::copy(dest, src);
	}
	// move ctor
template <typename T, typename... Args>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T, Args...>::move(
		basic_variant<Hash, Fail, Y...>& dest,
		basic_variant<Hash, Fail, Y...>&& src)
	{
		if (src.m_type == Hash<T>::value)
			new(&dest.buffer) T(std::move(reinterpret_cast<T&>(src.buffer)));
		else
			variant_helper<Args...>::move(dest, std::move(src));
	}
	// copy assign operator
template <typename T, typename... Args>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T, Args...>::copy_assign(
		basic_variant<Hash, Fail, Y...>& dest,
		const basic_variant<Hash, Fail, Y...>& src)
	{
		if (src.m_type == Hash<T>::value)
			reinterpret_cast<T&>(dest.buffer) = reinterpret_cast<const T&>(src.buffer);
		else
			variant_helper<Args...>::copy_assign(dest, src);
	}
	// move assign operator
template <typename T, typename... Args>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T, Args...>::move_assign(
		basic_variant<Hash, Fail, Y...>& dest,
		basic_variant<Hash, Fail, Y...>&& src)
	{
		if (src.m_type == Hash<T>::value)
			reinterpret_cast<T&>(dest.buffer) = std::move(reinterpret_cast<T&>(src.buffer));
		else
			variant_helper<Args...>::move_assign(dest, std::move(src));
	}

template <typename T>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T>::destroy(
		basic_variant<Hash, Fail, Y...>& var)
	{
		reinterpret_cast<T&>(var.buffer).~T();
	}
template <typename T>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T>::copy(
		basic_variant<Hash, Fail, Y...>& dest,
		const basic_variant<Hash, Fail, Y...>& src)
	{
		new(&dest.buffer) T(reinterpret_cast<const T&>(src.buffer));
	}
template <typename T>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T>::move(
		basic_variant<Hash, Fail, Y...>& dest,
		basic_variant<Hash, Fail, Y...>&& src)
	{
		new(&dest.buffer) T(std::move(reinterpret_cast<T&>(src.buffer)));
	}
template <typename T>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T>::copy_assign(
		basic_variant<Hash, Fail, Y...>& dest,
		const basic_variant<Hash, Fail, Y...>& src)
	{	
		reinterpret_cast<T&>(dest.buffer) = reinterpret_cast<const T&>(src.buffer);
	}
template <typename T>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	void variant_helper<T>::move_assign(
		basic_variant<Hash, Fail, Y...>& dest,
		basic_variant<Hash, Fail, Y...>&& src)
	{
		reinterpret_cast<T&>(dest.buffer) = std::move(reinterpret_cast<T&>(src.buffer));
	}


template <typename T, typename... Args>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	bool variant_iterator<T, Args...>::make_match(basic_variant<Hash, Fail, Y...>& var,
		std::function<void(T&)>&& func,
		std::function<void(Args&)>&&... call_back)
	{
		if (Hash<T>::value == var.m_type)
		{
			func(reinterpret_cast<T&>(var.buffer));
			return true;
		}
		return variant_iterator<Args...>::make_match(var,
			std::forward<std::function<void(Args&)>>(call_back)...);
	}
template <typename T>
template <template <typename X> class Hash,
		template <typename U, typename... Left> class Fail,
		typename... Y>
	bool variant_iterator<T>::make_match(basic_variant<Hash, Fail, Y...>& var,
		std::function<void(T&)>&& func)
	{
		if (Hash<T>::value == var.m_type)
		{
			func(reinterpret_cast<T&>(var.buffer));
			return true;
		}
		return false;
	}


template <class T>
struct type_hash
{
	using value_type = std::type_index;
	static const value_type value;
};
template <class T>
const typename type_hash<T>::value_type type_hash<T>::value = std::type_index(typeid(T));

template <class T, class... Args>
struct err_handler
{
	static void execute()
	{
		throw std::bad_cast();
	}
};

template <class... Args>
using variant = basic_variant<type_hash, err_handler, Args...>;
template <template <class T, class... Left> class Handler, class... Args>
using callback_variant = basic_variant<type_hash, Handler, Args...>;

#endif
#include "unique_variant.h"
