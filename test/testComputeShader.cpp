#include "bee.h"

using namespace std;
using namespace bee;
using namespace bee::gl;
using namespace glm;

Window<4, 2> window("fuck", false, 512, 512);

GLuint genTexture()
{
	return *new Texture2D();
}

GLuint genComputeProg(GLuint texHandle) {
	// Creating the compute shader, and the program object containing the shader
	GLuint progHandle = glCreateProgram();
	GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

	// In order to write to a texture, we have to introduce it as image2D.
	// local_size_x/y/z layout variables define the work group size.
	// gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
	// gl_LocalInvocationID is the local index within the work group, and
	// gl_WorkGroupID is the work group's index
	const char *csSrc[] = {
		"#version 430\n",
		"uniform float roll;\
		 uniform image2D destTex;\
		 layout (local_size_x = 16, local_size_y = 16) in;\
		 void main() {\
			 ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);\
			 float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);\
			 float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;\
			 imageStore(destTex, storePos, vec4(1.0-globalCoef*localCoef, 0.0, 0.0, 0.0));\
		 }"
	};

	glShaderSource(cs, 2, csSrc, NULL);
	glCompileShader(cs);
	int rvalue;
	glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in compiling the compute shader\n");
		GLchar log[10240];
		GLsizei length;
		glGetShaderInfoLog(cs, 10239, &length, log);
		fprintf(stderr, "Compiler log:\n%s\n", log);
		exit(40);
	}
	glAttachShader(progHandle, cs);

	glLinkProgram(progHandle);
	glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in linking compute shader program\n");
		GLchar log[10240];
		GLsizei length;
		glGetProgramInfoLog(progHandle, 10239, &length, log);
		fprintf(stderr, "Linker log:\n%s\n", log);
		exit(41);
	}   
	glUseProgram(progHandle);
	
	glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);

	checkErrors("Compute shader");
	return progHandle;
}

void updateTex(int frame) {
	glUseProgram(computeHandle);
	glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)frame*0.01f);
	glDispatchCompute(512/16, 512/16, 1); // 512^2 threads in blocks of 16^2
	checkErrors("Dispatch compute shader");
}

void draw() {
	glUseProgram(renderHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	swapBuffers();
	checkErrors("Draw screen");
}

int Main(int argc, char *argv[])
{
	Shader &shader = Shader::load(
		// VertexShader
		// FragmentShader
		"testcs",
		ComputeShader("test-cs.glsl")
	);
	window.dispatch<RenderEvent>([&]() -> bool {
		shader.use();
		glDispatchCompute(1, 1, 1);
		shader.unuse();
		return false;
	});
	window.dispatchMessages();
	return 0;
}