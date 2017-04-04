/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IceException.h"
#include "defs.h"
#include "macro.h"
#include "util.h"

#include "exfile.h"
#include "picio.h"
#include "pcxio.h"

using namespace std;

namespace ice
{
#define FNAME "ReadImgPCX"
  Image ReadPCXImg(const string& fname, Image& img, int flag)
  {
    FILE* fd;
    string hname = fname;
    struct HeaderPCX Header;
    unsigned char Ch;
    unsigned char* PCXLine;
    unsigned char Shift, Shift1, Shift2;
    int i, j, ii;
    int Count, MaxVal, val;
    int newimg;
    int sizex, sizey, xmax, ymax;
    int PixelPerByte;
    int ByteNbr, PosInByte;

    if (!IsImg(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }

    if ((fd = fopen(hname.c_str(), FRMODUS)) == NULL)
      {
        throw IceException(FNAME, M_FILE_OPEN, FILE_NOT_FOUND);
      }

    fread((unsigned char*)&Header, 128, 1, fd);

    if (Header.Signature != 0x0a)
      {
        throw IceException(FNAME, "Kein PCX-File !!!", ERROR);
        fclose(fd);
      }

    switch ((int)Header.BitsPerPixel)
      {
      case 1:
        Shift1 = 0;
        MaxVal = 1;
        break;
      case 2:
        Shift1 = 1;
        MaxVal = 3;
        break;
      case 4:
        Shift1 = 2;
        MaxVal = 15;
        break;
      case 8:
        Shift1 = 3;
        MaxVal = 255;
        break;
      default:
        fclose(fd);
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
      }

    Shift2 = 3 - Shift1;
    PixelPerByte = 0x08 >> Shift1;

    sizex = Header.MaxX - Header.MinX + 1;
    sizey = Header.MaxY - Header.MinY + 1;

    newimg = false;

    if (!IsImg(img))
      {
        img = NewImg(sizex, sizey, MaxVal);

        if (!IsImg(img))
          {
            fclose(fd);
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
          }

        newimg = true; /* merken dass Bild neu angefordert */
      }

    xmax = min(sizex, img->xsize);
    ymax = min(sizey, img->ysize);
    /* Speicheranforderung fuer ungepackte PCX-Zeile */
    PCXLine = (unsigned char*) malloc(Header.BytesPerLine * sizeof(unsigned char));

    for (j = 0; j < ymax; j++)
      {
        i = 0;

        /* Generieren des Bitmusters einer unkomprimierten PCX-File-Zeile */
        while (i < Header.BytesPerLine)
          {
            if ((val = fread(&Ch, 1, 1, fd)) == 0)
              {
                fclose(fd);

                if (newimg)
                  {
                    FreeImg(img);
                  }

                free(PCXLine);
                throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
              }

            if (Ch >> 6 == 0x03)
              {
                /* Daten komprimiert */
                Count = Ch & 0x3f;

                if ((val = fread(&Ch, 1, 1, fd)) == 0)
                  {
                    fclose(fd);

                    if (newimg)
                      {
                        FreeImg(img);
                      }

                    free(PCXLine);
                    throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
                  }

                for (ii = 0; ii < Count; ii++)
                  {
                    PCXLine[i++] = Ch;

                    if (i > Header.BytesPerLine)
                      {
                        fclose(fd);

                        if (newimg)
                          {
                            FreeImg(img);
                          }

                        free(PCXLine);
                        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
                      }
                  }
              }
            else
              {
                PCXLine[i++] = Ch;
              }
          }

        /* Transport der unkomprimierten Zeile "PCXLine" in das Bild "img" */
        for (i = 0; i < xmax; i++)
          {
            ByteNbr = i >> Shift2;
            PosInByte = PixelPerByte - 1 - (i % PixelPerByte);
            Shift = (PosInByte << Shift1);
            val = (PCXLine[ByteNbr] & (MaxVal << Shift)) >> Shift;
            val = img->maxval - val * img->maxval / MaxVal;
            PutVal(img, i, j, val);
          }
      }

    free(PCXLine);
    fclose(fd);
    return img;
  }
#undef FNAME

  /***************************************************************/
#define FNAME "WriteImgPCX"
  int WritePCXImg(const Image& img, const string& fname)
  {
    FILE* fd;
    struct HeaderPCX Header;
    string hname = fname;
    int Shift1;
    int PixelPerByte;
    unsigned char* PCXLine;
    int PosInByte;
    int i, ii, j;
    unsigned char val, valh, MaxVal, Count, Ch;
    unsigned char* ColorTable;

    if (!IsImg(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }

    if ((fd = fopen(hname.c_str(), FWMODUS)) == NULL)
      {
        throw IceException(FNAME, "File kann nicht eroeffnet werden", ERROR);
      }

    Header.Signature = 0x0a; /* Kennung PCX-File */
    Header.Version = 5;
    Header.Encoding = 1; /*Bild RLE-Kodiert*/

    /* Bits pro Bildpunkt */

    if (img->maxval < 2)
      {
        Header.BitsPerPixel = 1;
        Shift1 = 0;
        MaxVal = 1;
      }
    else if (img->maxval < 4)
      {
        Header.BitsPerPixel = 2;
        Shift1 = 1;
        MaxVal = 3;
      }
    else if (img->maxval < 16)
      {
        Header.BitsPerPixel = 4;
        Shift1 = 2;
        MaxVal = 15;
      }
    else
      {
        Header.BitsPerPixel = 8;
        Shift1 = 3;
        MaxVal = 255;
      }

    PixelPerByte = 0x08 >> Shift1;

    Header.MinX = 0; /* Fensterkoordinaten */
    Header.MinY = 0;
    Header.MaxX = img->xsize - 1;
    Header.MaxY = img->ysize - 1;
    Header.Hdpi = 150; /* Aufloesung */
    Header.Vdpi = 150;

    /* Schreiben der Farbpalette*/
    for (i = 0; i < 48; i++)
      {
        Header.ColorMap[i] = 0;
      }

    if (MaxVal < 16)
      for (i = 0; i < (MaxVal + 1); i++)
        {
          Header.ColorMap[3 * i]  = (unsigned char)255 * i / MaxVal;
          Header.ColorMap[3 * i + 1] = (unsigned char)255 * i / MaxVal;
          Header.ColorMap[3 * i + 2] = (unsigned char)255 * i / MaxVal;
        }

    /* Ausgabe Farbpalette */
    Header.Nplanes = 1; /* Zahl der Farbebenen */
    /* Bytes pro Zeile (unkomprimiert)*/
    Header.BytesPerLine = img->xsize / PixelPerByte;

    if ((Header.BytesPerLine % PixelPerByte) > 0)
      {
        Header.BytesPerLine++;
      }

    if (Header.BytesPerLine % 2 == 1)
      {
        Header.BytesPerLine++;
      }

    /* Interpretation der Farbpalette */
    Header.PaletteInfo = 1;

    /* Schreiben des PCX-Headers in PCX-File */
    if (fwrite(&Header, 128, 1, fd) == 0)
      {
        throw IceException(FNAME, "Fehler beim Schreiben des Headers", ERROR);
      }

    PCXLine = (unsigned char*) malloc(Header.BytesPerLine * sizeof(unsigned char));

    for (j = 0; j < img->ysize; j++)
      {
        /* Ruecksetzen von PCX-Line */
        for (i = 0; i < Header.BytesPerLine; i++)
          {
            PCXLine[i] = 0;
          }

        /* Generierung der unkomprimierten PCX-Zeile */
        ii = 0;

        for (i = 0; i < img->xsize; i++)
          {
            val = MaxVal - GetVal(img, i, j) * MaxVal / img->maxval;
            PosInByte = i % PixelPerByte;
            valh = val << (7 - (PosInByte << Shift1) - ((1 << Shift1) - 1));
            PCXLine[i / PixelPerByte] = PCXLine[i / PixelPerByte] | valh;
          }

        /* Run-Length-Coding (RLE) */
        i = 0;

        while (i < Header.BytesPerLine)
          {
            Count = 1;

            if (i < Header.BytesPerLine - 1)
              {
                ii = i + 1;

                while ((Count < 0x3f) && (ii < Header.BytesPerLine) && (PCXLine[ii++] == PCXLine[i]))
                  {
                    Count = ii - i;
                  }
              }

            if (Count > 1 || ((PCXLine[i] & 0xc0) == 0xc0))
              {
                Ch = 0xc0 | Count;
                fwrite(&Ch, 1, 1, fd);
                fwrite(&PCXLine[i], 1, 1, fd);
              }
            else
              {
                if ((PCXLine[i] & 0xc0) == 0xc0)
                  {
                    Ch = 0xc1;
                    fwrite(&Ch, 1, 1, fd);
                    fwrite(&PCXLine[i], 1, 1, fd);
                  }
                else
                  {
                    fwrite(&PCXLine[i], 1, 1, fd);
                  }
              }

            i += Count;
          }
      }

    free(PCXLine);

    if (MaxVal == 255)
      {
        ColorTable = (unsigned char*) malloc(3 * 256 * sizeof(unsigned char));

        for (i = 0; i < 256; i++)
          {
            ColorTable[3 * i] = i;
            ColorTable[3 * i + 1] = i;
            ColorTable[3 * i + 2] = i;
          }

        fwrite(ColorTable, 128, 3, fd);
        free(ColorTable);
      }

    fclose(fd) ;
    return OK;
  }

#undef FNAME


  /************************************************************************/
#define FNAME "InfPCXFile"
  int InfPCXFile(const string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr)
  {
    FILE* fd;
    string hname = fname;
    struct HeaderPCX Header;

    xsize = 0;
    ysize = 0;
    maxval = 0;
    nr = 0;

    if ((fd = fopen(hname.c_str(), FRMODUS)) == NULL)
      {
        return FILE_NOT_FOUND;
      }

    fread((unsigned char*)&Header, 128, 1, fd);

    if (Header.Signature != 0x0a)
      {
        fclose(fd);
        return ERROR;
      }

    xsize = Header.MaxX - Header.MinX + 1;
    ysize = Header.MaxY - Header.MinY + 1;
    maxval = (1 << Header.BitsPerPixel) - 1;
    nr = 1;
    fclose(fd);
    return PCX;
  }
#undef FNAME
}
