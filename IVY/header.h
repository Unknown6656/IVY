#pragma once

#include "stdafx.h"

#ifdef USE_BUILTIN
    typedef unsigned char byte;
    typedef unsigned short ushort;
    typedef unsigned int uint;
#else
    #include <stdint.h>

    typedef uint8_t byte;
    typedef uint16_t ushort;
    typedef uint32_t uint;
#endif

struct _bitmap;

typedef void(*setpixel_pointer)(_bitmap*, uint, uint, uint);
typedef uint(*getpixel_pointer)(_bitmap*, uint, uint);


enum BitmapType
{
    Gray_8,
    ARGB_32,
    YUV_16
};

struct _bitmap
{
    uint Width;
    uint Height;
    byte* Data;
    BitmapType Type;


    _bitmap(void* d, uint w, uint h, BitmapType t)
    {
        Data = (byte*)d;
        Width = w;
        Height = h;
        Type = t;
    }

    void SetPixel(setpixel_pointer func, uint x, uint y, uint c)
    {
        if ((x >= 0) && (y >= 0) && (x < Width) && (y < Height))
            func(this, x, y, c);
    }

    uint GetPixel(getpixel_pointer func, uint x, uint y)
    {
        return (x >= 0) && (y >= 0) && (x < Width) && (y < Height) ? func(this, x, y) : 0;
    }
};

class PixelProvider
{
protected:
    const _bitmap* bitmap = NULL;
    const byte* data = NULL;

public:
    virtual void SetPixelFromRGBA(uint, uint, byte, byte, byte, byte) = 0;
    virtual void SetPixelFromRGBA(uint x, uint y, uint rgba)
    {
        SetPixelFromRGBA(x, y, rgba >> 24, rgba >> 16, rgba >> 8, rgba);
    }
    virtual void SetPixelFromGray(uint, uint, byte) = 0;
    virtual void SetPixelFromYUV(uint, uint, uint) = 0;

    virtual uint GetPixelAsRGBA(uint, uint) = 0;
    virtual byte GetPixelAsGray(uint, uint) = 0;
    virtual uint GetPixelAsYUV(uint, uint) = 0;

    PixelProvider(const _bitmap* bmp)
    {
        bitmap = bmp;
    }
};


void BGRA_to_RGBA(_bitmap);
void RGBA_to_BGRA(_bitmap);
void TestProcess(_bitmap, _bitmap);


template <typename T> T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}
