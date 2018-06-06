#include "stdafx.h"
#include "header.h"

using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System;


bool little_endian;


int main(array<String^>^ args)
{
    uint __ = 0x0066AAFF;
    little_endian = *((byte*)&__) != 0x00;

    Bitmap^ src = gcnew Bitmap(Bitmap::FromFile(L"./input.png"));
    PixelFormat fmt = PixelFormat::Format32bppArgb;
    int w = src->Width;
    int h = src->Height;

    Bitmap^ dst = gcnew Bitmap(w, h, fmt);
    BitmapData^ sdat = src->LockBits(Rectangle(0, 0, w, h), ImageLockMode::ReadWrite, fmt);
    BitmapData^ ddat = dst->LockBits(Rectangle(0, 0, w, h), ImageLockMode::ReadWrite, fmt);
    _bitmap bsrc((void*)sdat->Scan0, w, h, BitmapType::ARGB_32);
    _bitmap bdst((void*)ddat->Scan0, w, h, BitmapType::ARGB_32);

    BGRA_to_RGBA(bsrc);

    TestProcess(bsrc, bdst);

    RGBA_to_BGRA(bdst);

    src->UnlockBits(sdat);
    dst->UnlockBits(ddat);
    dst->Save(L"./output.png");

    delete src;
    delete dst;
    return 0;
}

void BGRA_to_RGBA(_bitmap bmp)
{
    for (int i = 0, l = bmp.Width * bmp.Height; i < l; ++i)
    {
        uint* ptr = (uint*)bmp.Data + i;

        *ptr = (*ptr & 0xFF00FF00)
            | ((*ptr & 0x00FF0000) >> 16)
            | ((*ptr & 0x000000FF) << 16);

        if (!little_endian)
            *ptr = swap_endian(*ptr);
    }
}

void RGBA_to_BGRA(_bitmap bmp)
{
    for (int i = 0, l = bmp.Width * bmp.Height; i < l; ++i)
    {
        uint* ptr = (uint*)bmp.Data + i;

        if (!little_endian)
            *ptr = swap_endian(*ptr);

        *ptr = (*ptr & 0xFF00FF00)
            | ((*ptr & 0x00FF0000) >> 16)
            | ((*ptr & 0x000000FF) << 16);
    }
}

void TestProcess(_bitmap src, _bitmap dst)
{
    for (int i = 0, cnt = src.Width * src.Height; i < cnt; ++i)
        for (int j = 0; j < 4; ++j)
            dst.Data[i * 4 + j] = src.Data[i * 4 + j];
}
