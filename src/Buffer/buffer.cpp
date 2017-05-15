#include <cstring>

#include "buffer.hpp"

CBuffer::CBuffer()
{
    SetDefaults();
}

CBuffer::~CBuffer()
{
    Empty();
}

void CBuffer::AddData(void * Data, int DataSize)
{
    int Remaining = BufferSize - Position;

    if (DataSize > Remaining)
    {
        BYTE *OldBuffer = Buffer;
        int OldBufferSize = BufferSize;

        int Needed = DataSize - Remaining;

        BufferSize += Needed > BUFFER_SIZE_INCREMENT ? Needed : BUFFER_SIZE_INCREMENT;

        Buffer = new BYTE[BufferSize];

        memcpy(Buffer, OldBuffer, OldBufferSize);

        delete[] OldBuffer;
    }

    memcpy(Buffer + Position, Data, DataSize);

    Position += DataSize;
}

void CBuffer::Empty()
{
    delete[] Buffer;

    SetDefaults();
}

void * CBuffer::GetData()
{
    return Buffer;
}

int CBuffer::GetDataSize()
{
    return Position;
}

void CBuffer::SetDefaults()
{
    Buffer = nullptr;

    BufferSize = 0;
    Position = 0;
}
