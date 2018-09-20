#include "BOX.h"
#include "auxiliar.h"
// 2.2.c Importamos la piramide
#include "PIRAMID.h"

#include <windows.h>
#include <gl/glew.h>
//No haría falta llamar a esta función
#include <gl/gl.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 
#include <iostream>//escribir en consola
//using namespace std;
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <iostream> mal hecho

/*
 Practica 3: Introducción a la programación en OpenGL
  1. Parte obligatoria
	1.1 Modifica las propiedades (intensidad y posición) de la luz a través del teclado.	1.2 Define una matriz de proyección que conserve el aspect ratio.
	1.3 Añade un nuevo cubo a la escena.
	1.4 Control de la cámara con el teclado.
 2. Parte opcional
	2.1 filtro anisotrópico.
	2.1 funcionalidad de las prácticas 1 y 2
		2.2.a Controla el giro de la cámara utilizando el ratón.
		2.2.b Crea un tercer cubo y hazlo orbitar alrededor del primero.
		2.2.c Crea un nuevo modelo.
		2.2.d Ilumina el objeto con luces de distinto tipo.
		2.2.e Añade atenuación con la distancia.
		2.2.f Implementa Bump Mapping.
		2.2.g Añade una textura especular.
 */

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);

// cubo 1
glm::mat4	model = glm::mat4(1.0f);

// 1.3 Creamos la matriz model del cubo
glm::mat4   model2 = glm::mat4(1.0f);

// 2.2.b Creamos la matriz model del cubo de Bezier
glm::mat4	model3 = glm::mat4(1.0f);

// 2.2.c Creamos la matriz model de la piramide
glm::mat4   model4 = glm::mat4(1.0f);

// 1.1 Creamos los vectores posicion e iluminacion para mas tarde operar con ellos en el metodo del teclado
// difusa
glm::vec3	posicion = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3	iluminacion = glm::vec3(1.0f);
// direccional
glm::vec3	posicion2 = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3	iluminacion2 = glm::vec3(1.0f);
// focal
glm::vec3	posicion3 = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3	iluminacion3 = glm::vec3(1.0f);

//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////

// se inicializan los shaders y el programa
unsigned int vshader;
unsigned int fshader;
unsigned int program;

//Variables Uniform
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;

//Atributos
int inPos;
int inColor;
int inNormal;
int inTexCoord;
// 2.2.f creamos la variable inTangente, atributo del shader de vertices que proporcionará las tangentes del objeto
int inTangent;

// Cubo
// VAO del cubo
unsigned int vao;

// VBOs que forman parte del cubo
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;
// 2.2.f VBO tangent
unsigned int tangentVBO;

// Piramide
// 2.2.c  creamos el VAO de la piramide
// VAO de la piramide
unsigned int vao2;

// VBOs que forman parte de la piramide
unsigned int posVBO2;
unsigned int colorVBO2;
unsigned int normalVBO2;
unsigned int tangentVBO2;
unsigned int texCoordVBO2;
unsigned int triangleIndexVBO2;

// Textura 
unsigned int colorTexId;
unsigned int emiTexId;
// 2.2.f se añade la textura de normales
unsigned int normalTexId;
// 2.2.g se añade la textura especular
unsigned int specularTexId;

// Texturas Uniform
int uColorTex;
int uEmiTex;
// 2.2.f se crea la variable uniform del shader de fragmentos, la cual direccionara a la textura de normales
int uNormalTex;
// 2.2.g Añade una textura especular del shader de fragmentos, la cual direccionara a la textura especular
int uSpecularTex;
// 1.1 variables uniform del shader de fragmentos las cuales son la intensidad y la posicion de la luz difusa
// difusa
int uintensidad;
int upl;
// direccional
int uintensidad2;
int upl2;
// focal
int uintensidad3;
int upl3;
// 1.4 para evitar que las luces se muevan al mover la camara será necesario llevar la matriz view al shader de vertices
GLint uView = -1;

//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////

//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
void initShader(const char *vname, const char *fname);
void initObj();
void destroy();

