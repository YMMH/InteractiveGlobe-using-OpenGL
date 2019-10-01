#version 330 core
layout (location = 0) in vec3 aVertex; //<vec2 pos, vec2 tex>
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	//gl_Position = projection * view * model * vec4(aVertex, 1.0);
	gl_Position = projection * vec4(aVertex, 1.0);
	//gl_Position = vec4(aVertex, 1.0);
	TexCoords = aTexCoord;
}
