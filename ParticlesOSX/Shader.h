//
//  Shader.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/19/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Shader__
#define __ParticlesOSX__Shader__

#import "MyGLUtilities.h"

#include <string>

class Shader {
public:
virtual ~Shader();

    bool initialize(std::string source);
    
    GLuint getShaderID() { return m_shader; }

protected:
    bool m_initialized;
    GLuint m_shader;
    
    Shader() : m_initialized(false) { }
    
    virtual GLint _getShaderType() = 0;
};

class VertexShader : public Shader {
public:
    VertexShader() : Shader() {}
protected:
    virtual GLint _getShaderType() { return GL_VERTEX_SHADER; };
};

class FragmentShader : public Shader {
public:
    FragmentShader() : Shader() {}
protected:
    virtual GLint _getShaderType() { return GL_FRAGMENT_SHADER; };
};

#endif /* defined(__ParticlesOSX__Shader__) */
