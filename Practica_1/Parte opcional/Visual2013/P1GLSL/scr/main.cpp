#include "BOX.h"
#include "PIRAMID.h"
#include <IGL/IGlib.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//matriz view
glm::mat4 view = glm::mat4(1.0f);

//Idenficadores de los objetos de la escena
int objId = -1;		//cubo central
int objId2 = -1;	 //cubo de la parte obligatoria
int objId3 = -2;	//Piramide de la parte opcional
int objId4 = -1;

//Declaración de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P1/shader.v8.vert", "../shaders_P1/shader.v8.frag"))
		return -1;

	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setIdleCB(idleFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);

	//Se ajusta la cámara

		// matriz view
		view[3].z = -11.0f;
		view[3].y = 0.0;
		view[3].x = 0.0f;
		IGlib::setViewMat(view);

		// matriz proyección
		glm::mat4 proj = glm::mat4(0.0f);
		float near = 0.1f;
		float far = 100.0f;
		proj[0].x = 1.0f / tan(3.14159f / 6.0f);
		proj[1].y = 1.0f / tan(3.14159f / 6.0f);
		proj[2].z = -(far + near) / (far - near);
		proj[2].w = -1.0f;
		proj[3].z = -2.0f * far * near / (far - near);
		IGlib::setProjMat(proj);

	// creación de los objetos

		//Creamos el objeto central
		objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

		//Se crea el objeto de la parte obligatoria
		objId2 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

		//Se crea la pirámide de la parte opcional
		objId3 = IGlib::createObj(piramidNTriangleIndex, piramidNVertex, piramidTriangleIndex, 
		piramidVertexPos, piramidVertexColor, piramidVertexNormal, piramidVertexTexCoord, piramidVertexTangent);

		//Se crea el objeto de la parte opcional
		objId4 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

	// se crea la matriz model
	glm::mat4 modelMat = glm::mat4(1.0f);
	IGlib::setModelMat(objId, modelMat);

	glm::mat4 modelMat2 = glm::mat4(0.5f);
	IGlib::setModelMat(objId2, modelMat2);

	// se aplican las texturas a los objetos

	IGlib::addColorTex(objId, "..\\img\\color.png");	//accedemos a la textura deseada en la carpeta del proyecto
	
	
	//CBs
	IGlib::setIdleCB(idleFunc);
	IGlib::setResizeCB(resizeFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);

	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void resizeFunc(int width, int height)					// Ajusta el aspect ratio al tamaño de la venta
{

	glm::mat4 rproj(0.0f);								// se crea la matriz que emplearemos como nueva matriz proyección
	if (height == 0) height = 1;						// para prevenir indeterminación impedimos el valor nulo
	float w = width;									// para obtener decimales, se cambia el valor de entero a float
	float h = height;									// para obtener decimales, se cambia el valor de entero a float
	float aspect = (w / h);								// calculamos el cociente de aspect ratio a partir de width y height
	float far = 100.0f;									// establecemos el valor a la variable far	
	float near = 0.1f;									// establecemos el valor a la variable near
	
	// matriz rproj, es decir, la nueva matriz proyección
	rproj[0].x = (1.0f / tan(3.14159f/6.0f));			// el primer valor como antes se explicó es 1/tan30
	rproj[1].y = rproj [0].x*aspect;					// el segundo valor como se mencionó arriba es 1/tan30 * ar
	rproj[2].z = -(far + near) / (far - near);			// se calcula el valor de acuerdo con la matriz original
	rproj[2].w = -1.0f;									// se calcula el valor de acuerdo con la matriz original
	rproj[3].z = -2.0f * far * near / (far - near);		// se calcula el valor de acuerdo con la matriz original

	IGlib::setProjMat(rproj);							// le indicamos a la clase main que la matriz rproj es la matriz proyección actual
}


