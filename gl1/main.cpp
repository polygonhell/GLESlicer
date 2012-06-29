#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <signal.h>
#include <fcntl.h>
#include <termios.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "model.h"
#include "stl.h"


#define PI 3.14159265f
// Angle in Radians follows OpenGL convention
void Rotate(float *m, float a, float x,float y, float z)
{
	float angle=a;

	m[0] = 1+(1-cos(angle))*(x*x-1);
	m[1] = -z*sin(angle)+(1-cos(angle))*x*y;
	m[2] = y*sin(angle)+(1-cos(angle))*x*z;
	m[3] = 0;

	m[4] = z*sin(angle)+(1-cos(angle))*x*y;
	m[5] = 1+(1-cos(angle))*(y*y-1);
	m[6] = -x*sin(angle)+(1-cos(angle))*y*z;
	m[7] = 0;

	m[8] = -y*sin(angle)+(1-cos(angle))*x*z;
	m[9] = x*sin(angle)+(1-cos(angle))*y*z;
	m[10] = 1+(1-cos(angle))*(z*z-1);
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

void Scale(float *m, float s)
{
	memset(m, 0, sizeof(float)*16);
	m[0] = m[5] = m[10] = s;
	m[15] = 1;
}

void Multiply(float *mOut, const float *mA, const float *mB)
{

	/* Perform calculation on a dummy matrix (mRet) */
	mOut[ 0] = mA[ 0]*mB[ 0] + mA[ 1]*mB[ 4] + mA[ 2]*mB[ 8] + mA[ 3]*mB[12];
	mOut[ 1] = mA[ 0]*mB[ 1] + mA[ 1]*mB[ 5] + mA[ 2]*mB[ 9] + mA[ 3]*mB[13];
	mOut[ 2] = mA[ 0]*mB[ 2] + mA[ 1]*mB[ 6] + mA[ 2]*mB[10] + mA[ 3]*mB[14];
	mOut[ 3] = mA[ 0]*mB[ 3] + mA[ 1]*mB[ 7] + mA[ 2]*mB[11] + mA[ 3]*mB[15];

	mOut[ 4] = mA[ 4]*mB[ 0] + mA[ 5]*mB[ 4] + mA[ 6]*mB[ 8] + mA[ 7]*mB[12];
	mOut[ 5] = mA[ 4]*mB[ 1] + mA[ 5]*mB[ 5] + mA[ 6]*mB[ 9] + mA[ 7]*mB[13];
	mOut[ 6] = mA[ 4]*mB[ 2] + mA[ 5]*mB[ 6] + mA[ 6]*mB[10] + mA[ 7]*mB[14];
	mOut[ 7] = mA[ 4]*mB[ 3] + mA[ 5]*mB[ 7] + mA[ 6]*mB[11] + mA[ 7]*mB[15];

	mOut[ 8] = mA[ 8]*mB[ 0] + mA[ 9]*mB[ 4] + mA[10]*mB[ 8] + mA[11]*mB[12];
	mOut[ 9] = mA[ 8]*mB[ 1] + mA[ 9]*mB[ 5] + mA[10]*mB[ 9] + mA[11]*mB[13];
	mOut[10] = mA[ 8]*mB[ 2] + mA[ 9]*mB[ 6] + mA[10]*mB[10] + mA[11]*mB[14];
	mOut[11] = mA[ 8]*mB[ 3] + mA[ 9]*mB[ 7] + mA[10]*mB[11] + mA[11]*mB[15];

	mOut[12] = mA[12]*mB[ 0] + mA[13]*mB[ 4] + mA[14]*mB[ 8] + mA[15]*mB[12];
	mOut[13] = mA[12]*mB[ 1] + mA[13]*mB[ 5] + mA[14]*mB[ 9] + mA[15]*mB[13];
	mOut[14] = mA[12]*mB[ 2] + mA[13]*mB[ 6] + mA[14]*mB[10] + mA[15]*mB[14];
	mOut[15] = mA[12]*mB[ 3] + mA[13]*mB[ 7] + mA[14]*mB[11] + mA[15]*mB[15];

}




// Index to bind the attributes to vertex shaders
#define VERTEX_ARRAY	0


EGLint configAttr[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
						EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
						EGL_NONE
						};
EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

EGLDisplay eglDisplay = 0;
EGLSurface eglSurface = 0;

bool CreateContext()
{
	EGLConfig eglConfig	= 0;
	EGLConfig eglContext = 0;

	// Get the default display
	eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);
	EGLint majorVersion, minorVersion;
	if (!eglInitialize(eglDisplay, &majorVersion, &minorVersion))
	{
		printf("Error couldn't init egl\n");
		return false;
	}

	printf("Init EGL version %d.%d\n", majorVersion, minorVersion);

	eglBindAPI(EGL_OPENGL_ES_API);
	if (eglGetError() != EGL_SUCCESS)
	{
		printf("API Bind failed\n");
		return false;
	}


	int iConfigs;
	if (!eglChooseConfig(eglDisplay, configAttr, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		printf("Error: eglChooseConfig() failed.\n");
		return false;
	}

	printf("Selected Config %d\n", eglConfig);


	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType) NULL, NULL);
	if (eglGetError() != EGL_SUCCESS)
	{
		printf("Create Surface failed\n");
		return false;
	}

	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, contextAttribs);
	if (eglGetError() != EGL_SUCCESS)
	{
		printf("Create Context failed\n");
		return false;
	}

	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	return true;
}


