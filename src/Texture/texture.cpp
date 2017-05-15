#include <cmath>

#include "texture.hpp"

#include "cstring.hpp"

extern CString ModuleDirectory;
extern CString ErrorLog;

extern int gl_max_texture_size;
extern int gl_max_texture_max_anisotropy_ext;


CTexture::CTexture()
    : Texture(0)
{
}

CTexture::operator GLuint ()
{
    return Texture;
}

bool CTexture::LoadTexture2D(char *FileName)
{
    CString DirectoryFileName = ModuleDirectory + FileName;

    int Width, Height, BPP;

    FIBITMAP *dib = GetBitmap(DirectoryFileName, Width, Height, BPP);

    if (dib == NULL)
    {
        ErrorLog.Append("Error loading texture " + DirectoryFileName + "!\r\n");
        return false;
    }

    GLenum Format = 0;

    if (BPP == 32) Format = GL_BGRA;
    if (BPP == 24) Format = GL_BGR;

    if (Format == 0)
    {
        ErrorLog.Append("Unsupported texture format (%s)!\r\n", FileName);
        FreeImage_Unload(dib);
        return false;
    }

    Destroy();

    glGenTextures(1, &Texture);

    glBindTexture(GL_TEXTURE_2D, Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (GLEW_EXT_texture_filter_anisotropic)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib));

    glBindTexture(GL_TEXTURE_2D, 0);

    FreeImage_Unload(dib);

    return true;
}

bool CTexture::LoadTextureCubeMap(char **FileNames)
{
    int Width, Height, BPP;

    FIBITMAP *dib[6];

    bool Error = false;

    for (int i = 0; i < 6; i++)
    {
        CString DirectoryFileName = ModuleDirectory + FileNames[i];

        dib[i] = GetBitmap(DirectoryFileName, Width, Height, BPP);

        if (dib[i] == NULL)
        {
            ErrorLog.Append("Error loading texture " + DirectoryFileName + "!\r\n");
            Error = true;
        }
    }

    if (Error)
    {
        for (int i = 0; i < 6; i++)
        {
            FreeImage_Unload(dib[i]);
        }

        return false;
    }

    GLenum Format = 0;

    if (BPP == 32) Format = GL_BGRA;
    if (BPP == 24) Format = GL_BGR;

    if (Format == 0)
    {
        ErrorLog.Append("Unsupported texture format (%s)!\r\n", FileNames[5]);

        for (int i = 0; i < 6; i++)
        {
            FreeImage_Unload(dib[i]);
        }

        return false;
    }

    Destroy();

    glGenTextures(1, &Texture);

    glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (GLEW_EXT_texture_filter_anisotropic)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, Width, Height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib[i]));
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    for (int i = 0; i < 6; i++)
    {
        FreeImage_Unload(dib[i]);
    }

    return true;
}

void CTexture::Destroy()
{
    glDeleteTextures(1, &Texture);
    Texture = 0;
}

FIBITMAP * CTexture::GetBitmap(char * FileName, int & Width, int & Height, int & BPP)
{
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(FileName);

    if (fif == FIF_UNKNOWN)
    {
        fif = FreeImage_GetFIFFromFilename(FileName);
    }

    if (fif == FIF_UNKNOWN)
    {
        return NULL;
    }

    FIBITMAP *dib = NULL;

    if (FreeImage_FIFSupportsReading(fif))
    {
        dib = FreeImage_Load(fif, FileName);
    }

    if (dib != NULL)
    {
        int OriginalWidth = FreeImage_GetWidth(dib);
        int OriginalHeight = FreeImage_GetHeight(dib);

        Width = OriginalWidth;
        Height = OriginalHeight;

        if (Width == 0 || Height == 0)
        {
            FreeImage_Unload(dib);
            return NULL;
        }

        BPP = FreeImage_GetBPP(dib);

        if (Width > gl_max_texture_size) Width = gl_max_texture_size;
        if (Height > gl_max_texture_size) Height = gl_max_texture_size;

        if (!GLEW_ARB_texture_non_power_of_two)
        {
            Width = 1 << (int)floor((log((float)Width) / log(2.0f)) + 0.5f);
            Height = 1 << (int)floor((log((float)Height) / log(2.0f)) + 0.5f);
        }

        if (Width != OriginalWidth || Height != OriginalHeight)
        {
            FIBITMAP *rdib = FreeImage_Rescale(dib, Width, Height, FILTER_BICUBIC);
            FreeImage_Unload(dib);
            dib = rdib;
        }
    }

    return dib;
}
