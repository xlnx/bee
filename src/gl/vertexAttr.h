#pragma once 

#include "common.h"
#include <cstdlib>
#include <tuple>
#include <initializer_list>
#include <array>
#include <vector>

namespace bee
{

namespace gl
{

template <typename ...Attrs>
struct VertexAttr;

template <typename ...Attrs>
struct VertexAttrs;


enum VertexAttrType { 
	position = 0, color = 1, normal = 2, tangent = 3, 
	bitangent = 4, textureCoord = 5 };

constexpr GLenum vertexAttrTypeBegin = position; 
constexpr GLenum vertexAttrTypeEnd = textureCoord + 1; 

template <typename V>
struct GlmVectorInfo;
template <typename T, ::glm::precision P>
struct GlmVectorInfo<::glm::tvec1<T, P>>
{ using type = T; static constexpr ::glm::precision prec = P; static constexpr ::std::size_t size = 1; };
template <typename T, ::glm::precision P>
struct GlmVectorInfo<::glm::tvec2<T, P>>
{ using type = T; static constexpr ::glm::precision prec = P; static constexpr ::std::size_t size = 2; };
template <typename T, ::glm::precision P>
struct GlmVectorInfo<::glm::tvec3<T, P>>
{ using type = T; static constexpr ::glm::precision prec = P; static constexpr ::std::size_t size = 3; };
template <typename T, ::glm::precision P>
struct GlmVectorInfo<::glm::tvec4<T, P>>
{ using type = T; static constexpr ::glm::precision prec = P; static constexpr ::std::size_t size = 4; };

template <typename V, template <typename, ::glm::precision> typename T>
struct IsInstanceOfAux
{ static constexpr bool value = false; };
template <template <typename, ::glm::precision> typename T, typename Ty, ::glm::precision P>
struct IsInstanceOfAux<T<Ty, P>, T>
{ static constexpr bool value = true; };

template <VertexAttrType Ty, typename V, typename = typename
	::std::enable_if<
		IsInstanceOfAux<V, ::glm::tvec1>::value |
		IsInstanceOfAux<V, ::glm::tvec2>::value |
		IsInstanceOfAux<V, ::glm::tvec3>::value |
		IsInstanceOfAux<V, ::glm::tvec4>::value
	>::type>
struct VertexAttrElem
{
	constexpr static VertexAttrType type = Ty;
	constexpr static ::std::size_t size = GlmVectorInfo<V>::size;
	using elemType = typename GlmVectorInfo<V>::type;
	constexpr static ::glm::precision prec = GlmVectorInfo<V>::prec;
private:
	VertexAttrElem() = default;
};

using any = void;
using pos3 = VertexAttrElem<position, ::glm::vec3>;
using pos4 = VertexAttrElem<position, ::glm::vec4>;
using color3 = VertexAttrElem<color, ::glm::vec3>;
using color4 = VertexAttrElem<color, ::glm::vec4>;
using norm3 = VertexAttrElem<normal, ::glm::vec3>;
using norm4 = VertexAttrElem<normal, ::glm::vec4>;
using tg3 = VertexAttrElem<tangent, ::glm::vec3>;
using tg4 = VertexAttrElem<tangent, ::glm::vec4>;
using bitg3 = VertexAttrElem<bitangent, ::glm::vec3>;
using bitg4 = VertexAttrElem<bitangent, ::glm::vec4>;
using tex3 = VertexAttrElem<textureCoord, ::glm::vec3>;
using tex4 = VertexAttrElem<textureCoord, ::glm::vec4>;


template <VertexAttrType T, VertexAttrType Y, VertexAttrType ...Types>
struct TypeContains
{ static constexpr bool value = TypeContains<T, Types...>::value; };
template <VertexAttrType T, VertexAttrType ...Types>
struct TypeContains<T, T, Types...>
{ static constexpr bool value = true; };
template <VertexAttrType T, VertexAttrType Y>
struct TypeContains<T, Y>
{ static constexpr bool value = false; };
template <VertexAttrType T>
struct TypeContains<T, T>
{ static constexpr bool value = true; };
template <VertexAttrType T, VertexAttrType ...Types>
struct AreDifferent
{ static constexpr bool value = AreDifferent<Types...>::value && !TypeContains<T, Types...>::value; };
template <VertexAttrType T>
struct AreDifferent<T>
{ static constexpr bool value = true; };

template <::std::size_t N, typename T, ::glm::precision P>
struct VertexAttrStorageAux;
template <typename T, ::glm::precision P>
struct VertexAttrStorageAux<1, T, P>
{ using type = ::glm::tvec1<T, P>; };
template <typename T, ::glm::precision P>
struct VertexAttrStorageAux<2, T, P>
{ using type = ::glm::tvec2<T, P>; };
template <typename T, ::glm::precision P>
struct VertexAttrStorageAux<3, T, P>
{ using type = ::glm::tvec3<T, P>; };
template <typename T, ::glm::precision P>
struct VertexAttrStorageAux<4, T, P>
{ using type = ::glm::tvec4<T, P>; };

template <typename A>
struct VertexAttrStorage: VertexAttrStorageAux<A::size, typename A::elemType, A::prec>
{};

template <typename T>
struct VertexAttrSignature;
template <>
struct VertexAttrSignature<float>
{ constexpr static GLenum value = GL_FLOAT; };
template <>
struct VertexAttrSignature<double>
{ constexpr static GLenum value = GL_DOUBLE; };
template <>
struct VertexAttrSignature<int>
{ constexpr static GLenum value = GL_INT; };
template <>
struct VertexAttrSignature<unsigned>
{ constexpr static GLenum value = GL_UNSIGNED_INT; };

template <::std::size_t N, typename A, typename B, typename ...Attrs>
struct IndexOfAttrAux: IndexOfAttrAux<N + 1, A, Attrs...>
{};
template <::std::size_t N, typename A, typename ...Attrs>
struct IndexOfAttrAux<N, A, A, Attrs...>
{ constexpr static ::std::size_t value = N; };

template <typename A, typename ...Attrs>
struct IndexOfAttr: IndexOfAttrAux<0, A, Attrs...>
{};

template <bool X, typename TrueType, typename FalseType>
struct IfTypeAux
{ using type = TrueType; };
template <typename TrueType, typename FalseType>
struct IfTypeAux<false, TrueType, FalseType>
{ using type = FalseType; };

template <VertexAttrType Ty, typename A, typename ...Attrs>
struct VertexAttrStorageByType
{
	using type = typename IfTypeAux<A::type == Ty,
		typename VertexAttrStorage<A>::type,
		typename VertexAttrStorageByType<Ty, Attrs...>::type
	>::type;
	using attr = typename IfTypeAux<A::type == Ty,
		A, typename VertexAttrStorageByType<Ty, Attrs...>::attr
	>::type;
};
template <VertexAttrType Ty, typename A>
struct VertexAttrStorageByType<Ty, A>
{
	using type = typename IfTypeAux<A::type == Ty,
		typename VertexAttrStorage<A>::type,
		void
	>::type;
	using attr = typename IfTypeAux<A::type == Ty,
		A, void
	>::type;
};

template <typename ...Attrs>
struct VertexAttr
{
	static_assert(AreDifferent<Attrs::type...>::value, "All the components of VertexAttr object must have different types.");
	template <VertexAttrType Ty>
	constexpr typename VertexAttrStorageByType<Ty, Attrs...>::type &get() noexcept;
	template <VertexAttrType Ty>
	constexpr const typename VertexAttrStorageByType<Ty, Attrs...>::type &get() const noexcept;
	template <typename A>
	constexpr typename VertexAttrStorage<A>::type &get() noexcept;
	template <typename A>
	constexpr const typename VertexAttrStorage<A>::type &get() const noexcept;

