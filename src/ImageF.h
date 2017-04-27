/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002..2014 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef __IMAGEF_H
#define __IMAGEF_H

#include <limits>

#include "defs.h"
#include "Window.h"

#include "matrixtemplate.h"

namespace ice
{

//----------------- class ImageF ----------------------------------------
  /**
   * matrix with reference count.
   * used in classes image as pixel memory
   * @tparam T type of elements (pixel)
   */
  template<typename T>
  class rcMatrix: public matrix<T>
  {
  public:
    rcMatrix() : matrix<T>(), refcount(0) {}
    /**
     * constructor with size and optional initialization.
     * A matrix of the given size is created and possibly initialized.
     * @param r number of rows of the matrix.
     * @param c number of columns of the matrix.
     * @param init mode of initialization.
     */
    rcMatrix(int r, int c, int init = -1) : matrix<T>(r, c, init), refcount(0) {}
    //private:
    int refcount;
  };

  /**
   * image type.
   * ImageF is a "reference" to a matrix with reference counter (=>rcMatrix)
   * so copying an ImageF is copying a reference, not the image
   * do not use as base class for derivation. Methods are not virtual for performance reasons
   * @tparam T storage type of pixels
   */
  template<typename T>
  class ImageF
  {
    typedef void (*DestroyFPtr)(ImageF<T>*);

  public:
    typedef T ValueType;

    /**
    * standard constructor, no real image yet.
    */
    ImageF() : mat(nullptr), xsize(0), ysize(0), nVis(0), dfp(nullptr) {};

    /**
     * copy constructor.
     * *this and i point to the same image (shallow copy)
     */
    ImageF(const ImageF& i): xsize(i.xsize), ysize(i.ysize),
      minval(i.minval), maxval(i.maxval), dfp(nullptr)
    {
      assign(i.mat);
    }

    /**
     * "named constructor" which creates real image.
     * @param SizeX, SizeY size of image\n
     * note:
     * order of parameters x,y == nCols,nRows is reverse to matrix (nRows,nCols)
     * @param min,max range of values\n
     * min and max are informative only. Values are not testet
     *
     */
    static ImageF createImage(int SizeX, int SizeY,
                              T min = std::numeric_limits<T>::lowest(),
                              T max = std::numeric_limits<T>::max())
    {
      ImageF result;
      result.create(SizeX, SizeY, min, max);
      return result;
    }

    /**
     * "named constructor" which creates real image from template image.
     * creates an image with same size and limits
     * @param copy - copy content of image
     */
    static ImageF createImage(const ImageF& src, bool copy = false)
    {
      ImageF result;
      result.create(src, copy);
    }

    /**
     * method to [re]create image, old content is lost.
     * @param SizeX, SizeY size of image\n
     * note:
     * order of parameters x,y == nCols,nRows is reverse to matrix (nRows,nCols)
     * @param min,max range of values\n
     * min and max are informative only. Values are not testet
    */

    void create(int SizeX, int SizeY,
                T min = std::numeric_limits<T>::lowest(),
                T max = std::numeric_limits<T>::max())
    {
      if (mat != nullptr)
        {
          freemat();
        }

      xsize = SizeX;
      ysize = SizeY;
      maxval = max;
      minval = min;

      rcMatrix<T>* mat = new rcMatrix<T>(SizeY, SizeX);
      assign(mat);
    }

    /**
     * method to [re]create image, old content is lost.
     * creates an image with same size and limits
     * @param copy - copy content of image
     */
    void create(const ImageF<T>& templateImg, bool shallCopy = false)
    {
      create(templateImg.xsize, templateImg.ysize,
             templateImg.minval, templateImg.maxval);
      if (shallCopy)
        {
          copy(templateImg);
        }
    }

    /**
     * method to [re]create image, old content is lost.
     * template is a matrix like structure which has rows() and cols() to get sizes
     */
    template<typename Tt>
    void create(const Tt& templateImg)
    {
      create(templateImg.cols(), templateImg.rows());
    }

