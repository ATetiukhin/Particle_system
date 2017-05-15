#ifndef OPENGL_RENDERER_HPP_INCLUDED
#define OPENGL_RENDERER_HPP_INCLUDED

#define WMR 128 // water mesh resolution
#define WHMR 128 // water height map resolution
#define WNMR 256 // water normal map resolution
#define PCMR 512 // photon cube map resolution

#include "cstring.hpp"
#include "mathematics.hpp"
#include "shader_program.hpp"
#include "texture.hpp"

class COpenGLRenderer
{
public:
    bool WireFrame;
    bool Pause;
    float DropRadius;

    CString Text;

    COpenGLRenderer();
    ~COpenGLRenderer() = default;

    bool Init();
    void Render(float FrameTime);
    void Resize(int Width, int Height);
    void Destroy();

    void AddDrop(float x, float y, float DropRadius);
    void AddDropByMouseClick(int x, int y);

protected:
    int Width;
    int Height;
 
    mat3x3 NormalMatrix;

    mat4x4 ModelMatrix;
    mat4x4 ViewMatrix;
    mat4x4 ViewMatrixInverse;
    mat4x4 ProjectionMatrix;
    mat4x4 ProjectionBiasMatrixInverse;

    CTexture PoolSkyCubeMap;
 
    GLuint WaterHeightMaps[2];
    GLuint WHMID;
    GLuint WaterNormalMap;
    GLuint PhotonsTexture;
    GLuint PhotonsTempCubeMaps[2];
    GLuint PhotonsCubeMap;
    GLuint PhotonsVBO;
    GLuint PoolSkyVBO;
    GLuint WaterVBO;
    GLuint FBO;
 
    CShaderProgram WaterAddDropProgram;
    CShaderProgram WaterHeightMapProgram;
    CShaderProgram WaterNormalMapProgram;
    CShaderProgram PhotonProgram;
    CShaderProgram CubeMapHBlurProgram;
    CShaderProgram CubeMapVBlurProgram;
    CShaderProgram PoolSkyProgram;
    CShaderProgram WaterProgram;
 
    int PhotonsCount;
    int QuadsVerticesCount;

};

#endif /* End of 'opengl_renderer.hpp' file */