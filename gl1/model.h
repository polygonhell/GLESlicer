
#define min(x,y) (x)<(y)?(x):(y) 
#define max(x,y) (x)>(y)?(x):(y) 

struct Vec3
{
	union {
		float xyz[3];
		struct
		{
			float x;
			float y;
			float z;
		};
	};


	Vec3 &operator = (float v[3])
	{
		x = v[0];
		y = v[1];
		z = v[2];
		return *this;
	}

	Vec3 &operator = (Vec3 &in)
	{
		return *this = in.xyz;
	}

	static Vec3 &Min(Vec3 *out, Vec3 &in1, Vec3 &in2)
	{
		out->x = min(in1.x, in2.x);
		out->y = min(in1.y, in2.y);
		out->z = min(in1.z, in2.z);
		return *out;
	}

	static Vec3 &Max(Vec3 *out, Vec3 &in1, Vec3 &in2)
	{
		out->x = max(in1.x, in2.x);
		out->y = max(in1.y, in2.y);
		out->z = max(in1.z, in2.z);
		return *out;
	}

};

struct Triangle
{
	Vec3 normal;

	Vec3 v1;
	Vec3 v2;
	Vec3 v3;
};

class Model
{

public:
	Model();
	~Model();

	bool AllocTris(int triCount);
	bool CreateGLBuffers();

	uint32_t vbo;
	uint32_t ibo;
	uint32_t triCount;


	Triangle *tris;
	const char *filename;

	// Bounding box
	Vec3 bbMin;
	Vec3 bbMax;

};