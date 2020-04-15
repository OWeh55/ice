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
/* jpegio.cpp  */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

extern "C" {
#include <jpeglib.h>
}

#include <setjmp.h>

#include "defs.h"
#include "base.h"
#include "macro.h"
#include "IceException.h"
#include "exfile.h"
#include "picio.h"

using namespace std;

namespace ice
{
  /* error handling for libjpeg */

  /*
   * ERROR HANDLING:
   *
   * The JPEG library's standard error handler (jerror.c) is divided into
   * several "methods" which you can override individually.  This lets you
   * adjust the behavior without duplicating a lot of code, which you might
   * have to update with each future release.
   *
   * Our example here shows how to override the "error_exit" method so that
   * control is returned to the library's caller when a fatal error occurs,
   * rather than calling exit() as the standard error_exit method does.
   *
   * We use C's setjmp/longjmp facility to return control.  This means that the
   * routine which calls the JPEG library must first execute a setjmp() call to
   * establish the return point.  We want the replacement error_exit to do a
   * longjmp().  But we need to make the setjmp buffer accessible to the
   * error_exit routine.  To do this, we make a private extension of the
   * standard JPEG error handler object.  (If we were using C++, we'd say we
   * were making a subclass of the regular error handler.)
   *
   * Here's the extended error handler struct:
   */

  struct my_error_mgr
  {
    struct jpeg_error_mgr pub;  /* "public" fields */
    jmp_buf setjmp_buffer;  /* for return to caller */
  };

  typedef struct my_error_mgr* my_error_ptr;

  /*
   * Here's the routine that will replace the standard error_exit method:
   */

