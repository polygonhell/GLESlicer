#include "mat4.h"
#include <string.h>
#include <math.h>

Mat4::Mat4(float *matIn)
{
	memcpy(m, matIn, sizeof(m));
}

Mat4::Mat4(Mat4 &matIn)
{
	memcpy(m, matIn.m, sizeof(m));
}


void Mat4::Rotate(Mat4 &mat, float a, float x,float y, float z)
{
	float angle=a;

	mat.m[0] = 1+(1-cos(angle))*(x*x-1);
	mat.m[1] = -z*sin(angle)+(1-cos(angle))*x*y;
	mat.m[2] = y*sin(angle)+(1-cos(angle))*x*z;
	mat.m[3] = 0;

	mat.m[4] = z*sin(angle)+(1-cos(angle))*x*y;
	mat.m[5] = 1+(1-cos(angle))*(y*y-1);
	mat.m[6] = -x*sin(angle)+(1-cos(angle))*y*z;
	mat.m[7] = 0;

	mat.m[8] = -y*sin(angle)+(1-cos(angle))*x*z;
	mat.m[9] = x*sin(angle)+(1-cos(angle))*y*z;
	mat.m[10] = 1+(1-cos(angle))*(z*z-1);
	mat.m[11] = 0;

	mat.m[12] = 0;
	mat.m[13] = 0;
	mat.m[14] = 0;
	mat.m[15] = 1;
}

void Mat4::Scale(Mat4 &mat, float s)
{
	memset(mat.m, 0, sizeof(float)*16);
	mat.m[0] = mat.m[5] = mat.m[10] = s;
	mat.m[15] = 1;
}

void Mat4::Identity(Mat4 &mat)
{
	memset(mat.m, 0, sizeof(float)*16);
	mat.m[0] = mat.m[5] = mat.m[10] = 1;
	mat.m[15] = 1;
}

void Mat4::Multiply(Mat4 &mOut, const Mat4 &mA, const Mat4 &mB)
{

	/* Perform calculation on a dummy matrix (mRet) */
	mOut.m[ 0] = mA.m[ 0]*mB.m[ 0] + mA.m[ 1]*mB.m[ 4] + mA.m[ 2]*mB.m[ 8] + mA.m[ 3]*mB.m[12];
	mOut.m[ 1] = mA.m[ 0]*mB.m[ 1] + mA.m[ 1]*mB.m[ 5] + mA.m[ 2]*mB.m[ 9] + mA.m[ 3]*mB.m[13];
	mOut.m[ 2] = mA.m[ 0]*mB.m[ 2] + mA.m[ 1]*mB.m[ 6] + mA.m[ 2]*mB.m[10] + mA.m[ 3]*mB.m[14];
	mOut.m[ 3] = mA.m[ 0]*mB.m[ 3] + mA.m[ 1]*mB.m[ 7] + mA.m[ 2]*mB.m[11] + mA.m[ 3]*mB.m[15];

	mOut.m[ 4] = mA.m[ 4]*mB.m[ 0] + mA.m[ 5]*mB.m[ 4] + mA.m[ 6]*mB.m[ 8] + mA.m[ 7]*mB.m[12];
	mOut.m[ 5] = mA.m[ 4]*mB.m[ 1] + mA.m[ 5]*mB.m[ 5] + mA.m[ 6]*mB.m[ 9] + mA.m[ 7]*mB.m[13];
	mOut.m[ 6] = mA.m[ 4]*mB.m[ 2] + mA.m[ 5]*mB.m[ 6] + mA.m[ 6]*mB.m[10] + mA.m[ 7]*mB.m[14];
	mOut.m[ 7] = mA.m[ 4]*mB.m[ 3] + mA.m[ 5]*mB.m[ 7] + mA.m[ 6]*mB.m[11] + mA.m[ 7]*mB.m[15];

	mOut.m[ 8] = mA.m[ 8]*mB.m[ 0] + mA.m[ 9]*mB.m[ 4] + mA.m[10]*mB.m[ 8] + mA.m[11]*mB.m[12];
	mOut.m[ 9] = mA.m[ 8]*mB.m[ 1] + mA.m[ 9]*mB.m[ 5] + mA.m[10]*mB.m[ 9] + mA.m[11]*mB.m[13];
	mOut.m[10] = mA.m[ 8]*mB.m[ 2] + mA.m[ 9]*mB.m[ 6] + mA.m[10]*mB.m[10] + mA.m[11]*mB.m[14];
	mOut.m[11] = mA.m[ 8]*mB.m[ 3] + mA.m[ 9]*mB.m[ 7] + mA.m[10]*mB.m[11] + mA.m[11]*mB.m[15];

	mOut.m[12] = mA.m[12]*mB.m[ 0] + mA.m[13]*mB.m[ 4] + mA.m[14]*mB.m[ 8] + mA.m[15]*mB.m[12];
	mOut.m[13] = mA.m[12]*mB.m[ 1] + mA.m[13]*mB.m[ 5] + mA.m[14]*mB.m[ 9] + mA.m[15]*mB.m[13];
	mOut.m[14] = mA.m[12]*mB.m[ 2] + mA.m[13]*mB.m[ 6] + mA.m[14]*mB.m[10] + mA.m[15]*mB.m[14];
	mOut.m[15] = mA.m[12]*mB.m[ 3] + mA.m[13]*mB.m[ 7] + mA.m[14]*mB.m[11] + mA.m[15]*mB.m[15];

}