	using value_type = ::std::tuple<typename VertexAttrStorage<Attrs>::type...>;
	value_type data;
};

template <typename ...Attrs>
template <VertexAttrType Ty>
	constexpr typename VertexAttrStorageByType<Ty, Attrs...>::type &
		VertexAttr<Attrs...>::get() noexcept
{
	return ::std::get<IndexOfAttr<typename 
		VertexAttrStorageByType<Ty, Attrs...>::attr, Attrs...>::value>(data);
}
template <typename ...Attrs>
template <VertexAttrType Ty>
	constexpr const typename VertexAttrStorageByType<Ty, Attrs...>::type &
		VertexAttr<Attrs...>::get() const noexcept
{ 
	return ::std::get<IndexOfAttr<typename 
		VertexAttrStorageByType<Ty, Attrs...>::attr, Attrs...>::value>(data);
}

template <typename ...Attrs>
template <typename A>
	constexpr typename VertexAttrStorage<A>::type &
		VertexAttr<Attrs...>::get() noexcept
{
	return ::std::get<IndexOfAttr<A, Attrs...>::value>(data);
}
template <typename ...Attrs>
template <typename A>
	constexpr const typename VertexAttrStorage<A>::type &
		VertexAttr<Attrs...>::get() const noexcept
{ 
	return ::std::get<IndexOfAttr<A, Attrs...>::value>(data);
}

template <typename T>
struct ArrayMemoryManager
{
	using elemType = T;
	constexpr ArrayMemoryManager(const ::std::initializer_list<elemType> &l):
		num(l.size()), dataptr(new elemType[l.size()])
	{ ::memcpy(dataptr, l.begin(), sizeof(elemType) * num); }
	constexpr ArrayMemoryManager(::std::size_t size):
		num(size), dataptr(new elemType[size])
	{}
	constexpr ArrayMemoryManager(const ArrayMemoryManager &other):
		num(other.num), dataptr(new elemType[other.num])
	{ ::memcpy(dataptr, other.dataptr, sizeof(elemType) * num); }
	constexpr ArrayMemoryManager(ArrayMemoryManager &&other):
		num(other.num), dataptr(other.dataptr)
	{ other.dataptr = nullptr; }
	ArrayMemoryManager &operator = (const ArrayMemoryManager &other) = delete;
	~ArrayMemoryManager()
	{ delete [] dataptr; }

