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

    bool initialize(VertexShader vertex_shader, FragmentShader fragment_shader);
    
    void useProgram();
    void unuseProgram();
    
private:
    bool m_initialized;
    GLuint m_shader_program;
};

#endif /* defined(__ParticlesOSX__ShaderProgram__) */
