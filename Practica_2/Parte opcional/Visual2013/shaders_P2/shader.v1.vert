#version 330 core


//intensidad ambiental
vec3 Ia = vec3 (0.3);
//intensidad difusa
vec3 Il = vec3 (1.0);
vec3 Pl = vec3 (0.0); //posición de la luz, para ubicarla necesitamos un sistema de referencia (supondremos que el sistema de referencia es el 
					  //sistema de la camara)
//intensidad especular

//objeto ambiental
vec3 Ka;
//objeto difuso
vec3 Kd;
vec3 N;
vec3 pos; //para coordenadas de la camara

//objeto especular
vec3 Ks;
float n = 30.0; //pulido del objeto

//atributos
in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;

uniform mat4 modelViewProj;
uniform mat4 normal;
uniform mat4 modelView;

out vec3 color;
vec3 shade ();

void main()
{
	Ka = inColor;
	Kd = inColor;
	//Ka = vec3 (1.0);
	//Kd = vec3 (1.0);
	Ks = vec3 (1.0);
	N = normalize(vec3 (normal*vec4(inNormal, 0.0)));

	pos = vec3(modelView * vec4 (inPos, 1.0));

	color = shade();
	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}


vec3 shade() {

// ambiental
vec3 color = Ia*Ka;

// difusa
vec3 L = normalize(Pl - pos);
color+= Il*Kd*dot(N,L);

// especular
vec3 V = normalize (-pos);
vec3 R = normalize (reflect (-L, N)); //se refleja  
float factor = clamp(dot (V, R), 0.0001, 1.0);
factor = pow (factor, n);
color+= Il*Ks*factor;

return color;
}