	constexpr ::std::size_t size() const noexcept
	{ return num; }
	constexpr elemType &operator [] (::std::size_t index) noexcept
	{ return dataptr[index]; }
	constexpr const elemType &operator [] (::std::size_t index) const noexcept
	{ return dataptr[index]; }
	constexpr elemType *begin() const noexcept
	{ return dataptr; }
	constexpr elemType *end() const noexcept
	{ return dataptr + num; }

	constexpr static ::std::size_t elemSize = sizeof(elemType);
protected:
	const ::std::size_t num;
	elemType *dataptr;
};

template <typename ...Types>
constexpr void dummyExpand(Types ...)
{
}
template <typename A, typename E>
	int dummyExpandAux()
{
	glEnableVertexAttribArray(A::type);
	glVertexAttribPointer(A::type, A::size, 
 		VertexAttrSignature<typename A::elemType>::value, GL_FALSE, sizeof(E), 
		 	(void*)&((E*)nullptr)->template get<A::type>());
	glDisableVertexAttribArray(A::type);
	return 0;
}

struct VertexAttrTypeDyn
{
	VertexAttrType type;
	::std::size_t size;
	GLenum elemType;
	char *offset;
};

template <typename ...>
struct VertexAttrs;

struct VertexAttrEnabledInfo
{
	friend class VertexAttrs<any>;
	VertexAttrEnabledInfo():
		v{false}, dynamic(true)
	{}
	constexpr VertexAttrEnabledInfo(const ::std::initializer_list<VertexAttrType> &l):
		l(l), v{false}, dynamic(false)
	{}
	void invoke() const
	{
		if (!dynamic)
		{
			for (auto i = vertexAttrTypeBegin; i != vertexAttrTypeEnd; ++i)
				glDisableVertexAttribArray(i);
			for (auto type: l) glEnableVertexAttribArray(type);
		}
		else
		{
			for (auto i = vertexAttrTypeBegin; i != vertexAttrTypeEnd; ++i)
			{
				if (!v[i])
				{
					glDisableVertexAttribArray(i);
				}
				else
				{
					glEnableVertexAttribArray(i);
				}
			}
		}
	}
	void push(VertexAttrType type)
	{
		v[type] = true;
	}
private:
	::std::initializer_list<VertexAttrType> l;
	::std::array<bool, vertexAttrTypeEnd> v;
	bool dynamic;
};

template <typename ...Attrs>
struct VertexAttrs: public ArrayMemoryManager<VertexAttr<Attrs...>>
{
	using Super = ArrayMemoryManager<VertexAttr<Attrs...>>;
	using elemType = typename Super::elemType;
	// typename elemType::value_type
	constexpr VertexAttrs(const ::std::initializer_list<typename elemType::value_type> &l):
		Super(reinterpret_cast<const ::std::initializer_list<elemType>&>(l))
	{}
	constexpr VertexAttrs(::std::size_t size):
		Super(size)
	{}
	constexpr static void setVertexAttribute()
	{ dummyExpand(dummyExpandAux<Attrs, elemType>()...); }
	constexpr static VertexAttrEnabledInfo info = {Attrs::type...};
};

template <>
struct VertexAttrs<any>
{
	using Super = void;
	// using elemType = VertexAttr<any>;

