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

#define BEE_PROPERTY_R(ty, name) \
	public:\
		ty get##name() const\
		{\
			return f##name;\
		}\
	protected:\
		ty f##name

#define BEE_PROPERTY_REF(ty, name) \
	public:\
		void set##name(ty &arg)\
		{\
			f##name = &arg;\
		}\
		void set##name(ty *arg)\
		{\
			f##name = arg;\
		}\
		ty &get##name() const\
		{\
			return *f##name;\
		}\
	protected:\
		ty *f##name

#define BEE_PROPERTY_REF_R(ty, name) \
	public:\
		ty &get##name() const\
		{\
			return *f##name;\
		}\
	protected:\
		ty *f##name


#define BEE_UNIFORM(ty, name, section) \
	protected:\
		gl::UniformRef<ty> g##name = gl::Shader::uniform<ty>(::std::string("g") + (section) + "." #name)

#define BEE_UNIFORM_GLOB(ty, name) \
	protected:\
		gl::UniformRef<ty> g##name = gl::Shader::uniform<ty>("g" #name)



#define BEE_SC_UNIFORM(ty, name) \
	BEE_UNIFORM(ty, name, prefix)

#define BEE_SC_INHERIT(ty, base) \
	private:\
		using Super = base;\
		static constexpr const char *SuperParam = #ty ".Instance[].Base";\
	protected:\
		ty(const ::std::string &prefix): \
			base(prefix + ".Base"), prefix(prefix)\
		{\
		}\
	private:\
		gl::ShaderControllerInfoGetter getInfoFunc() override\
		{\
			return getShaderControllerInfo;\
		}\
		static gl::ShaderControllerInfo *getShaderControllerInfo() \
		{\
			static gl::ShaderControllerInfo info {\
				gl::Shader::uniform<int>("g" #ty ".Count"),\
				#ty,\
			};\
			return & info;\
		}\
	private:\
		const ::std::string prefix = #ty ".Instance[]"

#define BEE_SC_SUPER() \
		Super(SuperParam)

}