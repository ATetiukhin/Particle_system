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
    ~COpenGLRenderer();

    bool Init();
    void Render(float FrameTime);
    void Resize(int Width, int Height);
    void Destroy();

    void AddDrop(float x, float y, float DropRadius);
    void AddDropByMouseClick(int x, int y);

protected:
    int Width, Height;
    mat3x3 NormalMatrix;
    mat4x4 ModelMatrix, ViewMatrix, ViewMatrixInverse, ProjectionMatrix, ProjectionBiasMatrixInverse;

    CTexture PoolSkyCubeMap;
    GLuint WaterHeightMaps[2], WHMID, WaterNormalMap, PhotonsTexture, PhotonsTempCubeMaps[2], PhotonsCubeMap, PhotonsVBO, PoolSkyVBO, WaterVBO, FBO;
    CShaderProgram WaterAddDropProgram, WaterHeightMapProgram, WaterNormalMapProgram, PhotonProgram, CubeMapHBlurProgram, CubeMapVBlurProgram, PoolSkyProgram, WaterProgram;
    int PhotonsCount, QuadsVerticesCount;

};

#endif /* End of 'opengl_renderer.hpp' file */