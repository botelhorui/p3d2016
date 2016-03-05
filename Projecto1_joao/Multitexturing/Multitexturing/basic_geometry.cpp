/* --------------------------------------------------
Basic Revolution Geometry 
 *
 * based on vsResourceLib.cpp from Lighthouse3D
 *
 *
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs

 João Madeiras Pereira
----------------------------------------------------*/
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// include GLEW to access OpenGL 3.3 functions
#include "OpenGL\glew\glew.h"

// GLUT is the toolkit to interface with the OS
#include "OpenGL\freeglut\freeglut.h"
#include "VertexAttrDef.h"
#include "basic_geometry.h"
#include "cube.h"
#include "AVTmathLib.h"

extern struct MyMesh mesh[];
extern int objId;

GLuint VboId[2];

void createSkybox() {
	mesh[objId].numIndexes = faceCount * 3;

	float tangent[24 * 4];

	glGenVertexArrays(1, &(mesh[objId].vao));
	glBindVertexArray(mesh[objId].vao);

	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(sizeof(vertices)));


	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh[objId].numIndexes, faceIndex, GL_STATIC_DRAW);

	// unbind the VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(TEXTURE_COORD_ATTRIB);

	mesh[objId].type = GL_TRIANGLES;
}

void createCube() {

	mesh[objId].numIndexes = faceCount *3;

	//void CalculateTangentArray(long vertexCount, const float *vertex, const float *normal, const float *texcoord, long triangleCount, const short *triangle, float *tangent)
	float tangent[24 * 4];
	//float *tangent;
	//tangent = (float *)malloc(sizeof(tangent) * 24 * 4);
	CalculateTangentArray(verticeCount, vertices, normals, texCoords, faceCount, faceIndex, tangent);
	//std::cout << "Size of tangent: " << sizeof(tangent) << "\n";

	glGenVertexArrays(1, &(mesh[objId].vao));
	glBindVertexArray(mesh[objId].vao);

	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals) + sizeof(texCoords) + sizeof(tangent), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), sizeof(texCoords), texCoords);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals) + sizeof(texCoords), sizeof(tangent), tangent);

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(NORMAL_ATTRIB);
	glVertexAttribPointer(NORMAL_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)sizeof(vertices));
	glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(sizeof(vertices) + sizeof(normals)));
	glEnableVertexAttribArray(TANGENT_ATTRIB);
	glVertexAttribPointer(TANGENT_ATTRIB, 4, GL_FLOAT, 0, 0, (void *)(sizeof(vertices) + sizeof(normals) + sizeof(texCoords)));


	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh[objId].numIndexes, faceIndex , GL_STATIC_DRAW);

// unbind the VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(NORMAL_ATTRIB);
	glDisableVertexAttribArray(TEXTURE_COORD_ATTRIB);
	glDisableVertexAttribArray(TANGENT_ATTRIB);

	mesh[objId].type = GL_TRIANGLES;
}

