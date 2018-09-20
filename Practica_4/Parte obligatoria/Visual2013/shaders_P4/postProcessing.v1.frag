#version 330 core
#define MASK_SIZE 25u
//Color de salida
out vec4 outColor;

//Variables Variantes
in vec2 texCoord;

//Textura
uniform sampler2D colorTex;
uniform sampler2D vertexTex;
uniform sampler2D depthTex;

uniform float mascara[25];
uniform float focalDistance;
uniform float maxDistanceFactor;
uniform float variable_near;
uniform float variable_far;
//const float focalDistance = -25.0;
//const float maxDistanceFactor = 1.0/5.0;



const vec2 texIdx[MASK_SIZE] = vec2[](
	vec2(-2.0,2.0f), vec2(-1.0,2.0f), vec2(0.0,2.0f), vec2(1.0,2.0f), vec2(2.0,2.0),
	vec2(-2.0,1.0f), vec2(-1.0,1.0f), vec2(0.0,1.0f), vec2(1.0,1.0f), vec2(2.0,1.0),
	vec2(-2.0,0.0f), vec2(-1.0,0.0f), vec2(0.0,0.0f), vec2(1.0,0.0f), vec2(2.0,0.0),
	vec2(-2.0,-1.0f), vec2(-1.0,-1.0f), vec2(0.0,-1.0f), vec2(1.0,-1.0f), vec2(2.0,-1.0),
	vec2(-2.0,-2.0f), vec2(-1.0,-2.0f), vec2(0.0,-2.0f), vec2(1.0,-2.0f), vec2(2.0,-2.0));
	
	const float maskFactor = float (1.0/16);
	
const float mask[MASK_SIZE] = float[](
	/*
	mascara[0], mascara[1], mascara[2], mascara[3], mascara[4],
	mascara[5], mascara[6], mascara[7], mascara[8], mascara[9],
	mascara[10], mascara[11], mascara[12], mascara[13], mascara[14],
	mascara[15], mascara[16], mascara[17], mascara[18], mascara[19],
	mascara[20], mascara[21], mascara[22], mascara[23], mascara[24]
	*/
    
	0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, -1.0*maskFactor, -1.0*maskFactor, -1.0*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, -1.0*maskFactor, 8.0*maskFactor, -1.0*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, -1.0*maskFactor, -1.0*maskFactor, -1.0*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor
	
	/*
	1.0*maskFactor, 2.0*maskFactor, 3.0*maskFactor,2.0*maskFactor, 1.0*maskFactor,
	2.0*maskFactor, 3.0*maskFactor, 4.0*maskFactor,3.0*maskFactor, 2.0*maskFactor,
	3.0*maskFactor, 4.0*maskFactor, 5.0*maskFactor,4.0*maskFactor, 3.0*maskFactor,
	2.0*maskFactor, 3.0*maskFactor, 4.0*maskFactor,3.0*maskFactor, 2.0*maskFactor,
	1.0*maskFactor, 2.0*maskFactor, 3.0*maskFactor,2.0*maskFactor, 1.0*maskFactor			0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, (Mascara [0][0])*maskFactor, (Mascara [1][0])*maskFactor, (Mascara [2][0])*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, (Mascara [0][1])*maskFactor, (Mascara [1][1])*maskFactor, (Mascara [2][1])*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, (Mascara [0][2])*maskFactor, (Mascara [1][2])*maskFactor, (Mascara [2][2])*maskFactor, 0.0*maskFactor,
	0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor, 0.0*maskFactor		*/);
void main()
{
	

	//Sería más rápido utilizar una variable uniform el tamaño de la textura.
	vec2 ts = vec2(1.0) / vec2 (textureSize (colorTex,0));

	//float dof = abs(texture(vertexTex,texCoord).z - focalDistance) * maxDistanceFactor;
	float dof = abs((-variable_near*variable_far/(texture(depthTex,texCoord).r*(variable_near-variable_far) + variable_far)) - focalDistance) * maxDistanceFactor;
	//float dof = abs(texture(depthTex,texCoord).r - focalDistance) * maxDistanceFactor;
	dof = clamp (dof, 0.0, 1.0);
	dof *= dof;
	//vec4 color = vec4 (0.0);
	vec4 color = vec4 (0.0);


		for (uint i = 0u; i < MASK_SIZE; i++)
		{
			vec2 iidx = texCoord + ts * texIdx[i]*dof;
			color += texture(colorTex, iidx,0.0) * mascara[i];
		}

	//outColor =  -near*far/(texture(depthTex,texCoord)*(near-far) + far);
	outColor = color;
}