#version 330 core

in vec3 inPos;		
uniform mat4 modelViewProj;
out vex3 colorVert;
void main()
{

	gl_Position = modelViewProj*vec4(inPos, 1.0) ;
}
