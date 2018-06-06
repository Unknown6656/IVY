#include "stdafx.h"
#include "header.h"

using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System;


void switch_BGRA_RGBA(_bitmap, bool);


int main(array<String^>^ args)
{
    auto dir = IO::Directory::GetCurrentDirectory();

    Bitmap^ src = gcnew Bitmap(Bitmap::FromFile(L"./input.png"));
    PixelFormat fmt = PixelFormat::Format32bppArgb;
    int w = src->Width;
    int h = src->Height;

    Bitmap^ dst = gcnew Bitmap(w, h, fmt);
    BitmapData^ sdat = src->LockBits(Rectangle(0, 0, w, h), ImageLockMode::ReadWrite, fmt);
    BitmapData^ ddat = dst->LockBits(Rectangle(0, 0, w, h), ImageLockMode::ReadWrite, fmt);
    _bitmap bsrc((void*)sdat->Scan0, w, h, BitmapType::ARGB_32);
    _bitmap bdst((void*)ddat->Scan0, w, h, BitmapType::ARGB_32);

    TestProcess(bsrc, bdst);

    switch_BGRA_RGBA(bsrc, true);

    TestProcess(bsrc, bdst);

    switch_BGRA_RGBA(bdst, false);

    src->UnlockBits(sdat);
    dst->UnlockBits(ddat);
    dst->Save(L"./output.png");

    delete src;
    delete dst;
    return 0;
}

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

void switch_BGRA_RGBA(_bitmap bmp, bool dir)
{
    uint __ = 0x0066AAFF;
    bool little_endian = *((byte*)&__) != 0xFF;

    for (int i = 0, l = bmp.Width * bmp.Height; i < l; ++i)
    {
        uint* ptr = (uint*)bmp.Data + i;

        if (little_endian && !dir)
            *ptr = swap_endian<uint>(*ptr);

        *ptr = (*ptr & 0x00FF00FF)
            | ((*ptr & 0xFF000000) >> 16)
            | ((*ptr & 0x0000FF00) << 16);

        if (little_endian && dir)
            *ptr = swap_endian<uint>(*ptr);
    }
}

void TestProcess(_bitmap src, _bitmap dst)
{
    for (int i = 0, cnt = src.Width * src.Height; i < cnt; ++i)
        for (int j = 0; j < 4; ++j)
            if (j == 0)
                dst.Data[i * 4 + j] = 0xff;
            else
                dst.Data[i * 4 + j] = src.Data[i * 4 + j];
}
