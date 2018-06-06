#include "stdafx.h"
#include "header.h"


byte grayscale(byte r, byte g, byte b)
{
    return (byte)((r * 0.299)
                + (g * 0.587)
                + (b * 0.114));
}

class YUVPixelProvider
    : PixelProvider
{
private:
    /* ---------------------------------------------------------------------
    * Name: rgba2gray
    * Function: Converts 32-bit RGBA-value to YUV and writes back Y
    * Input:
    * 	- in: 32-bit RGBA-value, which is to be converted.
    *   - yuv: Pointer to the pixel(s) to manipulate
    *   - idx: 0 to edit the left pixel's Y, nonzero to edit the right pixel
    ---------------------------------------------------------------------*/
    inline static void rgba2gray(uint in, uint *yuv, int idx)
    {
        uint L;

        // Cauculation of Y from RGBA
        L =  (byte)((in & 0x000000FF)        * 0.299);
        L += (byte)(((in & 0x0000FF00) >> 8) * 0.587);
        L += (byte)(((in & 0x00FF0000) >> 16) * 0.114);

        if (idx)
        {
            *yuv &= 0xFFFFFF00;
            *yuv |= L;
        }
        else
        {
            *yuv &= 0xFF00FFFF;
            *yuv |= (L << 16);
        }
    }

    /* ---------------------------------------------------------------------
    * Name: rgba2yuv
    * Function: Converts 32-bit RGBA-value to YUV and writes back U, V and one Y
    * Input:
    * 	- in: 32-bit RGBA-value, which is to be converted.
    *   - yuv: Pointer to the pixel(s) to manipulate
    *   - idx: 0 to edit the left pixel's Y, nonzero to edit the right pixel
    ---------------------------------------------------------------------*/
    inline static void rgba2yuv(uint in, uint *yuv, int idx)
    {
        uint YUV;

        // Cauculation of Y from RGBA
        YUV =  (byte)((in & 0x000000FF)        * 0.299);
        YUV += (byte)(((in & 0x0000FF00) >> 8) * 0.587);
        YUV += (byte)(((in & 0x00FF0000) >> 16) * 0.114);

        if (idx)
        {
            *yuv &= 0xFFFFFF00;
            *yuv |= YUV;
        }
        else
        {
            *yuv &= 0xFF00FFFF;
            *yuv |= (YUV << 16);
        }

        // Calculation of U/Cb from RGBA
        YUV =  (byte)((in & 0x000000FF)        * -0.14713);
        YUV += (byte)(((in & 0x0000FF00) >> 8) * -0.28886);
        YUV += (byte)(((in & 0x00FF0000) >> 16) * 0.436);

        YUV <<= 16;

        // Calculation ov V/Cr from RGBA
        YUV += (byte)((in & 0x000000FF)        * 0.615);
        YUV += (byte)(((in & 0x0000FF00) >> 8) * -0.51499);
        YUV += (byte)(((in & 0x00FF0000) >> 16) * -0.10001);

        YUV <<= 8;

        *yuv &= 0x00FF00FF;
        *yuv |= YUV;
    }

public:
    inline void SetPixelFromRGBA(uint x, uint y, uint rgba)
    {
        rgba2yuv(rgba, (uint*)(this->bitmap->Data + 2 * (this->bitmap->Width * y + x & 0xFFFFFFFE)), x % 2);
    }

    inline void SetPixelFromRGBA(uint x, uint y, byte r, byte g, byte b, byte a)
    {
        SetPixelFromRGBA(x, y, RGBA(r, g, b, a));
    }

    inline void SetPixelFromGray(uint x, uint y, byte gray)
    {
        uint *pixel = (uint*)(this->bitmap->Data + 2 * (this->bitmap->Width * y + x & 0xFFFFFFFE));

        if (x % 2)
        {
            *pixel &= 0x000000FF;
            *pixel |= 0x80008000;
            *pixel |= (uint)gray << 16;
        }
        else
        {
            *pixel &= 0x00FF0000;
            *pixel |= 0x80008000;
            *pixel |= gray;
        }
    }

    inline void SetPixelFromYUV(uint x, uint y, uint yuv)
    {
        uint *pixel = (uint*)(this->bitmap->Data + 2 * (this->bitmap->Width * y + x & 0xFFFFFFFE));

        char luma;

        if (x % 2)
        {
            luma = (char)(*pixel >> 16);
            *pixel = yuv & 0xFF00FFFF;
            *pixel |= (uint)luma << 16;
        }
        else
        {
            luma = (char)(*pixel);
            *pixel = yuv & 0xFFFFFF00;
            *pixel |= luma;
        }
    }

    inline uint GetPixelAsRGBA(uint x, uint y)
    {
        // TODO
    }

    inline byte GetPixelAsGray(uint x, uint y)
    {
        // TODO
    }

    inline uint GetPixelAsYUV(uint x, uint y)
    {
        // TODO
    }
};

class RGBAPixelProvider
    : PixelProvider
{
    inline void SetPixelFromRGBA(uint x, uint y, byte r, byte g, byte b, byte a)
    {
        byte* ptr = this->bitmap->Data + (y * this->bitmap->Width + x) * 4;

        ptr[0] = r;
        ptr[1] = g;
        ptr[2] = b;
        ptr[3] = a;
    }

    inline void SetPixelFromGray(uint x, uint y, byte gray)
    {
        SetPixelFromRGBA(x, y, gray, gray, gray, 255);
    }

    inline void SetPixelFromYUV(uint x, uint y, uint yuv)
    {
        // TODO
    }

    inline uint GetPixelAsRGBA(uint x, uint y)
    {
        byte* ptr = this->bitmap->Data + (y * this->bitmap->Width + x) * 4;

        return RGBA(ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    inline byte GetPixelAsGray(uint x, uint y)
    {
        byte r, g, b, a;

        RGBA(GetPixelAsRGBA(x, y), r, g, b, a);

        return grayscale(r, g, b);
    }

    inline uint GetPixelAsYUV(uint x, uint y)
    {
        // TODO
    }
};

class GrayPixelProvider
    : PixelProvider
{
    inline void SetPixelFromRGBA(uint x, uint y, byte r, byte g, byte b, byte a)
    {
        SetPixelFromGray(x, y, grayscale(r, g, b));
    }

    inline void SetPixelFromGray(uint x, uint y, byte gray)
    {
        this->bitmap->Data[y * this->bitmap->Width + x] = gray;
    }

    inline void SetPixelFromYUV(uint x, uint y, uint yuv)
    {
        // TODO
    }

    inline uint GetPixelAsRGBA(uint x, uint y)
    {
        byte gray = GetPixelAsGray(x, y);

        return RGBA(gray, gray, gray, 0xff);
    }

    inline byte GetPixelAsGray(uint x, uint y)
    {
        return this->bitmap->Data[y * this->bitmap->Width + x];
    }

    inline uint GetPixelAsYUV(uint x, uint y)
    {
        byte gray = GetPixelAsGray(x, y);

        // TODO
    }
};

