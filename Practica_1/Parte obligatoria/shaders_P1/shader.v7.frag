#version 330 core



out vec4 outColor;
in vec2 texCoord;

uniform sampler2D colorTex;
void main()
{
		outColor = texture(colorTex,texCoord);
}