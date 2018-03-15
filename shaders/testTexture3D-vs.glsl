#version 330 core

layout (location = 0) in vec2 Position;

uniform int gTarget;

out vec3 TexVector0;

void main()
{
	gl_Position = vec4(Position, 0, 1);
	switch (gTarget)
	{
		case 0: TexVector0 = vec3(1, Position); break;
		case 1: TexVector0 = vec3(-1, Position); break;
		case 2: TexVector0 = vec3(Position.x, 1, Position.y); break;
		case 3: TexVector0 = vec3(Position.x, -1, Position.y); break;
		case 4: TexVector0 = vec3(Position, 1); break;
		case 5: TexVector0 = vec3(Position, -1); break;
	}
}