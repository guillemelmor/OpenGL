#include "BOX.h"
#include "auxiliar.h"
#include "PLANE.h"
#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstdlib>
//#define _glewUniform25f
#define RAND_SEED 31415926
#define SCREEN_SIZE 500,500					// tamaño de la ventana
#define MASK_SIZE 25u
//////////////////////////////////////////////////////////////////
///////  Datos que se almacenan en la memoria de la CPU	//////////
//////////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);			// matriz de proyeccion
glm::mat4	view = glm::mat4(1.0f);			// matriz view
glm::mat4	model = glm::mat4(1.0f);		// matriz model

float Mascara[25] = { 1.0f, 2.0f, 3.0f, 2.0f, 1.0f,
                    2.0f, 3.0f, 4.0f, 3.0f, 2.0f,
                    3.0f, 4.0f, 5.0f, 4.0f, 3.0f,
                    2.0f, 3.0f, 4.0f, 3.0f, 2.0f,
                    1.0f, 2.0f, 3.0f, 2.0f, 1.0f};

float factorMascara = 1.0f;
						 
// 1.1 Controla los parámetros del Motion Blur a través de teclado
// Creamos un vector de 4 coordenadas, el cual será sustituido por los parametros actuales
glm::vec4 glColor = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);

// 1.2 Controla los parámetros del DOF por teclado
// creamos los valores del DOF
float FocalDistance = -15.0f;
float MaxDistanceFactor = 1.0 / 5.0;

// 1.3 Creamos el valor de Far y Near
float Far = 50.0;
float Near = 1.0;
///////////////////////////////////////////////////////////////////
///////   Variables que nos dan acceso a Objetos OpenGL    ////////
///////////////////////////////////////////////////////////////////

float angle = 0.0f;

//VAO
unsigned int vao;							// almacenamiento de los BVOs

//VBOs que forman parte del objeto			// buffers
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;

unsigned int colorTexId;
unsigned int emiTexId;

// 1.3 Creamos el identificador de la textura de profundidad
unsigned int depthTexId;

// VBO y VAO 2 del plano
unsigned int planeVAO;
unsigned int planeVertexVBO;

// Programa:contenedores de shaders.
unsigned int vshader;
unsigned int fshader;
unsigned int program;

//Variables Uniform							// identificadores para poder subir informacion al shader.
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;

//Texturas Uniform
int uColorTex;
int uEmiTex;

// 1.3 identificador de la varibale uniform sampler 2D depthTex
int uDepthTex;

// 1.2 identificadores de las variables uniform focal distance y max focal distance
int uFocalDistance;
int uMaxFocalDistance;

// variables y atributos del shader de fragmentos para poder enlazar los shaders.
unsigned int postProccesVShader;
unsigned int postProccesFShader;
unsigned int postProccesProgram;
//Uniform
unsigned int uColorTexPP;
// 1.3 Creamos el uniform Far y Near
unsigned int uFar;
unsigned int uNear;
//1.4
unsigned int uMascaraTex[25] {2};
//Atributos
int inPosPP;

// identificador del framebuffer
unsigned int fbo;
unsigned int colorBuffTexId;

//Atributos									// identificadores de atributos.
int inPos;
int inColor;
int inNormal;
int inTexCoord;

//textura vertices 
unsigned int uVertexTexPP;
unsigned int vertexBuffTexId;

// 1.3 identificador de la textura en postproceso y el identificador del buffer respecto a la profundidad
unsigned int depthBuffTexId;
unsigned int uDepthTexPP;

//////////////////////////////////////////////////////////////
////////            Funciones auxiliares		//////////////
//////////////////////////////////////////////////////////////

//Declaración de CB
void renderFunc();										// función de renderizado
void resizeFunc(int width, int height);					// función resize
void idleFunc();										// función Idle
void keyboardFunc(unsigned char key, int x, int y);		// función del teclado
void mouseFunc(int button, int state, int x, int y);	// función del ratón
void renderCube();										// renderiza muchos cubos

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);				// inicialización del contexto
void initOGL();											// inicialización openGL
void initShaderFw(const char *vname, const char *fname);// inicializacion de los shaders.
void initShaderPP(const char *vname, const char *fname);// inicialización de los shaders de postproceso
void initObj();											// inicialización del objeto
void initPlane();										// inicialización del plano
void destroy();											// liberación de recursos
void initFBO();											// inicialización del frame buffer
void resizeFBO(unsigned int w, unsigned int h);			// función de reescalado del frame buffer

