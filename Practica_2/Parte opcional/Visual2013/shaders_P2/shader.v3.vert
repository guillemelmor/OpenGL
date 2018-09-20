#version 330 core

//atributos de entrada
in vec3 inPos;	
in vec3 inColor;
in vec2 inTexCoord;
in vec3 inNormal;
in vec3 inTangent;	

//atributos de salida
out vec3 color;
out vec3 outpos; 
out vec3 outnormal;
out vec2 texcoord;
out mat3 TBN;

//variable uniform
uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 modelView;
uniform sampler2D normalTex;

vec3 shade ();

// main de vertices
void main()
{
	vec3 Tangente = normalize (vec3 (inTangent));						// vector tangente
	vec3 Normal = normalize (vec3 (inNormal));							// vector normal
	vec3 Binormal = normalize (vec3 (cross (Normal, Tangente)));		// vector binormal

	TBN [0].x = Tangente.x;			// (Tangente		-			-   )
	TBN [0].y = Binormal.x;			// (Binormal		-			-   )
	TBN [0].z = Normal.x;			// (Normal)			-			-   )

	TBN [1].x = Tangente.y;			// (Tangente	Tangente		-   )
	TBN [1].y = Binormal.y;			// (Binormal	Binoraml		-   )
	TBN [1].z = Normal.z;			// (Normal)		Normal			-   )

	TBN [2].x = Tangente.z;			// (Tangente	Tangente	Tangente)
	TBN [2].y = Binormal.z;			// (Binormal	Binormal	Binormal)
	TBN [2].z = Normal.z;			// (Normal		Normal		Normal  )		
	
	// hemos de pasar de coordenadas de la camara a coordenadas de la tangente
	// x	
	// y =	L * TBN
	// z	
	
	outpos = vec3 (modelView* vec4(inPos, 1.0));
	outnormal = vec3 (normal * vec4(inNormal, 0.0));
	color = inColor;
	texcoord = inTexCoord;
	gl_Position =  modelViewProj * vec4 (inPos,1.0);

}


