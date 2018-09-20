#version 330 core

in vec3 inPos;		
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
void main()
{
	gl_Position = proj * view * vec4 (inPos, 1.0); ;
}