//Carga el shader indicado, devuele el ID del shader
GLuint loadShader(const char *fileName, GLenum type);	// funcion auxiliar para cargar shader

// Crea una textura, la configura, la sube a OpenGL, 
// y devuelve el identificador de la textura 
unsigned int loadTex(const char *fileName);				//funcion auxiliar para cargar texturas

//////////////////////////////////////////////////////////////
/////////////// Nuevas variables auxiliares  /////////////////
//////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);																		// inicialización del contexto
	initOGL();																						// inicialización de opengl
	initShaderFw("../shaders_P4/fwRendering.v1.vert", "../shaders_P4/fwRendering.v1.frag");			// inicialización de los shaders
	initObj();																						// inicialización del objeto
	initShaderPP("../shaders_P4/postProcessing.v1.vert", "../shaders_P4/postProcessing.v1.frag");	// inicialización del postproceso
	initPlane();																					// inicialización del plano
	initFBO();																						// inicialización del frame buffer
	resizeFBO(SCREEN_SIZE);																			// reajuste del frame buffer

	glutMainLoop();
	destroy();
	return 0;
}

//////////////////////////////////////////
///////// Funciones auxiliares  //////////
//////////////////////////////////////////

void initContext(int argc, char** argv)								// inicialización del contexto
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);									// versión de openGL
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_SIZE);								// tamaño de la ventana
	glutInitWindowPosition(0, 0);									// origen de la pantalla
	glutCreateWindow("Prácticas GLSL");								// titulo de la ventana
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)												// comprobamos que se ha inicializado con éxito
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
}

void initOGL()															// se inicializa openGL
{
	glEnable(GL_DEPTH_TEST);
	//	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);								// se limpia el fondo

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	proj = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 50.0f);	// se le da r a la matriz de proyeccion
	view = glm::mat4(1.0f);												// se crea la matriz view
	view[3].z = -25.0f;													// se le da valor a la matriz view
}

void destroy()													// función de liberación de recursos
{
	glDetachShader(program, vshader);							// se vacia el shader de vertices
	glDetachShader(program, fshader);							// se vacia el shader de fragmentos
	glDeleteShader(vshader);									// se elimina el shader de vertices
	glDeleteShader(fshader);									// se elimina el shader de fragmentos
	glDeleteProgram(program);									// se elimina el programa

	glDetachShader(postProccesProgram, postProccesVShader);		// se efectua el procedimiento anterior para el postproceso
	glDetachShader(postProccesProgram, postProccesFShader);		//	"			"			"			"			"
	glDeleteShader(postProccesVShader);							//	"			"			"			"			"
	glDeleteShader(postProccesFShader);							//	"			"			"			"			"
	glDeleteProgram(postProccesProgram);						//	"			"			"			"			"

	glBindBuffer(GL_ARRAY_BUFFER, 0);							// se liberan los recursos empleados en el plano
	glDeleteBuffers(1, &planeVertexVBO);						//	"			"			"			"
	glBindVertexArray(0);										//	"			"			"			"
	glDeleteVertexArrays(1, &planeVAO);							//	"			"			"			"

	glBindFramebuffer(GL_FRAMEBUFFER,0);						
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &colorBuffTexId);
	glDeleteTextures(1, &depthBuffTexId);
	glDeleteTextures(1, &vertexBuffTexId);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);
}

void initShaderFw(const char *vname, const char *fname)			// función que inicializa los shaders
{
	vshader = loadShader(vname, GL_VERTEX_SHADER);				// se inicializa el shader de vertices
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);			// se inicializa el shader de fragmentos

	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");

	glLinkProgram(program);

	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetProgramInfoLog(program, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;

		glDeleteProgram(program);
		program = 0;
		exit(-1);
	}

	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");
	
	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");


	// 1.3 definimos la variable uniform depthText
	uDepthTex = glGetUniformLocation(program, "depthTex");	

	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");
}

void initObj()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (inPos != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float)* 3,
			cubeVertexPos, GL_STATIC_DRAW);
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}

	if (inColor != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float)* 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}

	if (inNormal != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float)* 3,
			cubeVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}


	if (inTexCoord != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float)* 2,
			cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex*sizeof(unsigned int)* 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	model = glm::mat4(1.0f);

	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
}

