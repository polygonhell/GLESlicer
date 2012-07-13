#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>

#ifndef WIN32
#include <unistd.h>
#include <termios.h>
#include <tslib.h>
#endif

#ifdef WIN32
#include "Windows\window.h"
#endif

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "model.h"
#include "stl.h"
#include "mat4.h"


#define TOUCHSCREEN_INPUT "/dev/input/event0"

#define PI 3.14159265f



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
	eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//	eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);
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

	printf("Selected Config %d\n", (int)eglConfig);

	EGLNativeWindowType hWind = (EGLNativeWindowType)NULL;
#ifdef WIN32
	hWind = (EGLNativeWindowType)Windows::Init();
#endif


	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType) hWind, NULL);
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


#ifndef WIN32
	signal( SIGTTIN, SIG_IGN );
	signal( SIGTTOU, SIG_IGN );

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


	struct tsdev *ts = ts_open(TOUCHSCREEN_INPUT, 1);
	printf("Openned Touch screen %08x\n", (int)ts);
	int err = ts_config(ts);
	printf("Config returned %d\n", err);

#endif

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
	model.CreateGLBuffers();



	{
		int c = 1;
		float angle = 0;
		Mat4 m, m1, m2;
		
		int pmvMatrixLoc = glGetUniformLocation(program, "myPMVMatrix");

#ifndef WIN32
		struct ts_sample samples[1];
		memset(samples, 0, sizeof(samples));
#endif

		while(true)
		{

#ifndef WIN32
			err = ts_read(ts, samples, 1);
			if (err < 0)
				printf("err = %08x\n", err);
			printf("X %d -- Y %d -- P %d\n", samples->x, samples->y, samples->pressure);
			if (samples->x > 400 && samples->pressure >200)
				break;
#endif

			if (c)
				glClearColor(1,1,0,1);
			else
				glClearColor(0,1,1,1);

			c ^= 1;

			glClear(GL_COLOR_BUFFER_BIT);

			Mat4::Scale(m1, 0.01f);
			Mat4::Rotate(m2, angle, 0.7071f, 0.7071f, 0);
			angle += 0.03f;
			if (angle > 2*PI)
				angle -= 2*PI;
			Mat4::Multiply(m, m1, m2);

			glUniformMatrix4fv(pmvMatrixLoc, 1, GL_FALSE, m.m);
			glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
			glEnableVertexAttribArray(VERTEX_ARRAY);
			glVertexAttribPointer(VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//glDrawArrays(GL_TRIANGLES, 0, model.triCount *3);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ibo);
			glDrawElements(GL_TRIANGLES, model.triCount * 3, GL_UNSIGNED_SHORT, 0);

			eglSwapBuffers(eglDisplay, eglSurface);

#ifdef WIN32
			if (!Windows::Update())
				break;
			Sleep(10);
#endif
		}

	}

cleanup:
	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(eglDisplay);
	return 0;
}