void CalculateTangentArray(long vertexCount, const float *vertex, const float *normal, const float *texcoord, long triangleCount, const unsigned *triangle, float *tangent){
	float tan1[144];
	float tan2[72];
	ZeroMemory(tan1, (vertexCount * sizeof(float) * 2) * 3);
	ZeroMemory(tan2, (vertexCount * sizeof(float)) * 3);

	//std::cout << "Size of tan1: " << sizeof(tan1) << "\n";
	//std::cout << "Size of tan2: " << sizeof(tan2) << "\n";

	for (long a = 0; a < triangleCount; a++){
		long i1 = triangle[(a * 3) + 0];
		long i2 = triangle[(a * 3) + 1];
		long i3 = triangle[(a * 3) + 2];

		const float v1x = vertex[(i1 * 4) + 0];
		const float v1y = vertex[(i1 * 4) + 1];
		const float v1z = vertex[(i1 * 4) + 2];
		const float v2x = vertex[(i2 * 4) + 0];
		const float v2y = vertex[(i2 * 4) + 1];
		const float v2z = vertex[(i2 * 4) + 2];
		const float v3x = vertex[(i3 * 4) + 0];
		const float v3y = vertex[(i3 * 4) + 1];
		const float v3z = vertex[(i3 * 4) + 2];

		const float w1x = texcoord[(i1 * 4) + 0];
		const float w1y = texcoord[(i1 * 4) + 1];
		const float w2x = texcoord[(i2 * 4) + 0];
		const float w2y = texcoord[(i2 * 4) + 1];
		const float w3x = texcoord[(i3 * 4) + 0];
		const float w3y = texcoord[(i3 * 4) + 1];

		float x1 = v2x - v1x;
		float x2 = v3x - v1x;
		float y1 = v2y - v1y;
		float y2 = v3y - v1y;
		float z1 = v2z - v1z;
		float z2 = v3z - v1z;

		float s1 = w2x - w1x;
		float s2 = w3x - w1x;
		float t1 = w2y - w1y;
		float t2 = w3y - w1y;

		float r = 1.0F / (s1 * t2 - s2 * t1);

		float sdir[3];
		sdir[0] = (t2 * x1 - t1 * x2) * r;
		sdir[1] = (t2 * y1 - t1 * y2) * r;
		sdir[2] = (t2 * z1 - t1 * z2) * r;

		float tdir[3];
		tdir[0] = (s1 * x2 - s2 * x1) * r;
		tdir[1] = (s1 * y2 - s2 * y1) * r;
		tdir[2] = (s1 * z2 - s2 * z1) * r;

		tan1[(i1 * 3) + 0] += sdir[0];
		tan1[(i1 * 3) + 1] += sdir[1];
		tan1[(i1 * 3) + 2] += sdir[2];
		tan1[(i2 * 3) + 0] += sdir[0];
		tan1[(i2 * 3) + 1] += sdir[1];
		tan1[(i2 * 3) + 2] += sdir[2];
		tan1[(i3 * 3) + 0] += sdir[0];
		tan1[(i3 * 3) + 1] += sdir[1];
		tan1[(i3 * 3) + 2] += sdir[2];

		tan2[(i1 * 3) + 0] += tdir[0];
		tan2[(i1 * 3) + 1] += tdir[1];
		tan2[(i1 * 3) + 2] += tdir[2];
		tan2[(i2 * 3) + 0] += tdir[0];
		tan2[(i2 * 3) + 1] += tdir[1];
		tan2[(i2 * 3) + 2] += tdir[2];
		tan2[(i3 * 3) + 0] += tdir[0];
		tan2[(i3 * 3) + 1] += tdir[1];
		tan2[(i3 * 3) + 2] += tdir[2];
	}

	for (long a = 0; a < vertexCount; a++){
		float n[3];
		n[0] = normal[(a * 4) + 0];
		n[1] = normal[(a * 4) + 1];
		n[2] = normal[(a * 4) + 2];

		float t[3];
		t[0] = tan1[(a * 3) + 0];
		t[1] = tan1[(a * 3) + 1];
		t[2] = tan1[(a * 3) + 2];

		// Gram-Schmidt orthogonalize
		//tangent => vec4
		float dotProductNT = dotProduct(n, t);
		float tangentVecTemp[3];

		tangentVecTemp[0] = (t[0] - n[0] * dotProductNT);
		tangentVecTemp[1] = (t[1] - n[1] * dotProductNT);
		tangentVecTemp[2] = (t[2] - n[2] * dotProductNT);
		normalize(tangentVecTemp);

		tangent[(a * 4) + 0] = tangentVecTemp[0];
		tangent[(a * 4) + 1] = tangentVecTemp[1];
		tangent[(a * 4) + 2] = tangentVecTemp[2];

		// Calculate handedness
		float t2[3];
		t2[0] = tan2[(a * 3) + 0];
		t2[1] = tan2[(a * 3) + 1];
		t2[2] = tan2[(a * 3) + 2];

		float crossVecTemp[3];
		crossProduct(n, t, crossVecTemp);

		tangent[(a * 4) + 3] = (dotProduct(crossVecTemp, t2) < 0.0F) ? -1.0F : 1.0F;
		//std::cout << "Tangent: (" << tangent[(a * 4) + 0] << ", " << tangent[(a * 4) + 1] << ", " << tangent[(a * 4) + 2] << ", " << tangent[(a * 4) + 3] << ")\n";
	}
}
