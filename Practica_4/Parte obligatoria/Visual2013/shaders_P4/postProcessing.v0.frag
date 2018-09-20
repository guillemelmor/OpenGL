#version 330 core

//Color de salida
out vec4 outColor;

//Variables Variantes
in vec2 texCoord;

//Textura
uniform sampler2D colorTex;


void main()
{
	// el valor es 0.6 canal de transparencia para que tenga mayor o menor peso
	outColor = vec4(textureLod(colorTex, texCoord,0).xyz, 0.7);	
	//outColor = vec4(texCoord,vec2(1.0));
}