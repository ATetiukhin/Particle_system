#ifndef SHADER_PROGRAM_HPP_INCLUDED
#define Shader_Program_HPP_INCLUDED

#include "GL/glew.h" // http://glew.sourceforge.net/
#include "GL/wglew.h"

class CShaderProgram
{
protected:
    GLuint VertexShader, FragmentShader, Program;

public:
    GLuint *UniformLocations, *AttribLocations;

public:
    CShaderProgram();
    ~CShaderProgram();

    operator GLuint ();

    bool Load(char *VertexShaderFileName, char *FragmentShaderFileName);
    void Destroy();

protected:
    GLuint LoadShader(char *FileName, GLenum Type);
    void SetDefaults();
};

#endif /* End of 'shader_program.hpp' file */