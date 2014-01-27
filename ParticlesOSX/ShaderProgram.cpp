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

bool ShaderProgram::initialize(string vertex_shader_source, string fragment_shader_source)
{
    if (m_initialized) {
        return true;
    }

    bool ok;
    
    cout << "ShaderProgram initialize Vertex Shader " << endl;;
    ok = m_vertex_shader.initialize(vertex_shader_source);
    if (!ok) {
        cerr << "VERTEX SHADER ERROR" << endl;
    }
    GetGLError();
    
    cout << "ShaderProgram initialize Fragment Shader " << endl;;
    ok = m_fragment_shader.initialize(fragment_shader_source);
    if (!ok) {
        cerr << "FRAGMENT SHADER ERROR" << endl;
    }
    GetGLError();
    
    cout << "Shader Program glCreateProgram" << endl;;

    m_shader_program = glCreateProgram();

    /* Attach our shaders to our program */
    glAttachShader(m_shader_program, m_vertex_shader.getShaderID());
    glAttachShader(m_shader_program, m_fragment_shader.getShaderID());
 
    // TODO
//    glBindAttribLocation(m_shader_program, 1, "position");
//    glBindAttribLocation(m_shader_program, 2, "colorIn");
    
    glLinkProgram(m_shader_program);

    GLint linked;
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, (int *)&linked);
    if(!linked)
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
        cerr << "Shader Linker error:" << std::endl << infoLog << std::endl;
        
        delete[] infoLog;
    } else {
        m_initialized = true;
    }
    
    return m_initialized;
}

ShaderProgram::~ShaderProgram() {
    if (m_initialized) {
        //    glDetachShader(shaderprogram, m_vertex_shader.getShaderID());
        //    glDetachShader(shaderprogram, m_fragment_shader.getShaderID());
        
        glDeleteProgram(m_shader_program);
    }
}

void ShaderProgram::useProgram() {
    // if m_initialized ...
    glUseProgram(m_shader_program);
}

void ShaderProgram::unuseProgram() {
    // if m_initialized ...
    glUseProgram(0);

//    // TODO
//    glDisableVertexAttribArray(0);
//    glDisableVertexAttribArray(1);
}
