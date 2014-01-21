//
//  Shader.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/19/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Shader.h"

#include <iostream>
using namespace std;

bool Shader::initialize(std::string source) {
    
    m_shader = glCreateShader(_getShaderType());

    const char *shader_source = source.c_str();
    glShaderSource(m_shader, 1, (const GLchar**)& shader_source, 0);
    
    glCompileShader(m_shader);
    
    GLint compiled;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        int maxLength;
        
        glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &maxLength);
        
        /* The maxLength includes the NULL character */
        char *infoLog = new char[maxLength];
        
        glGetShaderInfoLog(m_shader, maxLength, &maxLength, infoLog);
        
        /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
        /* In this simple program, we'll just leave */
        
        cerr << "Shader Compile error:" << std::endl << infoLog << std::endl;
        
        delete[] infoLog;
    } else {
        m_initialized = true;
    }
    
    return m_initialized;
}

Shader::~Shader() {
    glDeleteShader(m_shader);
}