GLuint loadShader(const char *fileName, GLenum type)
{
	unsigned int fileLen;
	char *source = loadStringFromFile(fileName, fileLen);

	/////////////////////////////////////////////////
	/////// Creación y compilación del Shader ///////
	/////////////////////////////////////////////////

	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar **)&source, (const GLint *)&fileLen);
	glCompileShader(shader);
	delete source;

	//Comprobamos que se compilo bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;

		glDeleteShader(shader);
		exit(-1);
	}
	return shader;
}

unsigned int loadTex(const char *fileName)
{
	unsigned char *map;
	unsigned int w, h;
	map = loadTexture(fileName, w, h);

	if (!map)
	{
		std::cout << "Error cargando el fichero: "
			<< fileName << std::endl;
		exit(-1);
	}

	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, (GLvoid*)map);
	delete[] map;
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	return texId;
}

void renderFunc()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	//Texturas
	if (uColorTex != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexId);
		glUniform1i(uColorTex, 0);
	}

	if (uEmiTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, emiTexId);
		glUniform1i(uEmiTex, 1);
	}

	// 1.3 referenciamos la variable de la textura de profundidad
	if (uDepthTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, depthTexId);
		glUniform1i(uDepthTex, 2);
	}
	model = glm::mat4(2.0f);
	model[3].w = 1.0f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));
	renderCube();

	std::srand(RAND_SEED);

	for (unsigned int i = 0; i < 10; i++)
	{
		float size = float(std::rand() % 3 + 1);

		glm::vec3 axis(glm::vec3(float(std::rand() % 2),
			float(std::rand() % 2), float(std::rand() % 2)));
		if (glm::all(glm::equal(axis, glm::vec3(0.0f))))
			axis = glm::vec3(1.0f);

		float trans = float(std::rand() % 7 + 3) * 1.00f + 0.5f;
		glm::vec3 transVec = axis * trans;
		transVec.x *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.y *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.z *= (std::rand() % 2) ? 1.0f : -1.0f;

		model = glm::rotate(glm::mat4(1.0f), angle*2.0f*size, axis);
		model = glm::translate(model, transVec);
		model = glm::rotate(model, angle*2.0f*size, axis);
		model = glm::scale(model, glm::vec3(1.0f / (size*0.7f)));
		renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(postProccesProgram);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); uno x alpha y 1 menos alpha
	// fuente, destino
	glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);

	// 1.1 Controla los parámetros del Motion Blur a través de teclado
	// le definimos el vector glColor a glBlenColor y lo mostramos por consola
	glBlendColor(glColor.x, glColor.y, glColor.z, glColor.w);

	// 1.2 definimos las variables que controlarán el DAO
	float focalDistance = FocalDistance;
	float maxDistanceFactor = MaxDistanceFactor;
	// 1.3
	float variable_near = Near;
	float variable_far = Far;
	for (int j = 0; j < MASK_SIZE; j++){
		factorMascara += Mascara[j];
	}
	factorMascara = 1 / factorMascara;
	float mascara[25] = {Mascara[0] * factorMascara, Mascara[1] * factorMascara, Mascara[2] * factorMascara, Mascara[3] * factorMascara, Mascara[4] * factorMascara,
		Mascara[5] * factorMascara, Mascara[6] * factorMascara, Mascara[7] * factorMascara, Mascara[8] * factorMascara, Mascara[9] * factorMascara,
		Mascara[10] * factorMascara, Mascara[11] * factorMascara, Mascara[12] * factorMascara, Mascara[13] * factorMascara, Mascara[14] * factorMascara,
		Mascara[15] * factorMascara, Mascara[16] * factorMascara, Mascara[17] * factorMascara, Mascara[18] * factorMascara, Mascara[19] * factorMascara,
		Mascara[20] * factorMascara, Mascara[21] * factorMascara, Mascara[22] * factorMascara, Mascara[23] * factorMascara, Mascara[24] * factorMascara};

	if (uFocalDistance != -1){
		glUniform1f(uFocalDistance, focalDistance);
	}
	if (uMaxFocalDistance != -1){
		glUniform1f(uMaxFocalDistance, maxDistanceFactor);
	}
	if (uFar != -1){
		glUniform1f(uFar, Far);
	}
	if (uNear != -1){
		glUniform1f(uNear, Near);
	}
	
	if (uMascaraTex [25] != -1){
		glUniform1fv(uMascaraTex [25], 25, mascara);
	}
	// operación suma
	glBlendEquation(GL_FUNC_ADD);

	if (uColorTexPP != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
		glUniform1i(uColorTexPP, 0);
	}

	if (uVertexTexPP != -1) //EXAMEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEH 
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, vertexBuffTexId);
		glUniform1i(uVertexTexPP, 1);
	}
	// 1.3 referenciamos la variable uniform depthText del shader de post process
	if (uDepthTexPP != -1) 
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, depthBuffTexId);
		glUniform1i(uDepthTexPP, 2);
	}

	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);

	glUseProgram(NULL);
	glutSwapBuffers();
}

