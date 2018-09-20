#version 330 core

//
in vec3 inPos;	
out vec2 texCoord;

void main()
{
	texCoord = inPos.xy*0.5+vec2(0.5);//Como damos un plano ya proyectado no tenemos que multiplicarlo por ninguna matriz.
									  //no saseguramos de que todos los vertices son visibles
	gl_Position =  vec4 (inPos,1.0); //vertice proyectado y las coordenadas de textura. Pasa de -1 a 1 del rango de 0 a 1

}
