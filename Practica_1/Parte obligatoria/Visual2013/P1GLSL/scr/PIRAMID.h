#ifndef __PYRAMIDFILE__
#define __PYRRAMIDFILE__

//Describimos una pirámide
//*******************
//Se replican vértices para que cada cara 
//tenga una normal distinta.


//Número de vértices
const int piramidNVertex = 16; // 3 vértices x 4 caras + 4vértices x 1 cara
const int piramidNTriangleIndex = 6; //1 triángulo por cara x 4 caras + 2 traingulos por cara 1 cara

//definir los vertices de los triangulos de cada cara de la pirámide
const unsigned int piramidTriangleIndex[] = {
	//Cara z = 1
	0,1,2,
	//Cara z = -1
	3, 4, 5, 
	//Cara x = 1
	6, 7, 8,
	//Cara x = -1
	9, 10, 11, 
	//Cara y = -1
	12, 14, 13,  13, 14, 15,
};

//Posicíon de los vertices 
const float piramidVertexPos[] = {
	
	//Cara z = 1
	0.0f, 2.0f, 0.0f, //0
	-1.0f, -1.0f, 1.0f, //1
	1.0f, -1.0f, 1.0f, //2
	//Cara z = -1
	0.0f, 2.0f, 0.0f, //3
	1.0f, -1.0f, -1.0f, //4
	-1.0f, -1.0f, -1.0f, //5
	//Cara x = 1
	0.0f, 2.0f, 0.0f, //6
	1.0f, -1.0f, 1.0f, //7
	1.0f, -1.0f, -1.0f, //8

	//Cara x = -1
	0.0f, 2.0f, 0.0f, //9
	-1.0f, -1.0f, -1.0f, //10
	-1.0f, -1.0f, 1.0f, //11
	//Cara y = -1
	-1.0f, -1.0f, -1.0f, //12
	-1.0f, -1.0f, 1.0f, //13
	1.0f, -1.0f, -1.0f, //14
	1.0f, -1.0f, 1.0f, //15
   

};


//Normal de los vertices
const float piramidVertexNormal[] = {
	//Cara z = 1
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,


	//Cara z = -1		   
	0.0f, 1.0f, -1.0f,
	0.0f, 1.0f, -1.0f,
	0.0f, 1.0f, -1.0f,


	//Cara x = 1		   
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,


	//Cara x = -1		   
	-1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,


	//Cara y = -1		   
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

//Color de los vertices
const float piramidVertexColor[] = { 
	//Cara z = 1
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 


	//Cara z = -1		   
	0.6f,	0.6f,	 1.0f, 
	0.6f,	0.6f,	 1.0f, 
	0.6f,	0.6f,	 1.0f, 


	//Cara x = 1		   
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 


	//Cara x = -1		   
	1.0f,	0.6f,	 0.6f, 
	1.0f,	0.6f,	 0.6f, 
	1.0f,	0.6f,	 0.6f, 



	//Cara y = -1		   
	0.6f,	1.0f,	0.6f, 
	0.6f,	1.0f,	0.6f, 
	0.6f,	1.0f,	0.6f, 

 };

//Cood. de textura de los vertices
const float piramidVertexTexCoord[] = { 
	//Cara z = 1
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 0.0f, 1.0f,

	//Cara z = -1
	0.0f, 1.0f, 
	1.0f, 1.0f, 
	0.0f, 0.0f, 


	//Cara x = 1	
	0.0f,	1.0f,
	0.0f,	1.0f,
	0.0f,	0.0f,

	//Cara x = -1
	0.0f,	0.0f,
	1.0f,	0.0f,
	0.0f,	1.0f,


	//Cara y = -1
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
 };

//Tangente de cada vertice
const float piramidVertexTangent[] = {
	//Cara z = 1
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,


	//Cara z = -1				
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,


	//Cara x = 1				
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,


	//Cara x = -1				
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,



	//Cara y = -1					   
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
};
 
#endif


