#ifndef TEXTURE_HPP_INCLUDED
#define TEXTURE_HPP_INCLUDED

#include "GL/glew.h" // http://glew.sourceforge.net/
#include "GL/wglew.h"

#include "FreeImage.h" // http://freeimage.sourceforge.net/

class CTexture
{
protected:
    GLuint Texture;

public:
    CTexture();
    ~CTexture();

    operator GLuint ();

    bool LoadTexture2D(char *FileName);
    bool LoadTextureCubeMap(char **FileNames);
    void Destroy();

protected:
    FIBITMAP *CTexture::GetBitmap(char *FileName, int &Width, int &Height, int &BPP);
};


#endif /* End of 'texture.hpp' file */