    /**
     * method to get central point of image.
     */
    IPoint center() const
    {
      return IPoint(xsize / 2, ysize / 2);
    }

    /**
     * method to get size in x direction == number of columns.
     */
    int cols() const
    {
      return xsize;
    }

    /**
     * method to get size in y direction == number of rows.
     */
    int rows() const
    {
      return ysize;
    }

    /**
     * method to get pointer to row of matrix.
     * can be used as image[x][y] to access pixel
     */
    T* operator[](int i)
    {
      return (*mat)[i];
    }

    /**
     * const method to get pointer to const row of matrix.
     * can be used as image[x][y] to access pixel
     */
    const T* operator[](int i) const
    {
      return (*mat)[i];
    }

    /**
     * copy of an image.
     * this is the same as recreation with size of given image and deep copy of source data
     */
    void copy(const ImageF& src)
    {
      create(src);
      *mat = *src.mat;
    }

    /**
     * destructor
     */
    ~ImageF()
    {
      freemat();
    }

    /**
     * is image valid.
     * image is valid and can be used if there is an matrix allocated.
     * the standard constructor creates an invalid image
     */
    bool isValid() const
    {
      return mat != nullptr;
    }

    /**
     * get the matrix as reference
     */
    rcMatrix<T>& getMatrix()
    {
      return *mat;
    }

    const rcMatrix<T>& getMatrix() const
    {
      return *mat;
    }

    /**
     * get the matrix as reference
     */
    const rcMatrix<T>* getMatrixPointer() const
    {
      return mat;
    }

    rcMatrix<T>* getMatrixPointer()
    {
      return mat;
    }

    /**
     * copy assignment operator - this is the copy of the reference
     */
    const ImageF& operator = (const ImageF& i)
    {
      if (&i == this)
        {
          return *this;
        }

      freemat();
      xsize = i.xsize;
      ysize = i.ysize;
      assign(i.mat);
      return *this;
    }

    /**
     * operator== - object identity.
     * compares if the same matrix is associated with the images.
     * after image2 = image1 is  image1 == image2 true
     */
    bool operator == (const ImageF& rhs) const
    {
      return mat == rhs.mat;
    }

    /**
     * operator== - object inequility.
     * compares if the same matrix is associated with the images.
     * after image2 = image1 is  image1 != image2 false
     */
    bool operator != (const ImageF& rhs) const
    {
      return mat != rhs.mat;
    }

    /*
     * pixel access
     */
#define FNAME "getPixel"
    double getPixel(int x, int y) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);
      return (*mat)[y][x];
    }

    double getPixel(IPoint p) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)p.y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);
      return (*mat)[p.y][p.x];
    }
#undef FNAME
#define FNAME "getPixelClipped"
    double getPixelClipped(int x, int y) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        {
          return 0;
        }

      if ((unsigned int)y >= (unsigned int)ysize)
        {
          return 0;
        }
      return (*mat)[y][x];
    }

    double getPixelClipped(IPoint p) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        {
          return 0;
        }

      if ((unsigned int)p.y >= (unsigned int)ysize)
        {
          return 0;
        }
      return (*mat)[p.y][p.x];
    }
#undef FNAME

    double getPixelUnchecked(int x, int y) const
    {
      return (*mat)[y][x];
    }

    double getPixelUnchecked(IPoint p) const
    {
      return (*mat)[p.y][p.x];
    }

#define FNAME "setPixel"
    void setPixel(int x, int y, double val) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      (*mat)[y][x] = val;
    }

    void setPixel(IPoint p, double val) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)p.y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      (*mat)[p.y][p.x] = val;
    }

    void setPixelClipped(IPoint p, double value) const
    {
      if ((unsigned int)p.x < (unsigned int)xsize)
        if ((unsigned int)p.y < (unsigned int)ysize)
          {
            (*mat)[p.y][p.x] = value;
          }
    }

    void setPixelClipped(int x, int y, double value) const
    {
      if ((unsigned int)x < (unsigned int)xsize)
        if ((unsigned int)y < (unsigned int)ysize)
          {
            (*mat)[y][x] = value;
          }
    }

