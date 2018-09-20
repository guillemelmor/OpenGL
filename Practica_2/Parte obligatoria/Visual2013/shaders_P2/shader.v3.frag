#version 330 core

//variantes
in vec3 color;
in vec3 outnormal;
in vec3 outpos;
in vec2 texcoord;
in mat3 TBN;

out vec4 outColor;

//uniform
uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
uniform mat4 modelView;
uniform mat4 view;
uniform mat4 model;
//metodo shade
vec3 shade();

// fuentes de luz:

	//intensidad ambiental
	vec3 Ia = vec3 (0.3);				// intensidad ambiental

	//intensidad difusa 1
	vec3 Il = vec3 (1.0, 0.0, 0.0);		// la intensidad puede ser modificada para obtener un color en el foco
	vec3 Pl = vec3 (8.0, 0.0, 6.0);		// posición de la luz, para ubicarla necesitamos un sistema de referencia (supondremos que es el 
										// el sistema de referencia sistema de la camara)

	//intensidad difusa 2
	vec3 Il2 = vec3 (0.0, 0.0, 1.0);	// la intensidad puede ser modificada para obtener un color en el foco
	vec3 Pl2 = vec3 (-8.0, 0.0, 0.0);	// posición de la luz, para ubicarla necesitamos un sistema de referencia (supondremos que es el
										// sistema de referencia es el sistema de la camara)

	//intensidad direccional
	vec3 Il3 = vec3 (1.0, 1.0, 1.0);	// la intensidad puede ser modificada para obtener un color en el foco
	vec3 Pl3 = vec3 (0.0, 0.0, 6.0);	// posición de la luz, para ubicarla necesitamos un sistema de referencia (supondremos que es el 
										// sistema de referencia es el sistema de la camara)

	// intensidad focal
	vec3 Il4 = vec3 (0.3, 0.5, 0.0);	// la intensidad puede ser modificada para obtener un color en el foco
	vec3 Pl4 = vec3 (0.0, 0.0, 8.0);	// posición de la luz, para ubicarla necesitamos un sistema de referencia (supondremos que es el 
										// sistema de referencia es el sistema de la camara)

//propiedades del objeto
 
	//objeto ambiental
	vec3 Ka;		// constante ambiental

	//objeto difuso
	vec3 Kd;		// constante difusa
	vec3 N;			// Normal
	vec3 pos;		// para coordenadas de la camara
	vec3 Ke;		// constante emisiva

	//bump mapping
	vec4 Normal;	// Normal del mapa de texturas

	//objeto especular
	vec3 Ks;		// Constante especular
	float n = 30.0; // pulido del objeto

// para la atenuación del foco observar la 37 del tema 4: iluminación

// metodo main fragmentos
void main()
{
	Ka = texture (colorTex, texcoord). rgb;		 // ambiental
	Kd = texture (colorTex, texcoord). rgb;	     // difusa
	Ke = texture (emiTex, texcoord). rgb;		 // emisiva
	//Normal = texture(normalTex, texcoord);		 // bump mapping
	Ks = texture (specularTex, texcoord).rgb;	 // especular
	//Ks = vec3 (1.0);							 // especular

	// bump mapping
	//vec3 coordlocal = 2.0* Normal.rgb - vec3(1.0);		// estraemos las normales
	//N = normalize(TBN*coordlocal*mat3(modelView));		// hacemos un cambio de base	
	N = normalize (outnormal);								// normal inicial
	
	pos = outpos;
	outColor = vec4(shade(), 1.0);   
}




// metodo shade
vec3 shade() {

	// atenuacion
	vec3 C = vec3 (1.0, 0.3, 0.0);	// k1 valdra 1, k2 es la atenuacion lineal y k3 es la atenuacion cuadrática
									// ojear http://wiki.blender.org/index.php/Doc:ES/2.4/Manual/Lighting/Lights/Light_Attenuation
									// ojear http://www4.tecnun.es/asignaturas/grafcomp/OpenGL/practica/capitulo6.pdf

	// ambiental
	vec3 color = Ia*Ka;									// hallámos la iluminación ambiental

	// difusa 1
	float d = length (Pl);								// distancia desde el vertice hasta su incidencia
	float fatt = 1/ (C.x + C.y*d + C.z*pow(d, 2));		// hallamos la atenuación
	vec3 L = normalize(vec3(view*vec4(Pl, 1.0)-pos));	// hallamos el vector L
	color+= fatt*Il*Kd*dot(N,L);						// hallamos la iluminación difusa 1
	
	// difusa 2
	float d2 = length (Pl2);							// distancia desde el vertice hasta su incidencia 
	float fatt2 = 1/ (C.x + C.y*d2 + C.z*pow(d2, 2));	// hallamos la atenuación
	vec3 L2 = normalize(vec3(view*vec4(Pl2, 1.0)-pos));	// hallamos el vector L
	//color+= fatt2*Il2*Kd*dot(N,L2);						// hallamos la iluminación difusa 2

	// direccional
	float d3 = length (Pl3);							// distancia desde el vertice hasta su incidencia
	float fatt3 = 1/ (C.x + C.y*d3 + C.z*pow(d3, 2));	// hallamos la atenuación
	vec3 L3 = normalize(vec3(view*vec4(Pl3, 0.0)));		// hallamos el vector L
	//color+= fatt3*Il3*Kd*dot(N,L3);						// hallamos la iluminación direccional
	
	// focal	
	vec3 L4 = normalize(vec3(view*vec4(Pl4, 1.0)-pos));			// vector L
	vec3 D = normalize(vec3(-Pl4));								// vector D
	float beta = 3.14159/20;									// angulo Epsilon
	float no = 0.5;												// n
	float Ip;													// Iluminacion focal
	float calculo1 = dot ( D, -L4 );							// Producto escalar entre D y -L
	float calculo2 = cos(beta);									// coseno de Epsilon
	float calculo = (calculo1 - calculo2) / (1 - calculo2) ;	// (D*(-L) - cos (Epsilon))/(1-cos (Epsilon))
	if ( calculo1 > calculo2 ){									// si D*(-L) > cos (Epilon)
	Ip = pow (calculo , no);									// calculo ^ n
	}else{
	Ip = 0;														// si esta fuera es nulo
	}
	vec3 idiff = vec3 (Il4*Kd*dot(N, L4));						// Iluminacion focal
	//color+= Ip*idiff;											// Aplicamos la atenuación	

	// especular difusa 1
	vec3 V = normalize (-pos);
	vec3 R = normalize (reflect (-L, N));						//se refleja difusa  
	float factor = clamp(dot (V, R), 0.0001, 1.0);				
	factor = pow (factor, n);
	color+= fatt*Il*Ks*factor;
	
	// especular difusa 2
	vec3 R2 = normalize (reflect (-L2, N));	//se refleja difusa2  
	float factor2 = clamp(dot (V, R2), 0.0001, 1.0);
	factor2 = pow (factor2, n);
	color+= fatt2*Il2*Ks*factor2;

	// especular direccional
	vec3 R3 = normalize (reflect (-L3, N));	//se refleja direccional
	float factor3 = clamp(dot (V, R3), 0.0001, 1.0);
	factor3 = pow (factor3, n);
	color+= fatt3*Il3*Ks*factor3;

	// especular focal
	vec3 R4 = normalize (reflect (-L4, N));	//se refleja direccional
	float factor4 = clamp(dot (V, R4), 0.0001, 1.0);
	factor4 = pow (factor4, n);
	color+= Il4*Ks*factor4;
	
	// emisiva
	color+= Ke;

return color;
}