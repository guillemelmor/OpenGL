#version 330 core

in vec3 inPos;		
uniform mat4 modelViewProj;
out vec3 colorVert;
void main()
{
if (mod (gl_VertexID, 2)==0)
	colorVert = vec3 (0, 0, 1);
else
	colorVert = vec3 (1);

gl_Position = modelViewProj*vec4(inPos, 1.0) ;
}
