#include <cstdlib>
#include <GL/glew.h> // http://glew.sourceforge.net/
#include <GL/wglew.h>

#include "opengl_renderer.hpp"

#include "buffer.hpp"
#include "camera.hpp"
#include "cstring.hpp"
#include "opengl_view.hpp"
#include "mathematics.hpp"


extern CCamera Camera;
extern CString ErrorLog;
extern int gl_max_texture_max_anisotropy_ext;


COpenGLRenderer::COpenGLRenderer()
{
    WHMID = 0;

    WireFrame = false;
    Pause = false;

    DropRadius = 4.0f / 128.0f;

    Camera.SetViewMatrixPointer(&ViewMatrix, &ViewMatrixInverse);
}

COpenGLRenderer::~COpenGLRenderer()
{
}

bool COpenGLRenderer::Init()
{
    bool Error = false;

    if (!GLEW_ARB_texture_non_power_of_two)
    {
        ErrorLog.Append("GL_ARB_texture_non_power_of_two not supported!\r\n");
        Error = true;
    }

    if (!GLEW_ARB_texture_float)
    {
        ErrorLog.Append("GL_ARB_texture_float not supported!\r\n");
        Error = true;
    }

    if (!GLEW_EXT_framebuffer_object)
    {
        ErrorLog.Append("GL_EXT_framebuffer_object not supported!\r\n");
        Error = true;
    }

    char *PoolSkyCubeMapFileNames[] = { "pool\\right.jpg", "pool\\left.jpg", "pool\\bottom.jpg", "pool\\top.jpg", "pool\\front.jpg", "pool\\back.jpg" };

    Error |= !PoolSkyCubeMap.LoadTextureCubeMap(PoolSkyCubeMapFileNames);

    Error |= !WaterAddDropProgram.Load("wateradddrop.vs", "wateradddrop.fs");
    Error |= !WaterHeightMapProgram.Load("waterheightmap.vs", "waterheightmap.fs");
    Error |= !WaterNormalMapProgram.Load("waternormalmap.vs", "waternormalmap.fs");
    Error |= !PhotonProgram.Load("photon.vs", "photon.fs");
    Error |= !CubeMapHBlurProgram.Load("cube_map_blur.vs", "cube_map_hblur.fs");
    Error |= !CubeMapVBlurProgram.Load("cube_map_blur.vs", "cube_map_vblur.fs");
    Error |= !PoolSkyProgram.Load("poolsky.vs", "poolsky.fs");
    Error |= !WaterProgram.Load("water.vs", "water.fs");

    if (Error)
    {
        return false;
    }

    // ------------------------------------------------------------------------------------------------------------------------

    vec3 LightPosition = vec3(0.0f, 1.0f, 0.0f);

    vec3 CubeMapNormals[6] = {
        vec3(-1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 0.0f, -1.0f),
        vec3(0.0f, 0.0f, 1.0f),
    };

    mat4x4 BiasScaleMatrix = scale(1.0f / 2.0f, 1.0f / 3.0f, 1.0f) * BiasMatrix;

    mat4x4 PhotonsWorldToTextureMatrices[6] = {
        translate(0.0f / 2.0f, 0.0f / 3.0f, 1.0f) * BiasScaleMatrix * mat4x4(vec4(0.0f, 0.0f, -1.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f)),
        translate(1.0f / 2.0f, 0.0f / 3.0f, 1.0f) * BiasScaleMatrix * mat4x4(vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(-1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f)),
        translate(0.0f / 2.0f, 1.0f / 3.0f, 1.0f) * BiasScaleMatrix * mat4x4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, -1.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f)),
        translate(1.0f / 2.0f, 1.0f / 3.0f, 1.0f) * BiasScaleMatrix * mat4x4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(0.0f, -1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f)),
        translate(0.0f / 2.0f, 2.0f / 3.0f, 1.0f) * BiasScaleMatrix * mat4x4(vec4(-1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, -1.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f)),
        translate(1.0f / 2.0f, 2.0f / 3.0f, 1.0f) * BiasScaleMatrix * mat4x4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f))
    };

    // ------------------------------------------------------------------------------------------------------------------------

    glUseProgram(WaterHeightMapProgram);
    glUniform1f(glGetUniformLocation(WaterHeightMapProgram, "ODWHMR"), 1.0f / (float)WHMR);
    glUseProgram(0);

    glUseProgram(WaterNormalMapProgram);
    glUniform1f(glGetUniformLocation(WaterNormalMapProgram, "ODWNMR"), 1.0f / (float)WNMR);
    glUniform1f(glGetUniformLocation(WaterNormalMapProgram, "WMSDWNMRM2"), 2.0f / (float)WNMR * 2.0f);
    glUseProgram(0);

    glUseProgram(PhotonProgram);
    glUniform1i(glGetUniformLocation(PhotonProgram, "WaterHeightMap"), 0);
    glUniform1i(glGetUniformLocation(PhotonProgram, "WaterNormalMap"), 1);
    glUniform1i(glGetUniformLocation(PhotonProgram, "PhotonsTexture"), 2);
    glUniform3fv(glGetUniformLocation(PhotonProgram, "LightPosition"), 1, &LightPosition);
    glUniform3fv(glGetUniformLocation(PhotonProgram, "CubeMapNormals"), 6, (float*)CubeMapNormals);
    glUniformMatrix4fv(glGetUniformLocation(PhotonProgram, "PhotonsWorldToTextureMatrices"), 6, GL_FALSE, (float*)PhotonsWorldToTextureMatrices);
    glUseProgram(0);

    glUseProgram(PoolSkyProgram);
    glUniform1i(glGetUniformLocation(PoolSkyProgram, "PoolSkyCubeMap"), 0);
    glUniform1i(glGetUniformLocation(PoolSkyProgram, "PhotonsCubeMap"), 1);
    glUseProgram(0);

    glUseProgram(WaterProgram);
    glUniform1i(glGetUniformLocation(WaterProgram, "WaterHeightMap"), 0);
    glUniform1i(glGetUniformLocation(WaterProgram, "WaterNormalMap"), 1);
    glUniform1i(glGetUniformLocation(WaterProgram, "PoolSkyCubeMap"), 2);
    glUniform1i(glGetUniformLocation(WaterProgram, "PhotonsCubeMap"), 3);
    glUniform1f(glGetUniformLocation(WaterProgram, "ODWMS"), 1.0f / 2.0f);
    glUniform3fv(glGetUniformLocation(WaterProgram, "LightPosition"), 1, &LightPosition);
    glUniform3fv(glGetUniformLocation(WaterProgram, "CubeMapNormals"), 6, (float*)CubeMapNormals);
    glUseProgram(0);

    // ------------------------------------------------------------------------------------------------------------------------

    WaterAddDropProgram.UniformLocations = new GLuint[2];
    WaterAddDropProgram.UniformLocations[0] = glGetUniformLocation(WaterAddDropProgram, "DropRadius");
    WaterAddDropProgram.UniformLocations[1] = glGetUniformLocation(WaterAddDropProgram, "Position");

    CubeMapHBlurProgram.UniformLocations = new GLuint[1];
    CubeMapHBlurProgram.UniformLocations[0] = glGetUniformLocation(CubeMapHBlurProgram, "Offset");

    CubeMapVBlurProgram.UniformLocations = new GLuint[1];
    CubeMapVBlurProgram.UniformLocations[0] = glGetUniformLocation(CubeMapVBlurProgram, "Offset");

    WaterProgram.UniformLocations = new GLuint[1];
    WaterProgram.UniformLocations[0] = glGetUniformLocation(WaterProgram, "CameraPosition");

    // ------------------------------------------------------------------------------------------------------------------------

    glGenTextures(2, WaterHeightMaps);

    vec4 *Heights = new vec4[WHMR * WHMR];

    for (int i = 0; i < WHMR * WHMR; i++)
    {
        Heights[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WHMR, WHMR, 0, GL_RGBA, GL_FLOAT, Heights);
        glGenerateMipmapEXT(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    delete[] Heights;

    // ------------------------------------------------------------------------------------------------------------------------

    glGenTextures(1, &WaterNormalMap);

    vec4 *Normals = new vec4[WNMR * WNMR];

    for (int i = 0; i < WNMR * WNMR; i++)
    {
        Normals[i] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    glBindTexture(GL_TEXTURE_2D, WaterNormalMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WNMR, WNMR, 0, GL_RGBA, GL_FLOAT, Normals);
    glGenerateMipmapEXT(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] Normals;

    // ------------------------------------------------------------------------------------------------------------------------

    glGenTextures(1, &PhotonsTexture);

    vec4 *Colors = new vec4[PCMR * 2 * PCMR * 3];

    for (int i = 0; i < PCMR * 2 * PCMR * 3; i++)
    {
        Colors[i] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    glBindTexture(GL_TEXTURE_2D, PhotonsTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, PCMR * 2, PCMR * 3, 0, GL_RGBA, GL_FLOAT, Colors);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] Colors;

    // ------------------------------------------------------------------------------------------------------------------------

    glGenTextures(2, PhotonsTempCubeMaps);

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsTempCubeMaps[i]);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, PCMR, PCMR, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // ------------------------------------------------------------------------------------------------------------------------

    glGenTextures(1, &PhotonsCubeMap);

    Colors = new vec4[PCMR * PCMR];

    for (int i = 0; i < PCMR * PCMR; i++)
    {
        Colors[i] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsCubeMap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, PCMR, PCMR, 0, GL_RGBA, GL_FLOAT, Colors);
    }

    glGenerateMipmapEXT(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    delete[] Colors;

    // ------------------------------------------------------------------------------------------------------------------------

    glGenBuffers(1, &PhotonsVBO);

    int PMR = PCMR, PMRP1 = PMR + 1;

    PhotonsCount = PMRP1 * PMRP1;

    vec3 *Photons = new vec3[PhotonsCount];

    float WMSDPMR = 2.0f / (float)PMR;

    int i = 0;

    for (int y = 0; y <= PMR; y++)
    {
        for (int x = 0; x <= PMR; x++)
        {
            Photons[i++] = vec3((float)x * WMSDPMR - 1.0f, 0.0f, 1.0f - (float)y * WMSDPMR);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, PhotonsVBO);
    glBufferData(GL_ARRAY_BUFFER, PhotonsCount * 12, Photons, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] Photons;

    // ------------------------------------------------------------------------------------------------------------------------

    glGenBuffers(1, &PoolSkyVBO);

    float PoolSkyVertices[] =
    {	// x, y, z, x, y, z, x, y, z, x, y, z
        1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, // +X
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, // -X
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, // +Y
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, // -Y
        1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, // +Z
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f  // -Z
    };

    glBindBuffer(GL_ARRAY_BUFFER, PoolSkyVBO);
    glBufferData(GL_ARRAY_BUFFER, 288, PoolSkyVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ------------------------------------------------------------------------------------------------------------------------

    glGenBuffers(1, &WaterVBO);

    int WMRP1 = WMR + 1;

    vec3 *Vertices = new vec3[WMRP1 * WMRP1];

    float WMSDWMR = 2.0f / (float)WMR;

    for (int y = 0; y <= WMR; y++)
    {
        for (int x = 0; x <= WMR; x++)
        {
            Vertices[WMRP1 * y + x].x = x * WMSDWMR - 1.0f;
            Vertices[WMRP1 * y + x].y = 0.0f;
            Vertices[WMRP1 * y + x].z = 1.0f - y * WMSDWMR;
        }
    }

    CBuffer Quads;

    for (int y = 0; y < WMR; y++)
    {
        int yp1 = y + 1;

        for (int x = 0; x < WMR; x++)
        {
            int xp1 = x + 1;

            int a = WMRP1 * y + x;
            int b = WMRP1 * y + xp1;
            int c = WMRP1 * yp1 + xp1;
            int d = WMRP1 * yp1 + x;

            Quads.AddData(&Vertices[a], 12);
            Quads.AddData(&Vertices[b], 12);
            Quads.AddData(&Vertices[c], 12);
            Quads.AddData(&Vertices[d], 12);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, WaterVBO);
    glBufferData(GL_ARRAY_BUFFER, Quads.GetDataSize(), Quads.GetData(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    QuadsVerticesCount = Quads.GetDataSize() / 12;

    Quads.Empty();

    delete[] Vertices;

    // ------------------------------------------------------------------------------------------------------------------------

    glGenFramebuffersEXT(1, &FBO);

    // ------------------------------------------------------------------------------------------------------------------------

    Camera.Look(vec3(0.0f, 1.0f, 2.5f), vec3(0.0f, -0.5f, 0.0f), true);

    // ------------------------------------------------------------------------------------------------------------------------

    srand(GetTickCount());

    // ------------------------------------------------------------------------------------------------------------------------

    return true;
}

void COpenGLRenderer::Render(float FrameTime)
{
    // add drops --------------------------------------------------------------------------------------------------------------

    if (!Pause)
    {
        static DWORD LastTime = GetTickCount();

        DWORD Time = GetTickCount();

        if (Time - LastTime > 100)
        {
            LastTime = Time;

            AddDrop(2.0f * (float)rand() / (float)RAND_MAX - 1.0f, 1.0f - 2.0f * (float)rand() / (float)RAND_MAX, 4.0f / 128.0f * (float)rand() / (float)RAND_MAX);
        }
    }

    // update water surface and generate photon cube map ----------------------------------------------------------------------

    static DWORD LastTime = GetTickCount();

    DWORD Time = GetTickCount();

    if (Time - LastTime >= 16)
    {
        LastTime = Time;

        // update water height map --------------------------------------------------------------------------------------------

        glViewport(0, 0, WHMR, WHMR);

        GLuint whmid = (WHMID + 1) % 2;

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, WaterHeightMaps[whmid], 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

        glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[WHMID]);
        glUseProgram(WaterHeightMapProgram);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(0.0f, 1.0f);
        glEnd();
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[whmid]);
        glGenerateMipmapEXT(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        ++WHMID %= 2;

        // update water normal map --------------------------------------------------------------------------------------------

        glViewport(0, 0, WNMR, WNMR);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, WaterNormalMap, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

        glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[WHMID]);
        glUseProgram(WaterNormalMapProgram);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(0.0f, 1.0f);
        glEnd();
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glBindTexture(GL_TEXTURE_2D, WaterNormalMap);
        glGenerateMipmapEXT(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        // render photons into photons texture --------------------------------------------------------------------------------

        glViewport(0, 0, PCMR * 2, PCMR * 3);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, PhotonsTexture, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[WHMID]);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, WaterNormalMap);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, PhotonsTexture);
        glUseProgram(PhotonProgram);
        glBindBuffer(GL_ARRAY_BUFFER, PhotonsVBO);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 12, (void*)0);
        glDrawArrays(GL_POINTS, 0, PhotonsCount);
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        // --------------------------------------------------------------------------------------------------------------------

        static vec2 PhotonsTextureCoords[] = {
            vec2(0.0f / 2.0f, 0.0f / 3.0f), vec2(1.0f / 2.0f, 0.0f / 3.0f), vec2(1.0f / 2.0f, 1.0f / 3.0f), vec2(0.0f / 2.0f, 1.0f / 3.0f),
            vec2(1.0f / 2.0f, 0.0f / 3.0f), vec2(2.0f / 2.0f, 0.0f / 3.0f), vec2(2.0f / 2.0f, 1.0f / 3.0f), vec2(1.0f / 2.0f, 1.0f / 3.0f),
            vec2(1.0f / 2.0f, 1.0f / 3.0f), vec2(2.0f / 2.0f, 1.0f / 3.0f), vec2(2.0f / 2.0f, 2.0f / 3.0f), vec2(1.0f / 2.0f, 2.0f / 3.0f),
            vec2(0.0f / 2.0f, 1.0f / 3.0f), vec2(1.0f / 2.0f, 1.0f / 3.0f), vec2(1.0f / 2.0f, 2.0f / 3.0f), vec2(0.0f / 2.0f, 2.0f / 3.0f),
            vec2(1.0f / 2.0f, 2.0f / 3.0f), vec2(2.0f / 2.0f, 2.0f / 3.0f), vec2(2.0f / 2.0f, 3.0f / 3.0f), vec2(1.0f / 2.0f, 3.0f / 3.0f),
            vec2(0.0f / 2.0f, 2.0f / 3.0f), vec2(1.0f / 2.0f, 2.0f / 3.0f), vec2(1.0f / 2.0f, 3.0f / 3.0f), vec2(0.0f / 2.0f, 3.0f / 3.0f),
        };

        // copy photons texture to photons temp cube map 1 --------------------------------------------------------------------

        glViewport(0, 0, PCMR, PCMR);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        for (int i = 0; i < 6; i++)
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PhotonsTempCubeMaps[0], 0);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, PhotonsTexture);
            glBegin(GL_QUADS);
            glTexCoord2fv(&PhotonsTextureCoords[i * 4 + 0]); glVertex2f(-1.0f, -1.0f);
            glTexCoord2fv(&PhotonsTextureCoords[i * 4 + 1]); glVertex2f(1.0f, -1.0f);
            glTexCoord2fv(&PhotonsTextureCoords[i * 4 + 2]); glVertex2f(1.0f, 1.0f);
            glTexCoord2fv(&PhotonsTextureCoords[i * 4 + 3]); glVertex2f(-1.0f, 1.0f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }

        // --------------------------------------------------------------------------------------------------------------------

        static vec3 PhotonsCubeMapCoords[] = {
            vec3(1.0f,  1.0f,  1.0f), vec3(1.0f,  1.0f, -1.0f), vec3(1.0f, -1.0f, -1.0f), vec3(1.0f, -1.0f,  1.0f),
            vec3(-1.0f,  1.0f, -1.0f), vec3(-1.0f,  1.0f,  1.0f), vec3(-1.0f, -1.0f,  1.0f), vec3(-1.0f, -1.0f, -1.0f),
            vec3(-1.0f, -1.0f,  1.0f), vec3(1.0f, -1.0f,  1.0f), vec3(1.0f, -1.0f, -1.0f), vec3(-1.0f, -1.0f, -1.0f),
            vec3(-1.0f,  1.0f, -1.0f), vec3(1.0f,  1.0f, -1.0f), vec3(1.0f,  1.0f,  1.0f), vec3(-1.0f,  1.0f,  1.0f),
            vec3(1.0f,  1.0f, -1.0f), vec3(-1.0f,  1.0f, -1.0f), vec3(-1.0f, -1.0f, -1.0f), vec3(1.0f, -1.0f, -1.0f),
            vec3(-1.0f,  1.0f,  1.0f), vec3(1.0f,  1.0f,  1.0f), vec3(1.0f, -1.0f,  1.0f), vec3(-1.0f, -1.0f,  1.0f)
        };

        static float ODPCMR = 1.0f / (float)PCMR;

        static vec3 PhotonsCubeMapHorizontalOffsets[] = {
            vec3(0.0f, 0.0f, -ODPCMR), vec3(0.0f, 0.0f, ODPCMR),
            vec3(ODPCMR, 0.0f, 0.0f), vec3(ODPCMR, 0.0f, 0.0f),
            vec3(-ODPCMR, 0.0f, 0.0f), vec3(ODPCMR, 0.0f, 0.0f)
        };

        static vec3 PhotonsCubeMapVerticalOffsets[] = {
            vec3(0.0f, -ODPCMR, 0.0f), vec3(0.0f, -ODPCMR, 0.0f),
            vec3(0.0f, 0.0f, -ODPCMR), vec3(0.0f, 0.0f, ODPCMR),
            vec3(0.0f, -ODPCMR, 0.0f), vec3(0.0f, -ODPCMR, 0.0f)
        };

        // blur photons temp cube map 1 horizontally --------------------------------------------------------------------------

        for (int i = 0; i < 6; i++)
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PhotonsTempCubeMaps[1], 0);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

            glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsTempCubeMaps[0]);
            glUseProgram(CubeMapHBlurProgram);
            glUniform3fv(CubeMapHBlurProgram.UniformLocations[0], 1, &PhotonsCubeMapHorizontalOffsets[i]);
            glBegin(GL_QUADS);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 0]); glVertex2f(-1.0f, -1.0f);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 1]); glVertex2f(1.0f, -1.0f);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 2]); glVertex2f(1.0f, 1.0f);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 3]); glVertex2f(-1.0f, 1.0f);
            glEnd();
            glUseProgram(0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }

        // blur photons temp cube map 2 vertically ----------------------------------------------------------------------------

        for (int i = 0; i < 6; i++)
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PhotonsCubeMap, 0);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

            glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsTempCubeMaps[1]);
            glUseProgram(CubeMapVBlurProgram);
            glUniform3fv(CubeMapVBlurProgram.UniformLocations[0], 1, &PhotonsCubeMapVerticalOffsets[i]);
            glBegin(GL_QUADS);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 0]); glVertex2f(-1.0f, -1.0f);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 1]); glVertex2f(1.0f, -1.0f);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 2]); glVertex2f(1.0f, 1.0f);
            glTexCoord3fv(&PhotonsCubeMapCoords[i * 4 + 3]); glVertex2f(-1.0f, 1.0f);
            glEnd();
            glUseProgram(0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }

        // generate mipmaps ---------------------------------------------------------------------------------------------------

        glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsCubeMap);
        glGenerateMipmapEXT(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // render pool sky mesh ---------------------------------------------------------------------------------------------------

    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&ProjectionMatrix);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&ViewMatrix);

    if (WireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, PoolSkyCubeMap);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsCubeMap);
    glUseProgram(PoolSkyProgram);
    glBindBuffer(GL_ARRAY_BUFFER, PoolSkyVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 12, (void*)0);
    glDrawArrays(GL_QUADS, 0, 24);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDisable(GL_CULL_FACE);

    // render water surface ---------------------------------------------------------------------------------------------------

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[WHMID]);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, WaterNormalMap);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_CUBE_MAP, PoolSkyCubeMap);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_CUBE_MAP, PhotonsCubeMap);
    glUseProgram(WaterProgram);
    glUniform3fv(WaterProgram.UniformLocations[0], 1, &Camera.Position);
    glBindBuffer(GL_ARRAY_BUFFER, WaterVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 12, (void*)0);
    glDrawArrays(GL_QUADS, 0, QuadsVerticesCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);

    if (WireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisable(GL_DEPTH_TEST);
}