  METHODDEF(void)
  my_error_exit(j_common_ptr cinfo)
  {
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* message is written to stderr */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
  }

#define FNAME "WriteJPEGImg"
  int WriteJPEGImg(const Image& ir, const Image& ig, const Image& ib, const string& fname)
  {
    int xs, ys;
    int gm;

    if (!(IsImg(ir) && IsImg(ig) && IsImg(ib)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    xs = ir->xsize;
    ys = ir->ysize;
    gm = ir.maxval;

    if (
      (xs != ig->xsize) || (xs != ib->xsize) ||
      (ys != ig->ysize) || (ys != ib->ysize) ||
      (gm != ig.maxval) || (gm != ib.maxval)
    )
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    /* Select the output file.
     */

    FILE* output_file;

    if ((output_file = fopen(fname.c_str(), FWMODUS)) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    // create data structures
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
      {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_compress(&cinfo);
        fclose(output_file);
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, output_file);

    // set image/compression parameters
    cinfo.image_width = xs;
    cinfo.image_height = ys;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, 95, TRUE); // we cannot use bool here, because of jpeglib

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    row_pointer[0] = new JSAMPLE[xs * 3];
    int x, y;
    int gnorm = gm + 1;
    int jnorm = 1 << BITS_IN_JSAMPLE;
    int jm = jnorm - 1;

    for (y = 0; y < ys; y++)
      {
        int i = 0;

        for (x = 0; x < xs; x++)
          {
            if (gnorm == jnorm)
              {
                row_pointer[0][i] = jm - GetVal(ir, x, y);
                i++;
                row_pointer[0][i] = jm - GetVal(ig, x, y);
                i++;
                row_pointer[0][i] = jm - GetVal(ib, x, y);
                i++;
              }
            else
              {
                row_pointer[0][i] = jm - GetVal(ir, x, y) * jnorm / gnorm;
                i++;
                row_pointer[0][i] = jm - GetVal(ig, x, y) * jnorm / gnorm;
                i++;
                row_pointer[0][i] = jm - GetVal(ib, x, y) * jnorm / gnorm;
                i++;
              }
          }

        jpeg_write_scanlines(&cinfo, row_pointer, 1);
      }

    jpeg_finish_compress(&cinfo);

    fclose(output_file);
    delete [] row_pointer[0];

    jpeg_destroy_compress(&cinfo);

    return OK;
  }

  int WriteJPEGImg(const Image& img, const string& fname)
  {
    int xs, ys;
    int gm;

    if (! IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    xs = img->xsize;
    ys = img->ysize;
    gm = img.maxval;

    /* Select the output file.
     */

    FILE* output_file;

    if ((output_file = fopen(fname.c_str(), FWMODUS)) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    // create data structures
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
      {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_compress(&cinfo);
        fclose(output_file);
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, output_file);

    // set image/compression parameters
    cinfo.image_width = xs;
    cinfo.image_height = ys;
    cinfo.input_components = 1;
    cinfo.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo);

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    row_pointer[0] = new JSAMPLE[xs];
    int x, y;
    int gnorm = gm + 1;
    int jnorm = 1 << BITS_IN_JSAMPLE;
    int jm = jnorm - 1;

    for (y = 0; y < ys; y++)
      {
        int i = 0;

        for (x = 0; x < xs; x++)
          {
            if (gnorm == jnorm)
              {
                row_pointer[0][i] = jm - GetVal(img, x, y);
                i++;
              }
            else
              {
                row_pointer[0][i] = jm - GetVal(img, x, y) * jnorm / gnorm;
                i++;
              }
          }

        jpeg_write_scanlines(&cinfo, row_pointer, 1);
      }

    jpeg_finish_compress(&cinfo);

    fclose(output_file);
    delete [] row_pointer[0];

    jpeg_destroy_compress(&cinfo);

    return OK;
  }
#undef FNAME
#define FNAME "InfJPEGFile"
  int InfJPEGFile(const string& fname, int& xsize, int& ysize,
                  int& maxval, int& nr)
  {

    FILE* infile;

    if ((infile = fopen(fname.c_str(), FRMODUS)) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    struct jpeg_decompress_struct cinfo;

    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);

    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
      {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);

    xsize = cinfo.image_width;
    ysize = cinfo.image_height;
    maxval = (1 << BITS_IN_JSAMPLE) - 1;

    if (cinfo.num_components == 1)
      {
        nr = 1;
      }
    else
      {
        nr = 3;
      }

    fclose(infile);
    jpeg_destroy_decompress(&cinfo);
    return OK;
  }
#undef FNAME
#define FNAME "ReadJPEGImg"
  int ReadJPEGImg(const string& fname,
                  Image& imgr, Image& imgg, Image& imgb,
                  int flag)
  {
    ibuffer ib;
    ib.data = nullptr;

    FILE* infile;

    if ((infile = fopen(fname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    struct jpeg_decompress_struct cinfo;

    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);

    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
      {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);

        if (ib.data != nullptr)
          {
            free(ib.data);
          }

        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);

    ib.width = cinfo.image_width;
    ib.height = cinfo.image_height;
    ib.maxval = (1 << BITS_IN_JSAMPLE) - 1;

    ib.planes = cinfo.num_components;

    if (ib.maxval > 255)
      {
        ib.valuesize = sizeof(int);
      }
    else
      {
        ib.valuesize = 1;
      }

    ib.byteorder = IB_LSB_FIRST;
    ib.linelength = ib.planes * ib.width * ib.valuesize;
    ib.packmethod = IB_RGB;
    ib.intensity = true;

    ib.data = (unsigned char*) malloc(ib.linelength * ib.height);
    ib.can_delete = true;

    jpeg_start_decompress(&cinfo);

    for (int i = 0; i < ib.height; i++)
      {
        JSAMPROW row_pointer[1];
        row_pointer[0] = ((unsigned char*)ib.data) + ib.linelength * i;
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
      }

    fclose(infile);
    jpeg_destroy_decompress(&cinfo);

    if (!IsImg(imgr))
      {
        imgr = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(imgg))
      {
        imgb = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(imgb))
      {
        imgb = NewImg(ib.width, ib.height, ib.maxval);
      }

    Buffer2Image(ib, imgr, imgg, imgb, flag);
    return OK;
  }

  Image ReadJPEGImg(const string& fname, Image& img, int flag)
  {
    ibuffer ib;
    ib.data = nullptr;

    FILE* infile;

    if ((infile = fopen(fname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    struct jpeg_decompress_struct cinfo;

    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);

    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer))
      {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);

        if (ib.data != nullptr)
          {
            free(ib.data);
          }

        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_GRAYSCALE;

    ib.width = cinfo.image_width;
    ib.height = cinfo.image_height;
    ib.maxval = (1 << BITS_IN_JSAMPLE) - 1;
    ib.planes = 1;

    if (ib.maxval > 255)
      {
        ib.valuesize = sizeof(int);
      }
    else
      {
        ib.valuesize = 1;
      }

    ib.byteorder = IB_LSB_FIRST;
    ib.linelength = ib.planes * ib.width * ib.valuesize;
    ib.packmethod = IB_RGB;
    ib.intensity = true;

    ib.can_delete = true;
    ib.data = (unsigned char*) malloc(ib.linelength * ib.height);

    jpeg_start_decompress(&cinfo);

    for (int i = 0; i < ib.height; i++)
      {
        JSAMPROW row_pointer[1];
        row_pointer[0] = ((unsigned char*)ib.data) + ib.linelength * i;
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
      }

    fclose(infile);
    jpeg_destroy_decompress(&cinfo);

    if (!IsImg(img))
      {
        img = NewImg(ib.width, ib.height, ib.maxval);
      }

    img = Buffer2Image(ib, img, flag);
    return img;
  }

} // namespace ice