void renderCube()
{


	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(normal[0][0]));


	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
}

void resizeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(60.0f), float(width) / float(height), 1.0f, 50.0f);

	resizeFBO(width, height);

	glutPostRedisplay();
}

void idleFunc()
{
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.001f;
	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y){

	if (key == '1'){
		
		Mascara [0] = 0;
		Mascara [1] = 0;
		Mascara [2] = 0;
		Mascara [3] = 0;
		Mascara [4] = 0;

		Mascara [5] = 0;
		Mascara [6] = 1;
		Mascara [7] = 1;
		Mascara [8] = 1;
		Mascara [9] = 0;

		Mascara [10] = 0;
		Mascara [11] = 1;
		Mascara [12] = -8;
		Mascara [13] = 1;
		Mascara [14] = 0;

		Mascara [15] = 0;
		Mascara [16] = 1;
		Mascara [17] = 1;
		Mascara [18] = 1;
		Mascara [19] = 0;

		Mascara [20] = 0;
		Mascara [21] = 0;
		Mascara [22] = 0;
		Mascara [23] = 0;
		Mascara [24] = 0;
		
	}
	if (key == '2'){

		Mascara[0] = 1;
		Mascara[1] = 2;
		Mascara[2] = 3;
		Mascara[3] = 2;
		Mascara[4] = 1;

		Mascara[5] = 2;
		Mascara[6] = 3;
		Mascara[7] = 4;
		Mascara[8] = 3;
		Mascara[9] = 2;

		Mascara[10] = 3;
		Mascara[11] = 4;
		Mascara[12] = 5;
		Mascara[13] = 4;
		Mascara[14] = 3;

		Mascara[15] = 2;
		Mascara[16] = 3;
		Mascara[17] = 4;
		Mascara[18] = 3;
		Mascara[19] = 2;

		Mascara[20] = 1;
		Mascara[21] = 2;
		Mascara[22] = 3;
		Mascara[23] = 2;
		Mascara[24] = 1;
	}
	if (key == '3'){
		Mascara[0] = 0;
		Mascara[1] = 0;
		Mascara[2] = 0;
		Mascara[3] = 0;
		Mascara[4] = 0;

		Mascara[5] = 0;
		Mascara[6] = -2;
		Mascara[7] = -1;
		Mascara[8] = 0;
		Mascara[9] = 0;

		Mascara[10] = 0;
		Mascara[11] = -1;
		Mascara[12] = 1;
		Mascara[13] = 1;
		Mascara[14] = 0;

		Mascara[15] = 0;
		Mascara[16] = 2;
		Mascara[17] = 1;
		Mascara[18] = 0;
		Mascara[19] = 0;

		Mascara[20] = 0;
		Mascara[21] = 0;
		Mascara[22] = 0;
		Mascara[23] = 0;
		Mascara[24] = 0;
	}
	if (key == '4'){
		Mascara[0] = 0;
		Mascara[1] = 0;
		Mascara[2] = 0;
		Mascara[3] = 0;
		Mascara[4] = 0;

		Mascara[5] = 0;
		Mascara[6] = -1;
		Mascara[7] = -1;
		Mascara[8] = -1;
		Mascara[9] = 0;

		Mascara[10] = 0;
		Mascara[11] = -1;
		Mascara[12] = 9;
		Mascara[13] = -1;
		Mascara[14] = 0;

		Mascara[15] = 0;
		Mascara[16] = -1;
		Mascara[17] = -1;
		Mascara[18] = -1;
		Mascara[19] = 0;

		Mascara[20] = 0;
		Mascara[21] = 0;
		Mascara[22] = 0;
		Mascara[23] = 0;
		Mascara[24] = 0;
	}
	// 1.1 Controla los parámetros del Motion Blur a través de teclado
	// aumenta la coordenada x o red 
	if (key == 't'){
		if (glColor.x < 1){
			glColor.x += 0.01;
		}
	}
	// disminuye la coordenada x o red 
	if (key == 'g'){
		if (glColor.x > 0){
			glColor.x -= 0.01;
		}
	}
	// aumenta la coordenada y o green
	if (key == 'y'){
		if (glColor.y < 1){
			glColor.y += 0.01;
		}
	}
	// disminuye la coordenada y o green
	if (key == 'h'){
		if (glColor.y > 0){
			glColor.y -= 0.01;
		}
	}
	// aumenta la coordenada z o blue
	if (key == 'u'){
		if (glColor.z < 1){
			glColor.z += 0.01;
		}
	}
	// disminuye la coordenada z o blue
	if (key == 'j'){
		if (glColor.z > 0){
			glColor.z -= 0.01;
		}
	}
	// aumenta la coordenada w o alpha
	if (key == 'i'){
		if (glColor.w < 1){
			glColor.w += 0.01;
		}
	}
	// disminuye la coordenada w o alpha
	if (key == 'k'){
		if (glColor.w > 0){
			glColor.w -= 0.01;
		}
	}

	// 1.2 Controla los parámetros del DOF por teclado
	// aumenta la distancia focal
	if (key == 'z'){
		FocalDistance += 0.1;
	}
	// disminuye la distancia focal
	if (key == 'x'){
		FocalDistance -= 0.1;
	}
	// aumenta el factor de la distacia (desenfoque)
	if (key == 'c'){
		if (MaxDistanceFactor < 1){
			MaxDistanceFactor += 0.1;
		}
	}
	// disminuye el factor de la distancia (desenfoque)
	if (key == 'v'){
		if (MaxDistanceFactor > 0.1){
			MaxDistanceFactor -= 0.1;
		}
	}
}

