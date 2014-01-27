//
//  MyGLUtilities.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/24/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef ParticlesOSX_MyGLUtilities_h
#define ParticlesOSX_MyGLUtilities_h

#define GL3_PROTOTYPES
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>

#include <iostream>

#define GetGLError()									\
{														\
    GLenum err = glGetError();							\
    while (err != GL_NO_ERROR) {						\
        std::cerr << "GLError " << GetGLErrorString(err)\
            << " set in File:" << __FILE__ << " Line: " \
            << __LINE__ << std::endl;                   \
        err = glGetError();								\
        }												\
}

static inline const char * GetGLErrorString(GLenum error)
{
	const char *str;
	switch( error )
	{
		case GL_NO_ERROR:
			str = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			str = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			str = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			str = "GL_INVALID_OPERATION";
			break;
#if defined __gl_h_ || defined __gl3_h_
		case GL_OUT_OF_MEMORY:
			str = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			str = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
#endif
#if defined __gl_h_
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "GL_TABLE_TOO_LARGE";
			break;
#endif
		default:
			str = "(ERROR: Unknown Error Enum)";
			break;
	}
	return str;
}



#endif
