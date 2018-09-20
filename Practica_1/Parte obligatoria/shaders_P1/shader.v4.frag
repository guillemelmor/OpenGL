#version 330 core

out vec4 outColor; //queremos que salga el color de los vertices


void main()
{
	if (mod (gl_PrimitiveID, 2)==0)
	//aquellas primitivas que sean pares se colorearan de un color
	outColor = vec4 (0, 0, 1, 0);
	else
	//aquellas primitivas que no sean pares se colorerean de otro colpr
	outColor = vec4 (0, 1, 0, 0);
	//outColor = vec4 (gl_PrimitiveID*0.1f); 
}
