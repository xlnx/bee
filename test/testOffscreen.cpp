#include "bee.h"

using namespace bee;
using namespace gl;
using namespace glm;
using namespace std;

#define WIDTH 140
#define HEIGHT 140

Window<4, 2> window("offscreen", false, WIDTH, HEIGHT);

int Main(int argc, char *argv[])
{
	// Shader shader(
	// 	VertexShader("shaders/testMesh-vs.glsl"),
	// 	FragmentShader("shaders/testMesh-fs.glsl")
	// );
	// shader.use();
	const int render_width = WIDTH, render_height = HEIGHT;
	enum { Color, Depth, NumRenderbuffers };
    // multi-sampled frame buffer object as the draw target
    GLuint framebuffer_ms, renderbuffer_ms[NumRenderbuffers];

    // generate color and depth render buffers and allocate storage for the multi-sampled FBO
    glGenRenderbuffers(NumRenderbuffers, renderbuffer_ms);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_ms[Color]);
    	glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F//GL_RED//GL_RGBA, 
			, render_width, render_height);
		// glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, )
    // glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_ms[Depth]);
    // glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16,
    //     GL_DEPTH_COMPONENT24, render_width, render_height);

    // generate frame buffer object for the multi-sampled FBO
    glGenFramebuffers(1, &framebuffer_ms);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_ms);
    	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        	GL_RENDERBUFFER, renderbuffer_ms[Color]);
    // glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
    //     GL_RENDERBUFFER, renderbuffer_ms[Depth]);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		BEE_LOG("Failed to create framebuffer");
	}

	// glDrawBuffer(GL_COLOR_ATTACHMENT0);

	static TestMeshObject mo(gl::Mesh(gl::VertexAttrs<gl::pos3>{
		{{0, 1, 0}},
		{{-1, -1, 0}},
		{{1, -1, 0}}
	}, gl::Faces{
		{0, 1, 2}
	}));
	static ViewPort view(0, 0, WindowBase::getWidth(), WindowBase::getHeight());
	// mo.render(*cameras[0]);
	window.dispatch<RenderEvent>(
		[&]() -> bool
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_ms);

			glClear(GL_COLOR_BUFFER_BIT);
			mo.render(view);
			
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			// int objectIndex;
			static int indices[HEIGHT/2][WIDTH/2];// = {1, 2, 3, 4, 5};
			// glReadPixels(x, y, 1, 1, GL_RED, GL_INT, &objectIndex);
			// BEE_LOG("asdasd");
			glReadPixels(WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2, GL_RED//GL_RGBA, 
				, GL_FLOAT, &indices);
			// BEE_LOG("asdasd");
			for (int i = 0; i != HEIGHT/2; ++i)
			{
				for (int j = 0; j != WIDTH/2; ++j)
				{
					// if (indices[i][j])
					// {
					// 	cout << "x";
					// }
					// else
					// {
					// 	cout << ".";
					// }
					cout << indices[i][j] << " ";
				}
				std::cout << std::endl;
			}
			return false;
		}
	);
	window.dispatchMessages();
	return 0;
}