void idleFunc()																//creamos un angulo de giro estatico para mantenerla de una ejecucion a otra
{	

	//cubo central
	static float angle = 0.0f;												//angulo de rotacion
	angle = (angle <= 2.0f*3.15159) ? angle + 0.001f : 0.0f;				//modificacion del angulo
	glm::mat4 model(1.0f);													//declaracion de la matriz del primer modelo
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));			//se transforma la matriz del primer modelo en una de rotacion 
	IGlib::setModelMat(objId, model);										//Se cambia la matriz del modelo por una de rotacion

	//cubo rotatorio
	glm::mat4 model2rot(1.0f);												//Declarar la matriz de rotacion		
	model2rot = glm::rotate(model2rot, angle, glm::vec3(0.0f, 1.0, 0.0f));  //Dar valor a la matriz de rotación
	glm::mat4 model2tras(1.0f);												//Declarar la matriz de traslacion
	model2tras = glm::translate(model2tras, glm::vec3(5.0f, 0.0f, 0.0f));	//Dar valor a la matriz de traslacion
	glm::mat4 escalado2(1.0f);												//Declarar la matriz de escalado
	escalado2 = glm::scale(escalado2, glm::vec3(0.5f, 0.5f, 0.5f));			//Dar valor a la matriz de escalado
	model2rot *= model2tras*model2rot*escalado2;					        //multiplicar las matrices de las transformaciones
	IGlib::setModelMat(objId2, model2rot);									//Se cambia la matriz del modelo por una con las transformaciones(se ha elegido usar la de rotacion ya que se almacenan los valores)

	//piramide 
	glm::mat4 model3tras(1.0f);
	model3tras = glm::translate(model3tras, glm::vec3(6.0f, 0.0f, 0.0f));
	glm::mat4 model3rot(1.0f);												//
	model3rot = glm::rotate(model3rot, angle, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 escalado3(1.0f);												//Declarar la matriz de escalado
	escalado3 = glm::scale(escalado3, glm::vec3(0.5f, 0.5f, 0.5f));
	IGlib::setModelMat(objId3, model3tras*model3rot*escalado3);

	//cubo de bezier
	float xt, zt;															// xt y zt serán las variables que almacenaran los valores de Bezier
	static float t = 0.0f;													// t será la variable encargada de mover el cubo por la curva
	t = (t <= 1.0f) ? t + 0.0001f : 0.0f;									// t siempre mantendra un valor entre 0 y 1
	glm::vec3 P0 = glm::vec3(8, 0, -6);										// P0 (8, 0, -6)
	glm::vec3 P1 = glm::vec3(-20, 0, -20);									// P1 (-20, 0, -20)
	glm::vec3 P2 = glm::vec3(20, 0, 100);									// P2 (20, 0, 100)
	glm::vec3 P3 = glm::vec3(8, 0, -6);;									// P3 (8, 0, -6)
	xt = pow((1 - t), 3)*P0.x + 3 * pow((1 - t), 2)*t*P1.x + (1 - t)*(pow(t, 2))*P2.x + pow(t, 3)*P3.x;	// x(t) = .....
	zt = pow((1 - t), 3)*P0.z + 3 * pow((1 - t), 2)*t*P1.z + (1 - t)*(pow(t, 2))*P2.z + pow(t, 3)*P3.z;	// z(t) = ....

	glm::mat4 model4(1.0f);													// se crea una matriz model
	model4 = glm::translate(model4, glm::vec3(xt, 0.0f, zt));				// se aplica una traslación siendo la curva de Bezier
	glm::mat4 escalado4(1.0f);												// se crea una matriz de escalado
	escalado4 = glm::scale(escalado4, glm::vec3(0.5f, 0.5f, 0.5f));			// se escala el cubo
	model4 = escalado4*model4;												// se multiplica el escalado por el cubo
	IGlib::setModelMat(objId4, model4);										// se actualiza la matriz

}

void keyboardFunc(unsigned char key, int x, int y)						// Control de la cámara con el teclado
{
	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl; // se muestra por consola que tecla se ha pulsado

	//traslacion
	glm::mat4 tras(1.0f);												// se crea una matrix auxilior para ser empleada como matriz de traslación

	//rotacion 
	glm::mat4 rot(1.0f);												// se crea una matriz auxiliar para ser empleada como matriz de rotación
	static float alfa = 0.01f;											// se crea un ángulo auxiliar para la rotación en el eje x
	static float beta = 0.01f;											// se crea un ángulo auxiliar para la rotación en el eje y
	
		
		if (key == 'z'){												// si se pulsa la z la cámara se traslada en dirección abajo
			tras = glm::translate(tras, glm::vec3(0.0f, 0.1f, 0.0f));	// se configura la matriz de traslación para ser trasladada en el eje Y
			view = tras*view;											// se multiplica la matriz de traslación por la matriz view
		}

		if (key == 'x'){												// si se pulsa la x la cámara se traslada en dirección arriba
			tras = glm::translate(tras, glm::vec3(0.0f, -0.1f, 0.0f));	// se configura la matriz de traslación para ser trasladada en el eje Y
			view = tras*view;											// se multiplica la matriz de traslación por la matriz view
		}

		if (key == 'd'){												// si se pulsa la d la cámara se traslada en dirección derecha
			tras = glm::translate(tras, glm::vec3(-0.1f, 0.0f, 0.0f));	// se configura la matriz de traslacion para ser trasladada en el eje X
			view = tras*view;											// se multiplica la matriz de traslación por la matriz view
		}

		if (key == 'a'){												// si se pulsa la a la cámara se traslada en dirección izquierda
			tras = glm::translate(tras, glm::vec3(0.1f, 0.0f, 0.0f));	// se configura la matriz de traslación para ser trasladada en el eje X
			view = tras*view;											// se multiplica la matriz de traslación por la matriz view
		}

		if (key == 'w'){												// si se pulsa la w la cámara se traslada en dirección frontal
			tras = glm::translate(tras, glm::vec3(0.0f, 0.0f, 0.1f));	// se configura la matriz de traslación para ser trasladada en el eje Z
			view = tras*view;											// se multiplica la matriz de traslación por la matriz view
		}

		if (key == 's'){												// si se pulsa la s la cámara se traslada en dirección trasera
			tras = glm::translate(tras, glm::vec3(0.0f, 0.0f, -0.1f));	// se configura la matriz de traslación para ser trasladada en el eje Z
			view = tras*view;											// se multiplica la matriz de traslación por la matriz view
		}	

		if (key == 't'){												// si se pulsa la tecla t la cámara rota hacia arriba
			rot = glm::rotate(rot, -alfa, glm::vec3(1.0f, 0.0f, 0.0f));	// se configura la matriz de rotacion para ser rotada en el eje X
			view = rot*view;											// se multiplica la matriz de rotación por la matriz view
		}

		if (key == 'g'){												// si se pulsa la tecla g la cámara rota hacia abajo
			rot = glm::rotate(rot, alfa, glm::vec3(1.0f, 0.0f, 0.0f));	// se configura la matriz de rotacion para ser rotada en el eje X
			view = rot*view;											// se multiplica la matriz de rotación por la matriz view
		}

		if (key == 'f'){												// si se pulsa la tecla f la cámara rota hacia la derecha
			rot = glm::rotate(rot, -beta, glm::vec3(0.0f, 1.0f, 0.0f));	// se configura la matriz de rotacion para ser rotada en el eje Y
			view = rot*view;											// se multiplica la matriz de rotación por la matriz view
		}

		if (key == 'h'){												// si se pulsa la tecla h la cámara rota hacia la izquierda
			rot = glm::rotate(rot, beta, glm::vec3(0.0f, 1.0f, 0.0f));	// se configura la matriz de rotacion para ser rotada en el eje Y
			view = rot*view;											// se multiplica la matriz de rotación por la matriz view
		}
		
		IGlib::setViewMat(view);										// se indica a la clase main que la matriz view se actualiza
}

