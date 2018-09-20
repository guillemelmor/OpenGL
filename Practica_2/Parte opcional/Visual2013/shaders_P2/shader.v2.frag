#version 330 core

out vec4 outColor;
//variantes
in vec3 color;
in vec3 outnormal;
in vec3 outpos;
in vec2 texcoord;

//uniform
uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D specularTex;
uniform mat4 view;


//objeto ambiental
vec3 Ka;
//objeto difuso
vec3 Kd;
vec3 N;
vec3 pos; //para coordenadas de la camara
vec3 Ke;
vec3 Ks;
float n = 300.0; //pulido del objeto


//fuente de luz
vec3 Ia = vec3(0.3);
//difusa 1
vec3 Il = vec3(1.0, 0.0, 0.0);
vec3 Pl = vec3 (8.0, 0.0, 0.0);
// difusa 2
vec3 Il2 = vec3(0.0, 0.0, 1.0);
vec3 Pl2 = vec3 (-8.0, 0.0, 0.0);

vec3 shade();

void main()
{
	Ka = texture (colorTex, texcoord). rgb;		 //ambiental
	Kd = texture (colorTex, texcoord). rgb;	    	 //difusa
	Ke = texture (emiTex, texcoord). rgb;		 //emisiva
	Ks = texture (specularTex, texcoord).rgb;	 //specular
	N = normalize(vec3(outnormal));
	pos = outpos;
	outColor = vec4(shade(), 1.0);   
	
}


vec3 shade()
{
vec3 C = vec3 (1.0, 0.1, 0.0);
float d = length (Pl);			// distancia desde el vertice hasta su incidencia // Pl para varios
float fatt = 1/ (C.x + C.y*d + C.z*pow(d, 2));
	//ambiental
	vec3 color = vec3(0.0);
	color = Ia*Ka;
	//difusa 1
	vec3 L = normalize(vec3(view*vec4(Pl, 1.0)-pos));
	color += fatt*Il*Kd*dot(N,L);
	//difusa 2
	vec3 L2 = normalize(vec3(view*vec4(Pl2, 1.0)-pos));
	//color += Il2*Kd*dot(N,L2);


	vec3 V = normalize(-pos);
	vec3 R = normalize(reflect(-L,N));
	float factor = clamp (dot(V,R),0.0001,1.0);
	factor = pow (factor,n);
	color += Il * Ks * factor;
	color += Ke;

	return color;
}
