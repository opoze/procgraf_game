#define GLM_ENABLE_EXPERIMENTAL true

#include <GLM/vec3.hpp>
#include "ShapeGenerator.h"
#include "Vertex.h"
#include <iostream>
#include <GLM/gtx/string_cast.hpp>


#define QTD_VERTEX_PER_TRI 3
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

ShapeData ShapeGenerator::triangle(unsigned short qtyTri) {

	ShapeData ret;

	const unsigned short qtyVertex = qtyTri *  QTD_VERTEX_PER_TRI;
	ret.vertices= new Vertex[qtyVertex];
	ret.indices = new GLushort[qtyVertex];

	float size = 0.03;

	// Populate trisData & indexData
	for (int i = 0; i < qtyTri; ++i) {
		
		int index = i * QTD_VERTEX_PER_TRI;
		ret.vertices[index] = { glm::vec3(-size, size, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) };
		ret.vertices[index+1] = { glm::vec3(size, size, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		ret.vertices[index+2] = { glm::vec3(0.0f, -size, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
		
		ret.indices[index] = (GLushort)index;
		ret.indices[index+1] = (GLushort)index+1;
		ret.indices[index+2] = (GLushort)index+2;

		ret.numVertices+=3;
		ret.numIndices+=3;

	}

	return ret;
};

