#include <stdio.h>
#include <stdint.h>
#include <new>
#include "model.h"
#include <GLES2/gl2.h>


Model::Model() : triCount(0), tris(0) 
{

}

Model::~Model()
{
	delete[] tris;
}

bool Model::AllocTris(int triCount)
{
	this->triCount = triCount;
	this->tris = new Triangle[triCount];
	return this->tris != 0;
}

bool Model::CreateGLBuffers()
{
	// FIXME : actually do somwthing not stupid
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	Vec3 *verts = new Vec3[triCount *3];
	uint32_t *inds = new uint32_t[triCount *3];

	int index = 0;
	for (int i = 0; i < triCount; i++)
	{
		inds[index] = index;
		verts[index++] = tris[i].v1;
		inds[index] = index;
		verts[index++] = tris[i].v2;
		inds[index] = index;
		verts[index++] = tris[i].v3;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * triCount * 3, verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * triCount * 3, inds, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	int err = glGetError();
	printf("err = %d\n", err);

	delete[] verts;
	delete[] inds;
	return true;


}