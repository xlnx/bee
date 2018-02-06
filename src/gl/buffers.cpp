#include "buffers.h"

namespace bee
{

namespace gl
{

bool BufferGenerator::haveInstance = false;
GLuint *BufferGenerator::dataptr = nullptr;
GLuint *BufferGenerator::dataend = nullptr;
GLuint *BufferGenerator::ptr = nullptr;

static BufferGenerator buffer;

bool VertexArrayGenerator::haveInstance = false;
GLuint *VertexArrayGenerator::dataptr = nullptr;
GLuint *VertexArrayGenerator::dataend = nullptr;
GLuint *VertexArrayGenerator::ptr = nullptr;

static VertexArrayGenerator vertexArray;

}

}