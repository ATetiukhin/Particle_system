#ifndef BUFFER_HPP_INCLUDED
#define BUFFER_HPP_INCLUDED

#define BUFFER_SIZE_INCREMENT 1048576

typedef unsigned char BYTE;

class CBuffer
{
public:
    CBuffer();
    ~CBuffer();

    void AddData(void *Data, int DataSize);
    void Empty();
    void *GetData();
    int GetDataSize();

private:
    BYTE *Buffer;
    int BufferSize;
    int Position;

    void SetDefaults();
};


#endif /* End of 'buffer.hpp' file */