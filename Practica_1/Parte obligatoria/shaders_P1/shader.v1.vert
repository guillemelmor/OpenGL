#version 330 core

in vec3 inPos;		

void main()
{
mat4 view = mat4 (1.0);
mat4 proj = mat4 (0.0);

float near = 0.1;
float far = 100.0;

//matriz view de la camara que se traslada
view [3].z = -6.0;
view [3].y = 3.46411;
view [3].x = 0.0;

//                         +y ^
//                            |
//                            |
//                            | 
//                            |
//            -x  <-----------+ (-z)

//matriz de proyeccion
proj [0].x = 1.0 / tan (3.14159 / 6.0);
proj [1].y = 1.0 / tan (3.14159 / 6.0);
proj [2].z = - (far + near) / (far - near);
proj [2].w = -1.0;
proj [3].z = -2.0 * far * near / (far - near);

	gl_Position =  proj * view * vec4 (inPos, 1.0);;
}
