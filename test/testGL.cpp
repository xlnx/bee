#include "runtime.h"
#include "gl.h"

struct MyWindow: ::bee::gl::Window
{
public:
	template <typename ...Types>
		MyWindow(Types &&...args):
			::bee::gl::Window(std::forward<Types>(args)...)
	{
		// Specify OpenGL Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	}
};

MyWindow window("testGL", false, 512, 256);

::bee::gl::UniformRef<::glm::mat4> world;

::bee::gl::IBOPrimitive prim;
// Primitive prim;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.001f;
	// move
	::glm::mat4 move = {
		1.f, 0.f, 0.f, sinf(Scale),
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	// rotate z
	::glm::mat4 rotatez = {
		cosf(Scale), -sinf(Scale), 0.f, 0.f,
		sinf(Scale), cosf(Scale), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	// rotate y
	::glm::mat4 rotatey = {
		cosf(Scale), 0.f, -sinf(Scale), 0.f,
		0.f, 1.f, 0.f, 0.f,
		sinf(Scale), 0.f, cosf(Scale), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	// rotate x
	::glm::mat4 rotatex = {
		1.f, 0.f, 0.f, 0.f,
		0.f, cosf(Scale), -sinf(Scale), 0.f,
		0.f, sinf(Scale), cosf(Scale), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	::glm::mat4 scale = {
		cosf(Scale), 0.f, 0.f, 0.f,
		0.f, cosf(Scale), 0.f, 0.f,
		0.f, 0.f, cosf(Scale), 0.f, 
		0.f, 0.f, 0.f, 1.f
	};
	world = rotatey; // * rotatex; // * scale;

	// glVertexAttribPointer(0/*floatType*/, 3/*3 elements*/,  
	// 	GL_FLOAT/*elemType*/, GL_FALSE, 7 * sizeof(float)/*vectorSize*/, nullptr);

	::bee::gl::use<::bee::gl::color3, ::bee::gl::pos3>();
	glEnableVertexAttribArray(0);
		prim.draw();
	glDisableVertexAttribArray(0);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void init()
{
	const ::bee::gl::VertexAttrs<::bee::gl::color3, ::bee::gl::pos3> vertices = {
		{ {1, 2, 3}, {-1, -1, 0} },
		{ {1, 2, 3}, {0, -1, 1} },
		{ {1, 2, 3}, {1, -1, 0} },
		{ {1, 2, 3}, {0, 1, 0} },
	};
	// const ::bee::gl::VertexAttrs<::bee::gl::color3, ::bee::gl::pos3> vertices = {
	// 	{ {}, {-1, -1, 0} },
	// 	{ {}, {0, 1, 0} },
	// 	{ {}, {0, -1, 1} },
		
	// 	{ {}, {0, -1, 1} },
	// 	{ {}, {0, 1, 0} },
	// 	{ {}, {1, -1, 0} },

	// 	{ {}, {-1, -1, 0} },
	// 	{ {}, {0, 1, 0} },
	// 	{ {}, {1, -1, 0} },

	// 	{ {}, {-1, -1, 0} },
	// 	{ {}, {0, -1, 1} },
	// 	{ {}, {1, -1, 0} },
	// };
	// vertices[0].get<::bee::gl::pos3>() = {-1, -1, -1};
	BEE_LOG(vertices[0].get<::bee::gl::position>()[1]);
	::bee::gl::Indices indices = {
		{ 0, 3, 1 },
		{ 1, 3, 2 },
		{ 0, 3, 2 },
		{ 0, 1, 2 }
	};
	// prim = Primitive(vertices); 
	prim = ::bee::gl::IBOPrimitive(vertices, indices);
	vertices.use();
	// TODO: Perform more shader Usage
	::bee::gl::Shader shader{
		::bee::gl::VertexShader("test.vert"),
		::bee::gl::FragmentShader("test.frag")
	};
	shader.use();
	// glUseProgram(shader);
	world.bind(shader.uniform<::glm::mat4>("gWorld"));
}

int Main(int argc, char **argv)
{
	// -DBEE_RUNTIME_INTRUSIVE
	// ::bee::Runtime::exec([]()
	// {
	init();
	while (!glfwWindowShouldClose(window))
	{
		bee::gl::exec(render);
	}
	// });
	return 0;
}