// variables del método mouseFunc
float diferenciax;		// diferenciax sera la variable que calcule la diferencia de la coordenada x original menos la final
float diferenciay;		// diferenciax sera la variable que calcule la diferencia de la coordenada y original menos la final
float actualx = 0.0f;	// actualx será la variable encargada de guardar la coordenada x incial
float actualy = 0.0f;	// actualx será la variable encargada de guardar la coordenada y incial
float angulo = 0.1f;	// angulo será la variable encargada de incrementar/ decrementar el angulo de rotacion de x e y

void mouseFunc(int button, int state, int x, int y)				// Controla el giro de la cámara utilizando el ratón. 
{
	glm::mat4 aux (1.0f);										// se crea una matrix auxiliar que se empleara para trasladar y rotar

	if (state == 0){											// si se pulsa el raton...
		std::cout << "Se ha pulsado el botón ";					// se muestra por consola que se ha pulsado un botón del raton
		actualx = x;											// se recoge la coordenada x inicial
		actualy = y;											// se recoge la coordenada y inicial
		
	}else                                                       // si se suelta el botón una vez pulsada...
		std::cout << "Se ha soltado el botón ";					// se muestra por consola que se ha soltado el botón del ratón
		diferenciax = actualx - x;								// se resta la coordenada x inicial por la actual 
		diferenciay = actualy - y;								// se resta la coordenada y inicial por la actual
	

	if (button == 0){																			// si se pulsa el boton izquierdo del ratón...
		std::cout << "de la izquierda del ratón " << std::endl;									// se muestra por consola que el botón se ha pulsado
		if (diferenciax <= 50){																	// se marca un margen para expresar el movimiento
			aux = glm::rotate(aux, angulo*(diferenciay / 100.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// se configura la matriz para rotar en el eje X
			view = aux*view;																	// se multiplica la matriz aux por la matriz view
		}		
	}
	
	if (button == 1) {																			// si se pulsa el boton central del ratón...	
		std::cout << "central del ratón " << std::endl;											// se muestra por consola que el botón se ha pulsado
		aux = glm::translate(aux, glm::vec3(0.0f, 0.0f, diferenciay/100.0f));					// se configura la matriz para hacer zoom in u out
		view = aux*view;																		// se multiplica la matriz aux por la matriz view
	}

	if (button == 2){																			// si se pulsa el boton derecho del ratón....
		std::cout << "de la derecha del ratón " << std::endl;									// se muestra por consola que el botón se ha pulsado
		if (diferenciay <= 50){																	// se marca un margen para expresar el movimiento
			aux = glm::rotate(aux,angulo*(diferenciax/100.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// se configura la matriz para rotar en el eje Y
			view = aux*view;																	// se multiplica la matriz aux por la matriz view
		}		
	}

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;					// se muestra por pantalla que acción se ha realizado
	IGlib::setViewMat(view);																	// se le informa a la clase main que se actualiza la matriz
}