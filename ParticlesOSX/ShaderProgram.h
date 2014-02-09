//
//  ShaderProgram.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/19/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__ShaderProgram__
#define __ParticlesOSX__ShaderProgram__

#include "Shader.h"

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    bool initialize(std::string vertex_shader_source, std::string fragment_shader_source);
    
    void useProgram();
    void unuseProgram();
    
    bool setUniform2f(const char *uniformName, int v1, int v2);
    bool setUniform4fv(const char *uniformName, const float* values);
    
    GLuint id() { return m_shader_program; }
    
private:
    VertexShader m_vertex_shader;
    FragmentShader m_fragment_shader;
    
    bool m_initialized;
    GLuint m_shader_program;
};

typedef std::shared_ptr<ShaderProgram> ShaderProgramPtr;


#endif /* defined(__ParticlesOSX__ShaderProgram__) */
