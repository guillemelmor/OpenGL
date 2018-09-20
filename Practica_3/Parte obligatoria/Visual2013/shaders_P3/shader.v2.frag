#version 330 core

out vec4 outColor;

in vec3 color;
in vec3 outnormal;
in vec3 outpos;
in vec2 texcoord;
in mat3 TBN;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D normalTex;
uniform sampler2D specularTex;

// difusa
uniform vec3 lpos;
uniform vec3 Id;
// direccional
uniform vec3 lpos2;
uniform vec3 Id2;
// focal
uniform vec3 lpos3;
uniform vec3 Id3;

uniform mat4 View;
uniform mat4 modelView;
uniform mat4 model;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
float alpha = 5000.0;
vec3 Ke;
vec3 pos;
//bump mapping
vec4 Normal;

//Propiedades de la luz
	// ambiental
	vec3 Ia = vec3 (0.3);

	// difusa
	//vec3 Id = vec3 (1.0);		se cambio por una intensidad recibida como entrada del shader
	//vec3 lpos = vec3 (0.0);	se cambio por una posicion recibida como entrada del shader

	// direccional
	//vec3 Id2 = vec3 (1.0, 0.0, 0.0);
	//vec3 lpos2 = vec3 (0.0, 0.0, 8.0);

	// focal
	//vec3 Id3 = vec3 (1.0, 0.0, 0.0);
	//vec3 lpos3 = vec3 (0.0, 0.0, 8.0);

	// especular
	vec3 Is = vec3 (1.0);


vec3 shade();

void main()
{
	Ka = texture(colorTex, texcoord).rgb;
	Kd = texture(colorTex, texcoord).rgb;
	Ke = texture(emiTex, texcoord).rgb;
	Ks = texture(specularTex, texcoord).rgb;
	Normal = texture(normalTex, texcoord);		 // bump mapping

	// bump mapping
	vec3 coordlocal = 2.0* Normal.rgb - vec3(1.0);		// extraemos las normales
	N = normalize(TBN*coordlocal*mat3(modelView));		// hacemos un cambio de base	
	//N = normalize (outnormal);				

	pos = outpos;
	outColor = vec4(shade(), 1.0);   
}

vec3 shade()
{
	// atenuacion
	vec3 C = vec3 (1.0, 0.0, 0.0);	// k1 valdra 1, k2 es la atenuacion lineal y k3 es la atenuacion cuadrática

	// ambiental
	vec3 c = vec3(0.0);
	c = Ia * Ka;

	// difusa
	float d = length (lpos);								// distancia desde el vertice hasta su incidencia
	float fatt = 1/ (C.x + C.y*d + C.z*pow(d, 2));			// hallamos la atenuación
	vec3 L = normalize(vec3(View*vec4(lpos, 1.0)-pos));
	vec3 diffuse = fatt * Id * Kd * dot (L,N);
	c += clamp(diffuse, 0.0, 1.0);

	// direccional
	float d2 = length (lpos2);							// distancia desde el vertice hasta su incidencia
	float fatt2 = 1/ (C.x + C.y*d2 + C.z*pow(d2, 2));	// hallamos la atenuación
	vec3 L2 = normalize(vec3(View*vec4(lpos2, 0.0)));		// hallamos el vector L
	vec3 directional = fatt2 *Id2*Kd*dot(N,L2);;
	//c += clamp(directional, 0.0, 1.0);

	// focal
	vec3 L3 = normalize(vec3(View*vec4(lpos3, 1.0)-pos));			// vector L
	vec3 D = normalize(vec3(-lpos3));								// vector D
	float beta = 3.14159/17;										// angulo Epsilon
	float no = 0.5;													// n
	float Ip;														// Iluminacion focal
	float calculo1 = dot ( D, -L3 );							// Producto escalar entre D y -L
	float calculo2 = cos(beta);									// coseno de Epsilon
	float calculo = (calculo1 - calculo2) / (1 - calculo2) ;	// (D*(-L) - cos (Epsilon))/(1-cos (Epsilon))
	if ( calculo1 > calculo2 ){									// si D*(-L) > cos (Epilon)
	Ip = pow (calculo , no);									// calculo ^ n
	}else{
	Ip = 0;														// si esta fuera es nulo
	}
	vec3 idiff = vec3 (Id3*Kd*dot(N, L3));						// Iluminacion focal
	vec3 focal = Ip*idiff;											// Aplicamos la atenuación	
	//c += clamp(focal, 0.0, 1.0);

	// especular
	vec3 V = normalize (-pos);
		// especular difusa
		vec3 R = normalize (reflect (-L,N));
		float factor = max (dot (R,V), 0.01);
		vec3 specular = Is*Ks*pow(factor,alpha);
		c += clamp(specular, 0.0, 1.0);

		// especular direccional
		vec3 R2 = normalize (reflect (-L2,N));
		float factor2 = max (dot (R2,V), 0.01);
		vec3 specular2 = Is*Ks*pow(factor2,alpha);
		c += clamp(specular2, 0.0, 1.0);

		// especular focal
		vec3 R3 = normalize (reflect (-L3,N));
		float factor3 = max (dot (R3,V), 0.01);
		vec3 specular3 = Is*Ks*pow(factor3,alpha);
		c += clamp(specular3, 0.0, 1.0);

	// emisiva
	c+=Ke;
	
	return c;
}
