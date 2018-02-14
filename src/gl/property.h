#pragma once 

namespace bee
{

#define BEE_PROPERTY(ty, name) \
	public:\
		template <typename ...Types, typename = typename\
			::std::enable_if<::std::is_constructible<ty, Types...>::value>::type>\
		void set##name(Types &&...args)\
		{\
			f##name = ty(::std::forward<Types>(args)...);\
		}\
		ty get##name() const\
		{\
			return f##name;\
		}\
	protected:\
		ty f##name

#define BEE_PROPERTY_LARGE(ty, name) \
	public:\
		template <typename ...Types, typename = typename\
			::std::enable_if<::std::is_constructible<ty, Types...>::value>::type>\
		void set##name(Types &&...args)\
		{\
			f##name = ty(::std::forward<Types>(args)...);\
		}\
		const ty &get##name() const\
		{\
			return f##name;\
		}\
	protected:\
		ty f##name

#define BEE_UNIFORM(ty, name, section) \
	protected:\
		gl::UniformRef<ty> g##name = gl::Shader::uniform<ty>(::std::string("g") + (section) + "." #name)

#define BEE_UNIFORM_GLOB(ty, name) \
	protected:\
		gl::UniformRef<ty> g##name = gl::Shader::uniform<ty>("g" #name)


#define BEE_SC_BASE(ty) \
	protected:\
		ty(const ::std::string &prefix): \
			prefix(prefix)\
		{\
		}\
	private:\
		const ::std::string &prefix = #ty

#define BEE_SC_BASE_MULTI(ty) \
	protected:\
		ty(const ::std::string &prefix): \
			prefix(prefix)\
		{\
		}\
	private:\
		const ::std::string &prefix = #ty "[]"

#define BEE_SC_UNIFORM(ty, name) \
	BEE_UNIFORM(ty, name, prefix)

#define BEE_SC_INHERIT(ty, base) \
	private:\
		using Super = base;\
		static constexpr const char *SuperParam = #ty ".Base";\
	protected:\
		ty(const ::std::string &prefix): \
			base(prefix + ".Base"), prefix(prefix)\
		{\
		}\
	private:\
		const ::std::string &prefix = #ty

#define BEE_SC_INHERIT_MULTI(ty, base) \
	private:\
		using Super = base;\
		static constexpr const char *SuperParam = #ty "[].Base";\
	protected:\
		ty(const ::std::string &prefix): \
			base(prefix + ".Base"), prefix(prefix)\
		{\
		}\
	private:\
		const ::std::string &prefix = #ty "[]"

#define BEE_SC_SUPER() \
		Super(SuperParam)

}