#version 330 core

uniform sampler2D gColorMap;

in vec2 TexCoord; 
out vec4 FragColor; 

void main() 
{
    FragColor = vec4(1, 1, 1, 1);
	// FragColor = texture(gColorMap, TexCoord);

	// if (FragColor.r >= 0.9 && FragColor.g >= 0.9 && FragColor.b >= 0.9) {
		// discard;
	// }
}