#version 330 core

//atributos de entrada
in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;
in vec2 inTexCoord;	

//variable uniform
uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 modelView;
uniform sampler2D normalTex;

//atributos de salida
out vec3 color;
out vec3 outpos; 
out vec3 outnormal;
out vec2 texcoord;

void main()
{

	outpos = vec3 (modelView* vec4(inPos, 1.0));
	outnormal = vec3 (normal * vec4(inNormal, 0.0));
	color = inColor;
	texcoord = inTexCoord;
	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}