	VertexAttrs(::std::size_t size):
		elemSize(0), dataptr(nullptr), num(size)
	{}
	VertexAttrs(const VertexAttrs &other):
		elemSize(other.elemSize),
		dataptr(new char [other.num * other.elemSize]),
		num(other.num),
		dyninfo(other.dyninfo)
	{
		::memcpy(dataptr, other.dataptr, num * elemSize);
	}
	VertexAttrs(VertexAttrs &&other):
		elemSize(other.elemSize),
		dataptr(other.dataptr),
		num(other.num),
		dyninfo(other.dyninfo)
	{
		other.dataptr = nullptr;
	}
	VertexAttrs &operator = (const VertexAttrs &other) = delete;
	~VertexAttrs()
	{
		delete [] dataptr;
	}

	template <typename A>
	bool invoke(bool enable = true)
	{
		if (enable)
		{
			dyninfo[A::type] = {
				A::type, A::size, 
				VertexAttrSignature<typename A::elemType>::value,
				(char*)nullptr + elemSize
			};
			info.push(A::type);
			elemSize += sizeof(typename VertexAttrStorage<A>::type);
		}
		return enable;
	}
	template <typename A>
	typename VertexAttrStorage<A>::type &get(::std::size_t index) noexcept
	{
		return *reinterpret_cast<typename VertexAttrStorage<A>::type*>(
			dataptr + index * elemSize + (dyninfo[A::type].offset - (char*)nullptr) );
	}
	void alloc()
	{
		delete [] dataptr;
		dataptr = new char[num * elemSize];
	}
	::std::size_t size() const
	{ return num; }
	// elemType &operator [] (::std::size_t index) noexcept;
	// { return *reinterpret_cast<elemType*>(dataptr + index * elemSize); }
	// const elemType &operator [] (::std::size_t index) const noexcept;
	// { return *reinterpret_cast<const elemType*>(dataptr + index * elemSize); }
	char *begin() const noexcept
	{ return dataptr; }
	// const elemType *begin() const noexcept
	// { return dataptr; }
	// elemType *end() noexcept
	// { return dataptr + num * elemSize; }
	// const elemType *end() const noexcept
	// { return dataptr + num * elemSize; }
	void setVertexAttribute() const
	{
		for (auto &a: dyninfo)
		{
			glEnableVertexAttribArray(a.type);
			glVertexAttribPointer(a.type, a.size, a.elemType, GL_FALSE, elemSize, (void*)a.offset);
			glDisableVertexAttribArray(a.type);
		}
	}
	VertexAttrEnabledInfo info;
	::std::size_t elemSize;
private:
	char *dataptr;
	::std::size_t num;
	::std::array<VertexAttrTypeDyn, vertexAttrTypeEnd> dyninfo;
};

// template <typename ...Attrs>
// 	void use()
// {
// 	dummyExpand(dummyExpandAux<Attrs, VertexAttr<Attrs...>>()...);
// }

}

}