#undef FNAME

    void setPixelUnchecked(int x, int y, double val) const
    {
      (*mat)[y][x] = val;
    }

    void setPixelUnchecked(IPoint p, double val) const
    {
      (*mat)[p.y][p.x] = val;
    }

    double getPixelInterpol(double x, double y) const
    {
      double val;
      if (getPixelInterpol(x, y, val))
        {
          return val;
        }
      else
        {
          return 0.0;
        }
    }

    bool getPixelInterpol(double x, double y, double& val) const
    {
      // if x and y are to far outside the image border, we simply return 0.0
      if (x < -0.5 || y < -0.5)
        {
          val = 0.0;
          return false;
        }

      // Determine the local neighborhood of point (x, y), that means the 4 pixel positions
      // (xi, yi), (xi + 1, yi), (xi + 1, yi + 1), and (xi, yi + 1), that enclose (x, y)
      x = std::max<double> (x, 0.0);
      y = std::max<double> (y, 0.0);
      int xi  = (int) x;
      int xi1 = xi + 1;
      int yi  = (int) y;
      int yi1 = yi + 1;

      if ((xi >= xsize) || (yi >= ysize))
        {
          val = 0.0;
          return false;
        }

      if ((xi1 == xsize) || (yi1 == ysize))
        {
          return (*mat)[yi][xi];  // rechter/unterer Rand
        }

      double dx  = x - (double) xi;
      double dx1 = 1.0 - dx;
      double dy  = y - (double) yi;
      double dy1 = 1.0 - dy;
      val = dx1 * (dy1 * (*mat)[yi][xi] +
                   dy * (*mat)[yi1][xi])
            +  dx  * (dy1 * (*mat)[yi][xi1] +
                      dy * (*mat)[yi1][xi1]);
      return true;
    }

    // check if point coordinate is inside of an image
    // code doubling for efficiency purposes !!
    bool inside(int x, int y) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        {
          return false;
        }
      if ((unsigned int)y >= (unsigned int)ysize)
        {
          return false;
        }
      return true;
    }

    bool inside(IPoint p) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        {
          return false;
        }
      if ((unsigned int)p.y >= (unsigned int)ysize)
        {
          return false;
        }
      return true;
    }

    bool inside(const Point& p) const
    {
      if (p.x < -0.5)
        {
          return false;
        }
      if (p.y < -0.5)
        {
          return false;
        }
      if (p.x >= xsize - 0.5)
        {
          return false;
        }
      if (p.y >= ysize - 0.5)
        {
          return false;
        }
      return true;
    }

    // Check if given window is inside an image
    bool inside(const Window& window) const
    {
      return inside(window.p1) && inside(window.p2);
    }

    void set(T val) const
    {
      return mat->set(val);
    }

    // set call back function, that is called, if the image is destroyed
    // used by visualisation to switch off before the image is gone

    void startVis(DestroyFPtr af)
    {
      nVis++;
      dfp = af;
    }

    void stopVis()
    {
      nVis++;
      if (nVis == 0)
        {
          dfp = nullptr;
        }
    }

    /* does the callback */
    void destroy()
    {
      if (dfp != nullptr)
        {
          (*dfp)(this);
        }

      dfp = nullptr;
      nVis = 0;
      freemat();
    }

  protected:
    rcMatrix<T>* mat;

  public:
    int xsize, ysize;
    T minval, maxval;

  protected:
    int nVis;
    // pointer to the registered call back function
    // must be set to nullptr by constructor
    DestroyFPtr dfp;
    int* tsp;

// free the reference,
// delete image if no other reference exist

    void freemat()
    {
      if (mat != nullptr)
        {
          mat->refcount--;

          if (mat->refcount <= 0)
            {
              delete mat;
            }
          mat = nullptr;
        }
    }

// assign new image,
// count reference
    void assign(rcMatrix<T>* i)
    {
      mat = i;

      if (mat != nullptr)
        {
          mat->refcount++;
        }
    }
  };
}
#endif // #ifndef __IMAGEF_H
