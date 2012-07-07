
struct Mat4
{
	union {
		float m[16];
	};


	Mat4(){}
	// Copy Constructors
	Mat4(float *matIn);
	Mat4(Mat4 &matIn);


	// Basic operations
	// Angle in Radians follows OpenGL convention
	static void Rotate(Mat4 &m, float a, float x,float y, float z);
	static void Scale(Mat4 &mat, float s);
	static void Multiply(Mat4 &mOut, const Mat4 &mA, const Mat4 &mB);


};