//Carga el shader indicado, devuele el ID del shader
GLuint loadShader(const char *fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
unsigned int loadTex(const char *fileName);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();

	if (glewIsSupported("GL_EXT_texture_filter_anisotropic")){
		std::cout << "soportado ";
	}
	else{
		std::cout << " no soportado ";
	}

	initShader("../shaders_P3/shader.v2.vert", "../shaders_P3/shader.v2.frag");
	initObj();

	glutMainLoop();
	destroy();

	return 0;
}
	
//////////////////////////////////////////
// Funciones auxiliares 
void initContext(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Prácticas GLSL");
	
	glewExperimental = GL_TRUE;
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
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

void initOGL(){
	//cerr << "Test1" << endl;
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 50.0f);
	view = glm::mat4(1.0f);
	view[3].z = -12;
}
void destroy(){
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	// 2.2.f borramos el buffer de tangent
	if (inTangent != -1) glDeleteBuffers(1, &tangentVBO);

	// 2.2.c borramos los buffers de la piramide
	if (inPos != -1) glDeleteBuffers(1, &posVBO2);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO2);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO2);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO2);
	if (inTangent != -1) glDeleteBuffers(1, &tangentVBO2);

	glDeleteBuffers(1, &triangleIndexVBO);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	// 2.2.c borramos el array de vertices
	glDeleteVertexArrays(1, &vao2);

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);
	// 2.2.f 
	glDeleteTextures(1, &normalTexId);
	// 2.2.g
	glDeleteTextures(1, &specularTexId);
}
void initShader(const char *vname, const char *fname){
	vshader = loadShader(vname, GL_VERTEX_SHADER);
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");
	// 2.2.f 
	glBindAttribLocation(program, 4, "inTangent");

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
		glDeleteShader(vshader);
		glDeleteShader(fshader);
		
		exit(-1);
	}

	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");

	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");
	// 2.2.f asignamos la textura de normales
	uNormalTex = glGetUniformLocation(program, "normalTex");
	// 2.2.g asignamos la textura especular
	uSpecularTex = glGetUniformLocation(program, "specularTex");
	// 1.1 asignamos las variables intensidad y posicion
	// difusa
	uintensidad = glGetUniformLocation(program, "Id");
	upl = glGetUniformLocation(program, "lpos");
	// direccional
	uintensidad2 = glGetUniformLocation(program, "Id2");
	upl2 = glGetUniformLocation(program, "lpos2");
	// focal
	uintensidad3 = glGetUniformLocation(program, "Id3");
	upl3 = glGetUniformLocation(program, "lpos3");
	// 1.4 asignamos la variable view
	uView = glGetUniformLocation(program, "View");

	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");
	// 2.2.f asignamos inTangent
	inTangent = glGetAttribLocation(program, "inTangent");
}

void initObj(){
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
	// 2.2.f 
	if (inTangent != -1)
	{
		glGenBuffers(1, &tangentVBO);
		glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float)* 3,
			cubeVertexTangent, GL_STATIC_DRAW);

		glVertexAttribPointer(inTangent, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTangent);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex*sizeof(unsigned int)* 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	// 2.2.c inicializamos la piramide
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);

	if (inPos != -1)
	{
		glGenBuffers(1, &posVBO2);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO2);
		glBufferData(GL_ARRAY_BUFFER, piramidNVertex*sizeof(float)* 3,
			piramidVertexPos, GL_STATIC_DRAW);

		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}
	if (inColor != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, piramidNVertex*sizeof(float)* 3,
			piramidVertexColor, GL_STATIC_DRAW);

		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}
	if (inNormal != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, piramidNVertex*sizeof(float)* 3,
			piramidVertexNormal, GL_STATIC_DRAW);

		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}
	if (inTexCoord != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, piramidNVertex*sizeof(float)* 2,
			piramidVertexTexCoord, GL_STATIC_DRAW);

		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}
	if(inTangent != -1)
	{
		glGenBuffers(1, &tangentVBO2);
		glBindBuffer(GL_ARRAY_BUFFER, tangentVBO2);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex*sizeof(float)* 3,
			piramidVertexTangent, GL_STATIC_DRAW);

		glVertexAttribPointer(inTangent, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTangent);
	}
	

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		piramidNTriangleIndex*sizeof(unsigned int)* 3, piramidTriangleIndex,
		GL_STATIC_DRAW);

	// inicializamos las matrices model
	model = glm::mat4(1.0f);
	// 1.3 inicializamos la matriz model del cubo
	model2 = glm::mat4(1.0f);	
	// 2.2.b inicializamos la matriz model del cubo de Bezier
	model3 = glm::mat4(1.0f);
	// 2.2.c inicializamos la matriz model de la piramide 
	model4 = glm::mat4(1.0f);
	
	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
	// 2.2.f asignamos la textura de normales
	normalTexId = loadTex("../img/normal.png");
	// 2.2.g asignamos la textura especular
	specularTexId = loadTex("../img/specmap.png");
}

