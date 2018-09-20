#version 330 core



out vec4 outColor;

in vec3 normalOut;
uniform sampler2D colorTex;
void main()
{
	
	    
		outColor = vec4(normalOut,1);
}