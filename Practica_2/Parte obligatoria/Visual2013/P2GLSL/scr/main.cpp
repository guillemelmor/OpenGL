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
/*
int objId3 = -2;	//Piramide de la parte opcional
*/
//Declaración de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P2/shader.v3.vert", "../shaders_P2/shader.v3.frag"))
		return -1;
   
	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setIdleCB(idleFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);

	//Se ajusta la cámara
	//Si no se da valor se cojen valores por defecto

	view[3].z = -12;

	glm::mat4 proj = glm::mat4(1.0);
	float f = 1.0f / tan(3.141592 / 6);
	float far = 10.0;
	float near = 0.1;

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near - far);
	proj[2].w = -1.0f;
	proj[3].z = (2.0f * far * near) / (near - far);
	proj[3].w = 0.0f;
	IGlib::setProjMat(proj);
	IGlib::setViewMat(view);

	// creación de los objetos

	//Creamos el objeto central
	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

	//Se crea el objeto de la parte obligatoria
	objId2 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);
	/*
	//Se crea la pirámide de la parte opcional
	objId3 = IGlib::createObj(piramidNTriangleIndex, piramidNVertex, piramidTriangleIndex,
		piramidVertexPos, piramidVertexColor, piramidVertexNormal, piramidVertexTexCoord, piramidVertexTangent);
	*/
	

	//Texturas del objeto
	IGlib::addColorTex(objId, "../img/color2.png");
	IGlib::addEmissiveTex(objId, "../img/emissive.png");
	IGlib::addSpecularTex(objId, "../img/specmap.png");
	IGlib::addNormalTex(objId, "../img/normal.png");
	
	IGlib::addColorTex(objId2, "../img/color2.png");
	IGlib::addEmissiveTex(objId2, "../img/emissive.png");
	IGlib::addSpecularTex(objId2, "../img/specmap.png");
	IGlib::addNormalTex(objId2, "../img/normal.png");
	/*
	IGlib::addColorTex(objId3, "../img/color2.png");
	IGlib::addEmissiveTex(objId3, "../img/emissive.png");
	IGlib::addSpecularTex(objId3, "../img/specmap.png");
	IGlib::addNormalTex(objId3, "../img/normal.png");
	*/
		
	glm::mat4 modelMat = glm::mat4(1.0f);
	IGlib::setModelMat(objId, modelMat);
	glm::mat4 modelMat2 = glm::mat4(1.0f);
	IGlib::setModelMat(objId2, modelMat2);
	
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

void resizeFunc(int width, int height)					//Ajusta el aspect ratio al tamaño de la venta
{
	glm::mat4 rproj(0.0f);								// se crea la matriz que emplearemos como nueva matriz proyección
	if (height == 0) height = 1;						// para prevenir indeterminación impedimos el valor nulo
	float w = width;									// para obtener decimales, se cambia el valor de entero a float
	float h = height;									// para obtener decimales, se cambia el valor de entero a float
	float aspect = (w / h);								// calculamos el cociente de aspect ratio a partir de width y height
	float far = 100.0f;									// establecemos el valor a la variable far	
	float near = 0.1f;									// establecemos el valor a la variable near

	// matriz rproj, es decir, la nueva matriz proyección
	rproj[0].x = (1.0f / tan(3.14159f / 6.0f));			// el primer valor como antes se explicó es 1/tan30
	rproj[1].y = rproj[0].x*aspect;						// el segundo valor como se mencionó arriba es 1/tan30 * ar
	rproj[2].z = -(far + near) / (far - near);			// se calcula el valor de acuerdo con la matriz original
	rproj[2].w = -1.0f;									// se calcula el valor de acuerdo con la matriz original
	rproj[3].z = -2.0f * far * near / (far - near);		// se calcula el valor de acuerdo con la matriz original

	IGlib::setProjMat(rproj);							// le indicamos a la clase main que la matriz rproj es la matriz proyección actual
}

void idleFunc()
{
	glm::mat4 modelMat(1.0f);
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.001f;
	modelMat = glm::rotate(modelMat, angle, glm::vec3(1.0f, 1.0f, 0.0f));
	IGlib::setModelMat(objId, modelMat);

	//cubo rotatorio
	glm::mat4 model2(1.0f);												//Declarar la matriz de rotacion		
	model2 = glm::rotate(model2, angle, glm::vec3(0.0f, 1.0, 0.0f));  //Dar valor a la matriz de rotación
	glm::mat4 model2tras(1.0f);												//Declarar la matriz de traslacion
	model2tras = glm::translate(model2tras, glm::vec3(5.0f, 0.0f, 0.0f));	//Dar valor a la matriz de traslacion
	glm::mat4 escalado2(1.0f);												//Declarar la matriz de escalado
	escalado2 = glm::scale(escalado2, glm::vec3(0.5f, 0.5f, 0.5f));			//Dar valor a la matriz de escalado
	model2 *= model2tras*escalado2;					        //multiplicar las matrices de las transformaciones
	IGlib::setModelMat(objId2, model2);

	//piramide 
	/*
	glm::mat4 model3tras(1.0f);
	model3tras = glm::translate(model3tras, glm::vec3(6.0f, 0.0f, 0.0f));
	glm::mat4 model3rot(1.0f);												//
	//model3rot = glm::rotate(model3rot, angle, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 escalado3(1.0f);												//Declarar la matriz de escalado
	//escalado3 = glm::scale(escalado3, glm::vec3(0.5f, 0.5f, 0.5f));
	IGlib::setModelMat(objId3, model3tras*model3rot*escalado3);
	*/
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


void mouseFunc(int button, int state, int x, int y)
{
	if (state==0)
		std::cout << "Se ha pulsado el botón ";
	else
		std::cout << "Se ha soltado el botón ";
	
	if (button == 0) std::cout << "de la izquierda del ratón " << std::endl;
	if (button == 1) std::cout << "central del ratón " << std::endl;
	if (button == 2) std::cout << "de la derecha del ratón " << std::endl;

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;
}
