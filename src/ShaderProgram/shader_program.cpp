#include "shader_program.hpp"

#include "CString.hpp"

extern CString ModuleDirectory;
extern CString ErrorLog;

CShaderProgram::CShaderProgram()
{
    SetDefaults();
}

CShaderProgram::operator GLuint ()
{
    return Program;
}

bool CShaderProgram::Load(char *VertexShaderFileName, char *FragmentShaderFileName)
{
    bool Error = false;

    Destroy();

    Error |= ((VertexShader = LoadShader(VertexShaderFileName, GL_VERTEX_SHADER)) == 0);
    Error |= ((FragmentShader = LoadShader(FragmentShaderFileName, GL_FRAGMENT_SHADER)) == 0);

    if (Error)
    {
        Destroy();
        return false;
    }

    Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);

    int LinkStatus;
    glGetProgramiv(Program, GL_LINK_STATUS, &LinkStatus);

    if (LinkStatus == GL_FALSE)
    {
        ErrorLog.Append("Error linking program (%s, %s)!\r\n", VertexShaderFileName, FragmentShaderFileName);

        int InfoLogLength = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);

        if (InfoLogLength > 0)
        {
            char *InfoLog = new char[InfoLogLength];
            int CharsWritten = 0;
            glGetProgramInfoLog(Program, InfoLogLength, &CharsWritten, InfoLog);
            ErrorLog.Append(InfoLog);
            delete[] InfoLog;
        }

        Destroy();

        return false;
    }

    return true;
}

void CShaderProgram::Destroy()
{
    glDetachShader(Program, VertexShader);
    glDetachShader(Program, FragmentShader);

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    glDeleteProgram(Program);

    delete[] UniformLocations;
    delete[] AttribLocations;

    SetDefaults();
}

GLuint CShaderProgram::LoadShader(char *FileName, GLenum Type)
{
    CString DirectoryFileName = ModuleDirectory + FileName;

    FILE *File;

    if (fopen_s(&File, DirectoryFileName, "rb") != 0)
    {
        ErrorLog.Append("Error loading file " + DirectoryFileName + "!\r\n");
        return 0;
    }

    fseek(File, 0, SEEK_END);
    long Size = ftell(File);
    fseek(File, 0, SEEK_SET);
    char *Source = new char[Size + 1];
    fread(Source, 1, Size, File);
    fclose(File);
    Source[Size] = 0;

    GLuint Shader = glCreateShader(Type);

    glShaderSource(Shader, 1, (const char**)&Source, NULL);
    delete[] Source;
    glCompileShader(Shader);

    int CompileStatus;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);

    if (CompileStatus == GL_FALSE)
    {
        ErrorLog.Append("Error compiling shader %s!\r\n", FileName);

        int InfoLogLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);

        if (InfoLogLength > 0)
        {
            char *InfoLog = new char[InfoLogLength];
            int CharsWritten = 0;
            glGetShaderInfoLog(Shader, InfoLogLength, &CharsWritten, InfoLog);
            ErrorLog.Append(InfoLog);
            delete[] InfoLog;
        }

        glDeleteShader(Shader);

        return 0;
    }

    return Shader;
}

void CShaderProgram::SetDefaults()
{
    VertexShader = 0;
    FragmentShader = 0;

    Program = 0;

    UniformLocations = nullptr;
    AttribLocations = nullptr;
}