GLuint loadShader(const char *fileName, GLenum type){ 
	
	unsigned int fileLen;
	char *source = loadStringFromFile(fileName, fileLen);
	//////////////////////////////////////////////
	// Creación y compilación del Shader /////////
	//////////////////////////////////////////////
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar **)&source, (const GLint *)&fileLen);
	glCompileShader(shader);
	delete source;//Se libera espacio en memoria principal

	//Comprobamos que se compiló bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		char *logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);

		std::cout << fileName << std::endl;
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteShader(shader);
		exit(-1);
	}
	
	return shader;
}
unsigned int loadTex(const char *fileName){ 
	
	unsigned char *map;//array con todos los colores de la textura
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
	
	// 2.1 activamos el filtro anisotropico
	GLfloat largest_supported_anisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);
	
	/*
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	*/
	return texId;
}

void renderFunc(){
	//cerr << "Test" << endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Or a nivel de bit	

	glUseProgram(program);

	// primer cubo
	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	// 1.1 asignamos a los atributos los valores posicion e iluminacion los cuales serán los modificados con el teclado
	// difusa
	glm::vec3 lpos = posicion;
	glm::vec3 Id = iluminacion;
	// direccional
	glm::vec3 lpos2 = posicion2;
	glm::vec3 Id2 = iluminacion2;
	// focal
	glm::vec3 lpos3 = posicion3;
	glm::vec3 Id3 = iluminacion3;

	// 1.4 asignamos al atributo view la matriz view	
	glm::mat4 View = view;

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));
	
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));

	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
		&(normal[0][0]));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
		
	// 1.3 añadimos el cubo a la escena
	modelView = view * model2;
	modelViewProj = proj * view * model2;
	normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));

	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));

	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
		&(normal[0][0]));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);

	// 2.2.b añadimos el cubo de Bezier a la escena
	modelView = view * model3;
	modelViewProj = proj * view * model3;
	normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));

	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));

	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
		&(normal[0][0]));
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
	
	// 2.2.c añadimos la piramide a la escena 
	modelView = view * model4;
	modelViewProj = proj * view * model4;
	normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
		&(modelView[0][0]));

	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
		&(modelViewProj[0][0]));

	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
		&(normal[0][0]));
	
	glBindVertexArray(vao2);
	glDrawElements(GL_TRIANGLES, piramidNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
	
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

	// 2.2.f añadimos la textura de normales
	if (uNormalTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, normalTexId);
		glUniform1i(uNormalTex, 2);
	}

	// 2.2.g añadimos la textura especular
	if (uSpecularTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, specularTexId);
		glUniform1i(uSpecularTex, 3);
	}

	// 1.1 añadimos las variables uniform intensidad y upl
	// difusa
	if (uintensidad != -1)
		glUniform3f(uintensidad, Id.x, Id.y, Id.z);

	if (upl != -1)
		glUniform3f(upl, lpos.x, lpos.y, lpos.z);
	// direccional
	if (uintensidad2 != -1)
		glUniform3f(uintensidad2, Id2.x, Id2.y, Id2.z);

	if (upl2 != -1)
		glUniform3f(upl2, lpos2.x, lpos2.y, lpos2.z);
	// focal
	if (uintensidad3 != -1)
		glUniform3f(uintensidad3, Id3.x, Id3.y, Id3.z);

	if (upl3 != -1)
		glUniform3f(upl3, lpos3.x, lpos3.y, lpos3.z);	
	// 1.4 añadimos la variable View para evitar que las luces se muevan cuando movamos la camara
	if (uView != -1)
		glUniformMatrix4fv(uView, 1, GL_FALSE, &(View[0][0]));
	
	glUseProgram(NULL);
	glutSwapBuffers();
}
void resizeFunc(int width, int height){

	glViewport(0, 0, width, height);
	// 1.2 definimos la matriz de proyeccion que ajustara el aspect ratio
	float w = width;
	float h = height;
	float aspect = w / h;
	glm::mat4 rproj(0.0f);
	rproj[0].x = (1.0f / tan(3.14159f / 6.0f));			// el primer valor como antes se explicó es 1/tan30
	rproj[1].y = rproj[0].x*aspect;						// el segundo valor como se mencionó arriba es 1/tan30 * ar
	rproj[2].z = -(100 + 0.1) / (100 - 0.1);			// se calcula el valor de acuerdo con la matriz original
	rproj[2].w = -1.0f;									// se calcula el valor de acuerdo con la matriz original
	rproj[3].z = -2.0f * 100 * 0.1 / (100 - 0.1);		// se calcula el valor de acuerdo con la matriz original
	proj = rproj;

	glutPostRedisplay();
}
void idleFunc(){
	
	model = glm::mat4(1.0f);
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.001f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));

	// 1.3 definimos el movimiento del cubo
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, angle, glm::vec3(0.0f, 1.0, 0.0f));		//Dar valor a la matriz de rotación
	glm::mat4 model2tras(1.0f);												//Declarar la matriz de traslacion
	model2tras = glm::translate(model2tras, glm::vec3(5.0f, 0.0f, 0.0f));	//Dar valor a la matriz de traslacion
	glm::mat4 escalado2(1.0f);												//Declarar la matriz de escalado
	escalado2 = glm::scale(escalado2, glm::vec3(0.5f, 0.5f, 0.5f));			//Dar valor a la matriz de escalado
	model2 *= model2tras*escalado2*model2;

	// 2.2.b definimos la curva de Bezier del cubo de Bezier el cual rotará alrededor del primer cubo
	float xt, zt;															// xt y zt serán las variables que almacenaran los valores de Bezier
	static float t = 0.0f;													// t será la variable encargada de mover el cubo por la curva
	t = (t <= 1.0f) ? t + 0.0001f : 0.0f;									// t siempre mantendra un valor entre 0 y 1
	glm::vec3 P0 = glm::vec3(8, 0, -6);										// P0 (8, 0, -6)
	glm::vec3 P1 = glm::vec3(-20, 0, -20);									// P1 (-20, 0, -20)
	glm::vec3 P2 = glm::vec3(20, 0, 100);									// P2 (20, 0, 100)
	glm::vec3 P3 = glm::vec3(8, 0, -6);;									// P3 (8, 0, -6)
	xt = pow((1 - t), 3)*P0.x + 3 * pow((1 - t), 2)*t*P1.x + (1 - t)*(pow(t, 2))*P2.x + pow(t, 3)*P3.x;	// x(t) = .....
	zt = pow((1 - t), 3)*P0.z + 3 * pow((1 - t), 2)*t*P1.z + (1 - t)*(pow(t, 2))*P2.z + pow(t, 3)*P3.z;	// z(t) = ....
	model3 = glm::mat4(1.0f);
	model3 = glm::translate(model3, glm::vec3(xt, 0.0f, zt));	
	model3 = glm::scale(model3, glm::vec3(0.5f, 0.5f, 0.5f));	

	// 2.2.c definimos el movimiento de la pirámide
	model4 = glm::mat4(1.0f);
	model4 = glm::translate(model4, glm::vec3(3.0f, 0.0f, 0.0f));
	model4 = glm::scale(model4, glm::vec3(0.5f, 0.5f, 0.5f));

	glutPostRedisplay();
}
void keyboardFunc(unsigned char key, int x, int y){

	// 1.1 definimos las modificaciones que sufrirán la iluminacion y la posicion de la luz
	// posicion de la luz
	if (key == 'w'&& key == 'q'){
		posicion.z -= 0.1;
		//posicion2.z -= 0.1;
		//posicion3.z -= 0.1;
	}
	if (key == 's'){
		posicion.z += 0.1;
		//posicion2.z += 0.1;
		//posicion3.z += 0.1;
	}
	if (key == 'a'){
		posicion.x -= 0.1;
		//posicion2.x -= 0.1;
		//posicion3.x -= 0.1;
	}
	if (key == 'd'){
		posicion.x += 0.1;
		//posicion2.x += 0.1;
		//posicion3.x += 0.1;
	}
	if (key == 'z'){
		posicion.y += 0.1;
		//posicion2.y += 0.1;
		//posicion3.y += 0.1;
	}
	if (key == 'x'){
		posicion.y -= 0.1;
		//posicion2.y -= 0.1;
		//posicion3.y -= 0.1;
	}
	// intensidad de la luz
	if (key == 'u'){
		/*
		// difusa red aumenta
		if (iluminacion.r < 1){
		iluminacion.r += 0.1;
		}
		*/
		/*
		// direccional red aumenta
		if (iluminacion2.r < 1){
		iluminacion2.r += 0.1;
		}
		*/
		 
		// focal red aumenta
		if (iluminacion3.r < 1){
			iluminacion3.r += 0.1;
		}
		 
	}
	if (key == 'j'){
		/*
		// difusa red disminuye
		if (iluminacion.r > 0){
			iluminacion.r -= 0.1;
		}
		*/
		/*
		// direccional red disminuye
		if (iluminacion2.r > 0){
			iluminacion2.r -= 0.1;
		}
		*/
		
		// focal red disminuye
		if (iluminacion3.r > 0){
			iluminacion3.r -= 0.1;
		}
		
	}
	if (key == 'i'){
		/*
		// difusa green aumenta
		if (iluminacion.g < 1){
		iluminacion.g += 0.1;
		}
		*/
		/*
		// direccional green aumenta
		if (iluminacion2.g < 1){
		iluminacion2.g += 0.1;
		}
		*/
		
		// focal green aumenta
		if (iluminacion3.g < 1){
			iluminacion3.g += 0.1;
		}
		
	}
	if (key == 'k'){
		///*
		// difusa green disminuye
		if (iluminacion.g > 0){
			iluminacion.g -= 0.1;
		}
		//*/
		/*
		// direccional green disminuye
		if (iluminacion2.g > 0){
			iluminacion2.g -= 0.1;
		}
		*/
		 
		// focal green disminuye
		if (iluminacion3.g > 0){
			iluminacion3.g -= 0.1;
		}
		 
	}
	if (key == 'o'){
		///*
		// difusa blue aumenta
		if (iluminacion.b < 1){
		iluminacion.b += 0.1;
		}
		//*/
		/*
		// direccional blue aumenta
		if (iluminacion2.b < 1){
		iluminacion2.b += 0.1;
		}
		*/
		
		// focal blue aumenta
		if (iluminacion3.b < 1){
		iluminacion3.b += 0.1;
		}
		
	}
	if (key == 'l'){
		/*
		// difusa blue disminuye
		if (iluminacion.b > 0){
			iluminacion.b -= 0.1;
		}
		*/
		/*
		// direccional blue disminuye
		if (iluminacion2.b > 0){
			iluminacion2.b -= 0.1;
		}
		*/
		
		// focal blue disminuye 
		if (iluminacion3.b > 0){
		iluminacion3.b -= 0.1;
		}
		
	}

	// 1.4 control de la cámara con el teclado		
	//traslacion
	glm::mat4 tras(1.0f);												// se crea una matrix auxilior para ser empleada como matriz de traslación

	//rotacion 
	glm::mat4 rot(1.0f);												// se crea una matriz auxiliar para ser empleada como matriz de rotación
	static float alfa = 0.01f;											// se crea un ángulo auxiliar para la rotación en el eje x
	static float beta = 0.01f;											// se crea un ángulo auxiliar para la rotación en el eje y

	if (key == '7'){												// si se pulsa la z la cámara se traslada en dirección abajo
		tras = glm::translate(tras, glm::vec3(0.0f, 0.1f, 0.0f));	// se configura la matriz de traslación para ser trasladada en el eje Y
		view = tras*view;											// se multiplica la matriz de traslación por la matriz view
	}

	if (key == '9'){												// si se pulsa la x la cámara se traslada en dirección arriba
		tras = glm::translate(tras, glm::vec3(0.0f, -0.1f, 0.0f));	// se configura la matriz de traslación para ser trasladada en el eje Y
		view = tras*view;											// se multiplica la matriz de traslación por la matriz view
	}

	if (key == '6'){												// si se pulsa la d la cámara se traslada en dirección derecha
		tras = glm::translate(tras, glm::vec3(-0.1f, 0.0f, 0.0f));	// se configura la matriz de traslacion para ser trasladada en el eje X
		view = tras*view;											// se multiplica la matriz de traslación por la matriz view
	}

	if (key == '4'){												// si se pulsa la a la cámara se traslada en dirección izquierda
		tras = glm::translate(tras, glm::vec3(0.1f, 0.0f, 0.0f));	// se configura la matriz de traslación para ser trasladada en el eje X
		view = tras*view;											// se multiplica la matriz de traslación por la matriz view
	}

	if (key == '8'){												// si se pulsa la w la cámara se traslada en dirección frontal
		tras = glm::translate(tras, glm::vec3(0.0f, 0.0f, 0.1f));	// se configura la matriz de traslación para ser trasladada en el eje Z
		view = tras*view;											// se multiplica la matriz de traslación por la matriz view
	}

	if (key == '2'){												// si se pulsa la s la cámara se traslada en dirección trasera
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

}

// 2.2.a giro de la cámara utilizando el ratón.
// variables del método mouseFunc
float diferenciax;		// diferenciax sera la variable que calcule la diferencia de la coordenada x original menos la final
float diferenciay;		// diferenciax sera la variable que calcule la diferencia de la coordenada y original menos la final
float actualx = 0.0f;	// actualx será la variable encargada de guardar la coordenada x incial
float actualy = 0.0f;	// actualx será la variable encargada de guardar la coordenada y incial
float angulo = 0.1f;	// angulo será la variable encargada de incrementar/ decrementar el angulo de rotacion de x e y

void mouseFunc(int button, int state, int x, int y){
	glm::mat4 aux(1.0f);										// se crea una matrix auxiliar que se empleara para trasladar y rotar

	if (state == 0){											// si se pulsa el raton...
		std::cout << "Se ha pulsado el botón ";					// se muestra por consola que se ha pulsado un botón del raton
		actualx = x;											// se recoge la coordenada x inicial
		actualy = y;											// se recoge la coordenada y inicial

	}
	else														// si se suelta el botón una vez pulsada...
		std::cout << "Se ha soltado el botón ";					// se muestra por consola que se ha soltado el botón del ratón
	diferenciax = actualx - x;									// se resta la coordenada x inicial por la actual 
	diferenciay = actualy - y;									// se resta la coordenada y inicial por la actual


	if (button == 0){																			// si se pulsa el boton izquierdo del ratón...
		std::cout << "de la izquierda del ratón " << std::endl;									// se muestra por consola que el botón se ha pulsado
		if (diferenciax <= 50){																	// se marca un margen para expresar el movimiento
			aux = glm::rotate(aux, angulo*(diferenciay / 100.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// se configura la matriz para rotar en el eje X
			view = aux*view;																	// se multiplica la matriz aux por la matriz view
		}
	}

	if (button == 1) {																			// si se pulsa el boton central del ratón...	
		std::cout << "central del ratón " << std::endl;											// se muestra por consola que el botón se ha pulsado
		aux = glm::translate(aux, glm::vec3(0.0f, 0.0f, diferenciay / 100.0f));					// se configura la matriz para hacer zoom in u out
		view = aux*view;																		// se multiplica la matriz aux por la matriz view
	}

	if (button == 2){																			// si se pulsa el boton derecho del ratón....
		std::cout << "de la derecha del ratón " << std::endl;									// se muestra por consola que el botón se ha pulsado
		if (diferenciay <= 50){																	// se marca un margen para expresar el movimiento
			aux = glm::rotate(aux, angulo*(diferenciax / 100.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// se configura la matriz para rotar en el eje Y
			view = aux*view;																	// se multiplica la matriz aux por la matriz view
		}
	}

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;					// se muestra por pantalla que acción se ha realizado
}