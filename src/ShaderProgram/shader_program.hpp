#ifndef SHADER_PROGRAM_HPP_INCLUDED
#define SHADER_PROGRAM_HPP_INCLUDED

#include "GL/glew.h"
#include "GL/wglew.h"

class CShaderProgram
{
public:
    GLuint * UniformLocations;
    GLuint * AttribLocations;

    CShaderProgram();
    ~CShaderProgram() = default;

    operator GLuint ();

    bool Load(char * VertexShaderFileName, char * FragmentShaderFileName);
    void Destroy();

protected:
    GLuint VertexShader;
    GLuint FragmentShader;
    GLuint Program;

    GLuint LoadShader(char * FileName, GLenum Type);
    void SetDefaults();
};

#endif /* End of 'shader_program.hpp' file */