GLint CompileShader(GLuint type, const char *source)
{
	GLint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	// Check if compilation succeeded
	GLint shaderCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);

	if (!shaderCompiled)
	{
		// An error happened, first retrieve the length of the log message
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		// Allocate enough space for the message and retrieve it
		char* pszInfoLog = (char*)malloc(i32InfoLogLength);
        glGetShaderInfoLog(shader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		// Displays the error
		printf("Failed to compile fragment shader: %s\n", pszInfoLog);
		free(pszInfoLog);
		return -1;
	}
	return shader;
}



const char* fragShaderCode = "\
	void main (void)\
	{\
		gl_FragColor = vec4(0.0, 0.0, 1.0 ,1.0);\
	}";
const char* vertShaderCode = "\
	attribute highp vec4	myVertex;\
	uniform mediump mat4	myPMVMatrix;\
	void main(void)\
	{\
		gl_Position = myPMVMatrix * myVertex;\
	}";

GLint fragShader, vertShader, program;

float identityMat[] =
{
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f
};

GLfloat verts[] = {	-40.0f,-40.0f,0.0f, // Position
					40.0f ,-40.0f,0.0f,
					0.0f ,40.0f ,0.0f};


int main(int argc, char *argv[])
{
	printf("Trivial EGL test\n");


	Model model;
	bool val = stl::convert("grotux.stl", &model);
	if (!val)
		printf("ERROR reading file\n");

	printf("Min = %5.2f, %5.2f, %5.2f\n", model.bbMin.x, model.bbMin.y, model.bbMin.z);
	printf("Max = %5.2f, %5.2f, %5.2f\n", model.bbMax.x, model.bbMax.y, model.bbMax.z);



	signal( SIGTTIN, SIG_IGN );
	signal( SIGTTOU, SIG_IGN );
	
	// struct termios termio, termio_orig;
	// int devfd=open("/dev/tty", O_RDWR|O_NDELAY);
	// printf("devfd = %d\n", devfd);
	// tcgetattr(devfd,&termio_orig);
	// tcgetattr(devfd,&termio);
	// cfmakeraw(&termio);
	// termio.c_oflag |= OPOST | ONLCR; // Turn back on cr-lf expansion on output
	// termio.c_cc[VMIN]=1;
	// termio.c_cc[VTIME]=0;
	// tcsetattr(devfd,TCSANOW,&termio);

	// Get rid of blinking cursor
	FILE *tty = 0;
	tty = fopen("/dev/tty0", "w");
	if (tty != 0)
	{
		const char txt[] = { 27 /* the ESCAPE ASCII character */
						   , '['
						   , '?'
						   , '2'
						   , '5'
						   , 'l'
						   , 0
						   };

		fprintf(tty, "%s", txt);
		fclose(tty);
	}




	// Create a display surface
	if (!CreateContext())
		goto cleanup;

	fragShader = CompileShader(GL_FRAGMENT_SHADER, fragShaderCode);
	if (fragShader < 0)
		goto cleanup;
	vertShader = CompileShader(GL_VERTEX_SHADER, vertShaderCode);
	if (vertShader < 0)
		goto cleanup;

	program = glCreateProgram();
	glAttachShader(program, fragShader);
	glAttachShader(program, vertShader);
	glBindAttribLocation(program, VERTEX_ARRAY, "myVertex");
	glLinkProgram(program);
	GLint bLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &bLinked);
	if (!bLinked)
	{
		int ui32InfoLogLength, ui32CharsWritten;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &ui32InfoLogLength);
		char* pszInfoLog = (char *)malloc(ui32InfoLogLength);
		glGetProgramInfoLog(program, ui32InfoLogLength, &ui32CharsWritten, pszInfoLog);
		printf("Failed to link program: %s\n", pszInfoLog);
		free(pszInfoLog);
		goto cleanup;
	}



	glUseProgram(program);

	// Create a vertex buffer
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	model.CreateGLBuffers();



	{
		int c = 1;
		float angle = 0;
		float m[16], m1[16], m2[16];
		
		int pmvMatrixLoc = glGetUniformLocation(program, "myPMVMatrix");


		while(true)
		{

			if (c)
				glClearColor(1,1,0,1);
			else
				glClearColor(0,1,1,1);

			c ^= 1;

			glClear(GL_COLOR_BUFFER_BIT);

			Scale(m1, 0.01f);
			Rotate(m2, angle, 0.7071f, 0.7071f, 0);
			angle += 0.03f;
			if (angle > 2*PI)
				angle -= 2*PI;
			Multiply(m, m1, m2);

			glUniformMatrix4fv(pmvMatrixLoc, 1, GL_FALSE, m);
			glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
			glEnableVertexAttribArray(VERTEX_ARRAY);
			glVertexAttribPointer(VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//glDrawArrays(GL_TRIANGLES, 0, 4002);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ibo);
			glDrawElements(GL_TRIANGLES, model.triCount * 3, GL_UNSIGNED_INT, 0);

			eglSwapBuffers(eglDisplay, eglSurface);
		}

	}

cleanup:
	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(eglDisplay);
	return 0;
}