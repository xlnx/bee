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

class IBOPrimitive
{
public:
	IBOPrimitive(): IBO(0), VBO(0) {}
	template <typename ...Types>
	IBOPrimitive(const ::bee::gl::VertexAttrs<Types...> &vertices, 
			unsigned int indices[], unsigned M):
		posvecsize(sizeof(vertices[0].get<::bee::gl::position>(0)))
	{
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, M*sizeof(indices[0]), indices, GL_STATIC_DRAW);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	}

	void draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, posvecsize/*3*sizeof(float)*/, GL_UNSIGNED_INT, 0);
	}
private:
	GLuint IBO, VBO;
	::std::size_t posvecsize;
};

class Primitive
{
public:
	Primitive(): VBO(0) {}
	template <typename ...Types>
	Primitive(const ::bee::gl::VertexAttrs<Types...> &vertices):
		vertnum(vertices.size())
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	}

	void draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, vertnum);//3*sizeof(float), GL_UNSIGNED_INT, 0);
	}
private:
	GLuint VBO;
	int vertnum;
};

// IBOPrimitive prim;
Primitive prim;

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
	// const ::bee::gl::VertexAttrs<::bee::gl::color3, ::bee::gl::pos3> vertices = {
	// 	{{ {}, {-1, -1, 0} }},
	// 	{{ {}, {0, -1, 1} }},
	// 	{{ {}, {1, -1, 0} }},
	// 	{{ {}, {0, 1, 0} }},
	// };
	const ::bee::gl::VertexAttrs<::bee::gl::color3, ::bee::gl::pos3> vertices = {
		{{ {}, {-1, -1, 0} }},
		{{ {}, {0, 1, 0} }},
		{{ {}, {0, -1, 1} }},
		
		{{ {}, {0, -1, 1} }},
		{{ {}, {0, 1, 0} }},
		{{ {}, {1, -1, 0} }},

		{{ {}, {-1, -1, 0} }},
		{{ {}, {0, 1, 0} }},
		{{ {}, {1, -1, 0} }},

		{{ {}, {-1, -1, 0} }},
		{{ {}, {0, -1, 1} }},
		{{ {}, {1, -1, 0} }},
	};
	// vertices[0].get<::bee::gl::pos3>() = {-1, -1, -1};
	// BEE_LOG(vertices[0].get<::bee::gl::position>()[1]);
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		0, 3, 2,
		0, 1, 2
	};
	prim = Primitive(vertices); //IBOPrimitive(vertices, indices, 12);
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