#include "shader.h"

namespace bee
{

namespace gl
{

Shader *Shader::currShader = nullptr;
Shader::ShaderRec Shader::shaderHead;
::std::map<::std::string, int> Shader::uniformIndex;
::std::vector<::std::string> Shader::registeredUniforms;

}

}