void COpenGLRenderer::Resize(int Width, int Height)
{
    this->Width = Width;
    this->Height = Height;

    ProjectionMatrix = perspective(45.0f, (float)Width / (float)Height, 0.125f, 512.0f);
    ProjectionBiasMatrixInverse = inverse(ProjectionMatrix) * BiasMatrixInverse;
}

void COpenGLRenderer::Destroy()
{
    PoolSkyCubeMap.Destroy();

    WaterAddDropProgram.Destroy();
    WaterHeightMapProgram.Destroy();
    WaterNormalMapProgram.Destroy();
    PhotonProgram.Destroy();
    CubeMapHBlurProgram.Destroy();
    CubeMapVBlurProgram.Destroy();
    PoolSkyProgram.Destroy();
    WaterProgram.Destroy();

    glDeleteTextures(2, WaterHeightMaps);
    glDeleteTextures(1, &WaterNormalMap);
    glDeleteTextures(1, &PhotonsTexture);
    glDeleteTextures(2, PhotonsTempCubeMaps);
    glDeleteTextures(1, &PhotonsCubeMap);

    glDeleteBuffers(1, &PhotonsVBO);
    glDeleteBuffers(1, &PoolSkyVBO);
    glDeleteBuffers(1, &WaterVBO);

    if (GLEW_EXT_framebuffer_object)
    {
        glDeleteFramebuffersEXT(1, &FBO);
    }
}

