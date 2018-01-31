#pragma once
#include <vector>
#include <type_traits>
#include <initializer_list>

namespace np
{

struct element			// base of element literal type
{
	long long value;
	constexpr element(long long v):
		value(v) {}
	constexpr element(const element& elem):
		value(elem.value) {}
};

template <typename T, template <typename...> class F>
struct is_instance_of: std::integral_constant<bool, false>
{};
template <typename... Args, template <typename...> class F>
struct is_instance_of<F<Args...>, F>: std::integral_constant<bool, true>
{}; 

template <bool X, bool... Y>
	struct tmp_and: std::integral_constant<bool, X && tmp_and<Y...>::value>
		{};
template <bool X>
	struct tmp_and<X>: std::integral_constant<bool, X>
		{};

template <typename T, typename = typename
	std::enable_if<std::is_class<T>::value>::type>
struct initializer: std::vector<T>
{
	using value_type = T;
	initializer(const T& elem):
		std::vector<T>({elem}) {}
	initializer(const initializer<T>& list):
		std::vector<T>(list) {}
	initializer(const std::initializer_list<T>& list):
		std::vector<T>(list) {}
	template <typename X, typename... Args, typename = typename
			std::enable_if<
				tmp_and<
					std::is_base_of<
						element, X
					>::value,
					std::is_base_of<
						element, Args
					>::value...
				>::value
			>::type
		>
		initializer(const X& elem, const Args&... args):
			initializer(initializer(args...) | elem) {}
	template <typename U, typename = typename
			std::enable_if<
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const U& elem):
			std::vector<T>({T(elem)}) {}
	template <typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const initializer<U>& list)
		{ for (auto& elem: list) std::vector<T>::push_back(elem); }
	template <typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const std::initializer_list<U>& list)
		{ for (auto& elem: list) std::vector<T>::push_back(elem); }
};

using element_list = initializer<element>;
/*template <typename T, typename = typename
	std::enable_if<std::is_base_of<init_element, T>::value>::type>
using init_element_list = initializer<T>;*/


// element concat operator
inline initializer<element> operator + (const element& e1, const element& e2)
{
	return initializer<element>({e1, e2});
}
inline const initializer<element>& operator + (const initializer<element>& list, const element& e)
{
	const_cast<initializer<element>&>(list).push_back(e);
	return list;
}

// concat operator
template <typename T>
inline initializer<T> operator | (const T& e1, const T& e2)
{
	return initializer<T>({e1, e2});
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				!is_instance_of<T, initializer>::value &&
				!is_instance_of<T, std::initializer_list>::value &&
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const U& e1, const T& e2)
{
	return initializer<T>({T(e1), e2});
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				!is_instance_of<T, initializer>::value &&
				!is_instance_of<T, std::initializer_list>::value &&
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const T& e1, const U& e2)
{
	return initializer<T>({e1, T(e2)});
}
template <typename T>
inline const initializer<T>& operator | (const initializer<T>& list, const T& e)
{
	const_cast<initializer<T>&>(list).push_back(e);
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline const initializer<T>& operator | (const initializer<T>& list, const U& e)
{
	const_cast<initializer<T>&>(list).push_back(T(e));
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const initializer<U>& list, const T& e)
{
	auto l = initializer<T>(list);
	l.push_back(e);
	return l;
}

}