void mouseFunc(int button, int state, int x, int y){}

void initShaderPP(const char *vname, const char *fname){
	postProccesVShader = loadShader(vname, GL_VERTEX_SHADER);
	postProccesFShader = loadShader(fname, GL_FRAGMENT_SHADER);
	postProccesProgram = glCreateProgram();

	glAttachShader(postProccesProgram, postProccesVShader); //ENLAZAR SHADERS
	glAttachShader(postProccesProgram, postProccesFShader);
	glBindAttribLocation(postProccesProgram, 0, "inPos");
	glLinkProgram(postProccesProgram);

	int linked;
	glGetProgramiv(postProccesProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(postProccesProgram, GL_INFO_LOG_LENGTH, &logLen);
		char *logString = new char[logLen];
		glGetProgramInfoLog(postProccesProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(postProccesProgram);
		postProccesProgram = 0;
		exit(-1);
	}

	uVertexTexPP = glGetUniformLocation(postProccesProgram, "vertexTex");
	uColorTexPP = glGetUniformLocation(postProccesProgram, "colorTex"); //identificadores para subir variables
	// 1.3 referenciamos la variable depthTex postprocess
	uDepthTexPP = glGetUniformLocation(postProccesProgram, "depthTex");
	inPosPP = glGetAttribLocation(postProccesProgram, "inPos");

	// 1.2 asignamos las variables DAO
	uFocalDistance = glGetUniformLocation(postProccesProgram, "focalDistance");
	uMaxFocalDistance = glGetUniformLocation(postProccesProgram, "maxDistanceFactor");
	// 1.3 asignamos valores al far y near
	uNear = glGetUniformLocation (postProccesProgram, "variable_near");
	uFar = glGetUniformLocation(postProccesProgram, "variable_far");
	//1.4
	uMascaraTex [25] = glGetUniformLocation(postProccesProgram, "mascara");
}

void initPlane(){
	
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	glGenBuffers(1, &planeVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexVBO); //activar vbo
	glBufferData(GL_ARRAY_BUFFER, planeNVertex*sizeof(float)* 3, //reservar espacio vbo
		planeVertexPos, GL_STATIC_DRAW);
	glVertexAttribPointer(inPosPP, 3, GL_FLOAT, GL_FALSE, 0, 0); //3 componentes tipo float sin normalizar, stride y offset
	glEnableVertexAttribArray(inPosPP);
}

void initFBO(){
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &colorBuffTexId);
	glGenTextures(1, &depthBuffTexId);
	glGenTextures(1, &vertexBuffTexId);
	//glGenTextures(1, &vertexBuffTexId);
}

void resizeFBO(unsigned int w, unsigned int h){
	glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
		GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // INTERPOLACION BILINEAL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, depthBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, vertexBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, vertexBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffTexId, 0);

	const GLenum buffs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(2, buffs);


	//const GLenum buffs[1] = {GL_COLOR_ATTACHMENT3}; //arrays de colorattachment
	//glDrawBuffers(1, buffs);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		std::cerr << "Error configurando el FBO" << std::endl;
		exit(-1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}