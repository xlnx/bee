#ifndef UNIQUE_PTR__VARIANT__HEADER_FILE
#define UNIQUE_PTR__VARIANT__HEADER_FILE
#include "variant.h"
#include <memory>

template <template <typename T> class Hash,
	template <typename U, typename... Left> class Fail,
	class... Hp>
class basic_unique_variant
{
	basic_variant<Hash, Fail, std::unique_ptr<Hp>...> value;
public:
	basic_unique_variant() = default;
	template <typename T, typename... Args>
	static basic_unique_variant create(Args&&... args)
	{
		basic_unique_variant var;
		var.value = std::make_unique<T>(std::forward<Args>(args)...);
		return var;
	}
	template <typename T>
	basic_unique_variant& operator=(T&& v)
	{
		value = std::make_unique<typename std::remove_reference<T>::type>(std::forward<T>(v));
		return *this;
	}
	template<typename T> bool is()
	{
		return value.is<std::unique_ptr<T>>();
	}
	template<typename T> T& get()
	{
		return *value.get<std::unique_ptr<T>>();
	}
};

template <class... Args>
using unique_variant = basic_unique_variant<type_hash, err_handler, Args...>;
template <template <class T, class... Left> class Handler, class... Args>
using callback_unique_variant = basic_unique_variant<type_hash, Handler, Args...>;

#endif
