#pragma once
#include "iqm.h"
#include "geom.h"

struct u8vec4{uint8_t v[4];};

class iqmesh
{
//Variables
public:
	iqmheader data_header;
	uint8_t * data_buff;

    const char** texts;

    ///metadata buffers
	iqmmesh * submeshes;
	iqmvertexarray * vertexarrays; //IQM vertex array info
	iqmtriangle* triangles; //IQM triangle info(for indices)
	iqmjoint* joints; //IQM joint info
	iqmpose* poses; //IQM pose info
	iqmanim* anims; //IQM animation info
	iqmbounds* bounds; //IQM the bounding box

    /// data buffers
	vec3* positions;
	vec2* texcoords;
	vec3* normals;
	vec4* tangents;
	u8vec4 * bindexes;
	u8vec4 * bweights;
	vec3 * colors;

    ///animation
	mat3x4 * frames;
	mat3x4 * base_frame;
	mat3x4 * inverse_base_frame;
	mat3x4 * current_frame;
	uint16_t * frame_data;

	///GL buffers
	GLuint vaoid;
	GLuint buffers[IQM_BUFFER_COUNT];

private:
protected:

//Methods
public:
	iqmesh();
	virtual ~iqmesh();
	bool generate();
	void draw(bool whole);

	void set_frame(float frame);

private:
protected:
};
