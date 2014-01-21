//
//  ShaderProgram.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/19/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "ShaderProgram.h"

#include <iostream>

using namespace std;

ShaderProgram::ShaderProgram() {
    m_initialized = false;
}

bool ShaderProgram::initialize(VertexShader vertex_shader, FragmentShader fragment_shader)
{
    m_shader_program = glCreateProgram();

    /* Attach our shaders to our program */
    glAttachShader(m_shader_program, vertex_shader.getShaderID());
    glAttachShader(m_shader_program, fragment_shader.getShaderID());
 
    // TODO
//    glBindAttribLocation(m_shader_program, 0, "color");
//    glBindAttribLocation(m_shader_program, 1, "radius");
    
    glLinkProgram(m_shader_program);

    GLint compiled;
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, (int *)&compiled);
    if(!compiled)
    {
        int maxLength;

        /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
        glGetProgramiv(m_shader_program, GL_INFO_LOG_LENGTH, &maxLength);
        
        /* The maxLength includes the NULL character */
        char *infoLog = new char[maxLength];
        
        /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
        glGetProgramInfoLog(m_shader_program, maxLength, &maxLength, infoLog);
        
        /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
        /* In this simple program, we'll just leave */
        cerr << "Shader Compile error:" << std::endl << infoLog << std::endl;
        
        delete[] infoLog;
    } else {
        m_initialized = true;
    }
    
    return m_initialized;
}

ShaderProgram::~ShaderProgram() {
//    glDetachShader(shaderprogram, vertex_shader.getShaderID());
//    glDetachShader(shaderprogram, fragment_shader.getShaderID());

    glDeleteProgram(m_shader_program);
}

void ShaderProgram::useProgram() {
    glUseProgram(m_shader_program);
}

void ShaderProgram::unuseProgram() {
    glUseProgram(0);

    // TODO
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