void COpenGLRenderer::AddDrop(float x, float y, float DropRadius)
{
    if (x >= -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f)
    {
        glViewport(0, 0, WMR, WMR);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, WaterHeightMaps[(WHMID + 1) % 2], 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

        glBindTexture(GL_TEXTURE_2D, WaterHeightMaps[WHMID]);
        glUseProgram(WaterAddDropProgram);
        glUniform1f(WaterAddDropProgram.UniformLocations[0], DropRadius);
        glUniform2fv(WaterAddDropProgram.UniformLocations[1], 1, &vec2(x * 0.5f + 0.5f, 0.5f - y * 0.5f));
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(0.0f, 1.0f);
        glEnd();
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        ++WHMID %= 2;
    }
}

void COpenGLRenderer::AddDropByMouseClick(int x, int y)
{
    float s = (float)x / (float)(Width - 1);
    float t = 1.0f - (float)y / (float)(Height - 1);

    vec4 Position = ViewMatrixInverse * (ProjectionBiasMatrixInverse * vec4(s, t, 0.5f, 1.0f));
    Position /= Position.w;

    vec3 Ray = normalize(*(vec3*)&Position - Camera.Position);

    vec3 Normal = vec3(0.0f, 1.0f, 0.0f);
    float D = -dot(Normal, vec3(0.0f, 0.0f, 0.0f));

    float NdotR = -dot(Normal, Ray);

    if (NdotR != 0.0f)
    {
        float Distance = (dot(Normal, Camera.Position) + D) / NdotR;

        if (Distance > 0.0f)
        {
            vec3 Position = Ray * Distance + Camera.Position;

            AddDrop(Position.x, Position.z, DropRadius);
        }
    }
}
