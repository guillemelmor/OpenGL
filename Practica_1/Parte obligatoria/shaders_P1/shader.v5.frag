#version 330 core

out vec4 outColor;
in vec3 colorVert;

void main()
{
	
	outColor = vec4 (colorVert, 0);
	
}
