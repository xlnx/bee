#include "shader.h"

namespace bee
{

namespace gl
{

::std::map<::std::string, int> *Shader::uniformIndex = nullptr;
::std::vector<::std::string> *Shader::registeredUniforms = nullptr;

::std::map<::std::string, int> *Shader::uniformArrayIndex = nullptr;
::std::vector<::std::pair<::std::string, ::std::string>> *Shader::registeredUniformArrays = nullptr;

Shader *Shader::currShader = nullptr;
Shader::ShaderRec Shader::shaderHead;

}

}