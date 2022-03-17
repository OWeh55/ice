/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __IMAGEC_H
#define __IMAGEC_H

#include <string>

#include "Window.h"
#include "ImageBase.h"

namespace ice
{
//----------------- class image ----------------------------------------
  /**
   * Image is the class typically used to represent images.
   * The class organizes an appropriate memory and allows a
   * visualisation with Show().\n
   * Pixel values are integers. Methods accept int as parameter
   * and return int as pixel value. The internal storage
   * is adjusted to the maximum value and is often a
   * smaller integer type.\n
   * Instances of Image are a kind of reference to the real
   * memory to store pixel data (ImageBase). Therefore
   * copying an Image by copy constructor
   * or assignment is a shallow copy and the result are two
   * references to the same image data. For a deep copy use
   * methods like copy.
   */
  class Image final
  {
  public:
    typedef int ValueType;

    /**
    * standard constructor.
    * There is no real image yet, the image is not valid.
    * It must be created by create or other methods.
    * @see create
    * @see isValid
    */
    Image(): img(nullptr), xsize(0), ysize(0), maxval(0) {};

    /**
    * copy constructor.
    * *this and i point to the same image (shallow copy)
    */
    Image(const Image& i): xsize(i.xsize), ysize(i.ysize), maxval(i.maxval)
    {
      assign(i.img);
    }

    /**
     * "named constructors" which create a real image
     * @param SizeX, SizeY extension of image (in pixel)
     * @param maxValue maximum of pixel values\n
     *     used for parameter tests and selection of storage type
     * @param title name of image used in visualization
     */
    static Image createImage(int SizeX, int SizeY, int maxValue = 255,
                             const std::string& title = "");

    /**
     * "named constructors" which create a real image
     * @param src image, that ist used as "template"\n
     *  new image has the same sizes and maximum values as src.
     * @param title name of image used in visualization
     */
    static Image createImage(const Image& src,
                             const std::string& title = "");

    /*
     * method to [re]create image, old content is lost.
     * image parameters are given.
     * @param SizeX, SizeY extension of image (in pixel)
     * @param maxValue maximum of pixel values\n
     *     used for parameter tests and selection of storage type
     */

    void create(int SizeX, int SizeY, int maxValue = 255, const std::string& title = "");

    /*
     * method to [re]create image, old content is lost
     * image parameters are given by a template image
     * @param src image, that ist used as "template"\n
     *  new image has the same sizes and maximum values as src.
     * @param title name of image used in visualization
     */

    void create(const Image& templateImg, const std::string& title = "");

    /**
     *  recreate with size of given image and deep copy of source data\n
     *  works like create(src, true);
     */
    void copy(const Image& src);

    /**
     *  invert all values of image\n
     */
    void invert()
    {
      img->invert();
    }

    /**
    * subimage constructor.
    * subimages are references to a part of another image (img).
    * @param img parent image
    * @param window describes part of parent to use as subimage
    * @param title name of image used in visualization
    */
    Image(const Image& img, const Window& window,
          const std::string& title = "");

    /**
    * subimage method.
    * returns a subimage.
    * @param w  part window to use as subimage
    * @param title name of image used in visualization
    */
    Image operator()(const Window& w, const std::string& title = "") const
    {
      return Image(*this, w, title);
    }

    /**
     * destructor.
     */
    ~Image()
    {
      freeimg();
    }

    /**
     * free image memory.
     */
    void destroy()
    {
      freeimg();
    }

    /**
     * is the Image valid.
     * An Image is valid, if the image memory (ImageBase) is assigned using
     * create(), copy(), createImage...
     */
    bool isValid() const
    {
      return img != nullptr;
    }

    /**
     * copy content of image.
     * *this and source must exist and have the same sizes.
     */
    void copyData(const Image& source) const
    {
      img->copyData(source.img);
    }

    /**
     * get title of Image.
     */
    const std::string& getTitle() const
    {
      return img->getTitle();
    }

    /**
     * set title of Image.
     * This may reflect different use of image and is not use by the system.
     */
    void setTitle(const std::string& title)
    {
      img->setTitle(title);
    }

    /**
    * is Image a subimage.
    */
    bool isSubImage() const
    {
      return img->isSubImage();
    }

    /**
     * center of Image.
     * The central point of an Image can be important if used for fourier spectrum etc.
     * This is calculated by integer division of xSize and ySize by 2.
     */
    IPoint center() const
    {
      return IPoint(xsize / 2, ysize / 2);
    }

    /**
     * number of columns ==  xsize.
     * this makes Image compatible to other "matrix like" two-dimensional structures.
     */
    int cols() const
    {
      return xsize;
    }

    /**
     * number of rows ==  ysize.
     * this makes Image compatible to other "matrix like" two-dimensional structures.
     */
    int rows() const
    {
      return ysize;
    }

    /**
     * assignment operator:
     * this is a copy of the reference
     */
    const Image& operator = (const Image& i)
    {
      if (&i == this)
        {
          return *this;
        }

      freeimg();
      xsize = i.xsize;
      ysize = i.ysize;
      maxval = i.maxval;
      assign(i.img);
      return *this;
    }

    /**
    * access to BaseImage.
    */
    ImageBase* operator ->() const
    {
      return img;
    }

    /**
    * access to BaseImage.
    */
    ImageBase* Img() const
    {
      return img;
    }

    /**
    * access to BaseImage.
    */
    ImageBase& operator *() const
    {
      return *img;
    }

    /**
     * test object identity.
     * Is true if both Images refer the same ImageBase.
     */
    bool operator == (const Image& rhs) const
    {
      return img == rhs.img;
    }

    /**
     * test object identity.
     * Is false if both Images refer the same ImageBase.
     */
    bool operator != (const Image& rhs) const
    {
      return img != rhs.img;
    }

#define FNAME "getPixel"
    //@{
    /**
     * get pixel.
     * Get the value of pixel at given coordinates. The
     * parameters (coordinates) are fully checked.
     * Coordinates outside image raise an error
     */
    int getPixel(int x, int y) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      return img->getP(x, y);
    }

    int getPixel(IPoint p) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)p.y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      return img->getP(p.x, p.y);
    }
    //@}
#undef FNAME
#define FNAME "getPixelClipped"
    //@{
    /**
     * get pixel value.
     * Get the value of pixel at given coordinates. If
     * the coordinates are outside of the image the method
     * return 0.
     */
    int getPixelClipped(int x, int y) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        {
          return 0;
        }

      if ((unsigned int)y >= (unsigned int)ysize)
        {
          return 0;
        }
      return img->getP(x, y);
    }

    int getPixelClipped(IPoint p) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        {
          return 0;
        }

      if ((unsigned int)p.y >= (unsigned int)ysize)
        {
          return 0;
        }
      return img->getP(p.x, p.y);
    }
#undef FNAME
//@}
    //@{
    /**
     * get pixel value without any parameter test.
     * this is the fastest method to read the value, but
     * the caller must make sure, that coordinates are valid.
     */
    int getPixelUnchecked(int x, int y) const
    {
      return img->getP(x, y);
    }

    int getPixelUnchecked(IPoint p) const
    {
      return img->getP(p.x, p.y);
    }
    //@}
#define FNAME "setPixel"
    //@{
    /**
     * set pixel to value.
     * parameters (coordinates and value) are fully checked
     * coordinates outside of the image or values out of range raise an error
     */
    void setPixel(int x, int y, int val) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      if ((unsigned int)val > (unsigned int)maxval)
        throw IceException(FNAME, M_VAL_OUT_OF_RANGE);

      img->setP(x, y, val);
    }

    void setPixel(IPoint p, int val) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)p.y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      if ((unsigned int)val > (unsigned int)maxval)
        throw IceException(FNAME, M_VAL_OUT_OF_RANGE);

      img->setP(p.x, p.y, val);
    }
    //@}
#undef FNAME
#define FNAME "Image::setPixelClipped"
    //@{
    /**
     * set pixel value ignoring pixels outside the image and values out of range
     * The parameters (coordinates and value) are fully checked,
     * nothing is done in case of coordinates outside of the image
     * or values out of range
     */

    void setPixelClipped(IPoint p, int value) const
    {
      if ((unsigned int)p.x < (unsigned int)xsize)
        if ((unsigned int)p.y < (unsigned int)ysize)
          if ((unsigned int)value <= (unsigned int)maxval)
            {
              img->setP(p.x, p.y, value);
            }
    }

    void setPixelClipped(int x, int y, int value) const
    {
      if ((unsigned int)x < (unsigned int)xsize)
        if ((unsigned int)y < (unsigned int)ysize)
          if ((unsigned int)value <= (unsigned int)maxval)
            {
              img->setP(x, y, value);
            }
    }
    //@}
#undef FNAME
#define FNAME "setPixelLimited"
    //@{
    /**
     * set pixel value limiting values too high or too low.
     * The parameters (coordinates and value) are fully checked.
     * Coordinates outside of the image raise an error.
     * Pixel values out of range are limited to the range of the image values.
     */
    void setPixelLimited(int x, int y, int val) const
    {
      if ((unsigned int)x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      if (val <= 0)
        {
          img->setP(x, y, 0);
        }
      else if (val >= maxval)
        {
          img->setP(x, y, maxval);
        }
      else
        {
          img->setP(x, y, val);
        }
    }

    void setPixelLimited(IPoint p, int val) const
    {
      if ((unsigned int)p.x >= (unsigned int)xsize)
        throw IceException(FNAME, M_X_OUT_OF_RANGE);

      if ((unsigned int)p.y >= (unsigned int)ysize)
        throw IceException(FNAME, M_Y_OUT_OF_RANGE);

      if (val <= 0)
        {
          img->setP(p.x, p.y, 0);
        }
      else if (val >= maxval)
        {
          img->setP(p.x, p.y, maxval);
        }
      else
        {
          img->setP(p.x, p.y, val);
        }
    }
    //@}
#undef FNAME
    //@{
    /**
     * set pixel values without parameter testing.
     * This is the fastest method to write the value, but
     * the caller must make sure, that coordinates are valid.
     */

    void setPixelUnchecked(int x, int y, int val) const
    {
      img->setP(x, y, val);
    }

    void setPixelUnchecked(IPoint p, int val) const
    {
      img->setP(p.x, p.y, val);
    }
    //@}

    /**
     * get value interpolated.
     * Returns an interpolated value for floating point coordinates
     * @param x, y coordinates
     * @return interpolated value
     */
    double getPixelInterpol(double x, double y) const;

    /**
     * get value interpolated.
     * Returns an interpolated value for Point (with floating point coordinates)
     * @param p coordinates
     * @return interpolated value
     */
    double getPixelInterpol(Point p) const;

    /**
     * get value interpolated.
     * Returns an interpolated value for floating point coordinates
     * @param x, y coordinates
     * @param val variable to store interpolated value
     * @return lies (x,y) inside image
     */
    bool getPixelInterpol(double x, double y, double& val) const;

    //@{
    /**
     * get pixel value as intensity.
     * Get the value of pixel at given coordinates as intensity.
     * This is equivalent to Image.maxval - Image.getPixel(..).
     * The parameters (coordinates) are fully checked.
     * Coordinates outside image raise an error
     */

    int getIntensity(int x, int y) const
    {
      return maxval - getPixel(x, y);
    }

    int getIntensity(IPoint p) const
    {
      return maxval - getPixel(p.x, p.y);
    }
    //@}

    //@{
    /**
     * get pixel value as intensity without any parameter test.
     * This is equivalent to Image.maxval - Image.getPixelUnchecked(..).
     * this is the fastest method to read the value, but
     * the caller must make sure, that coordinates are valid.
     */

    int getIntensityUnchecked(int x, int y) const
    {
      return maxval - getPixelUnchecked(x, y);

    }

    int getIntensityUnchecked(IPoint p) const
    {
      return maxval - getPixel(p.x, p.y);
    }
    //@}

    //@{
    /**
     * set pixel to intensity.
     * This is equivalent to Image.setPixel(x, y, Image.maxval - val).
     * parameters (coordinates and value) are fully checked
     * coordinates outside of the image or values out of range raise an error
     */
    void setIntensity(int x, int y, int val) const
    {
      setPixel(x, y, maxval - val);
    }

    void setIntensity(IPoint p, int val) const
    {
      setPixel(p.x, p.y, maxval - val);
    }
    //@}

    //@{
    /**
     * set pixel to intensity without any parameter test.
     * This is equivalent to Image.setPixelUnchecked(x, y, Image.maxval - val).
     * this is the fastest method to read the value, but
     * the caller must make sure, that coordinates are valid.
     */

    void setIntensityUnchecked(int x, int y, int val) const
    {
      setPixelUnchecked(x, y, maxval - val);
    }

    void setIntensityUnchecked(IPoint p, int val) const
    {
      setPixelUnchecked(p.x, p.y, maxval - val);
    }
    //@}

    //@{
    /**
     * is point with given integer coordinates inside image.
     */
    bool inside(int x, int y) const
    {
      return ((unsigned int)x < (unsigned int)xsize) && ((unsigned int)y < (unsigned int)ysize);
    }

    bool inside(IPoint p) const
    {
      return ((unsigned int)p.x < (unsigned int)xsize) && ((unsigned int)p.y < (unsigned int)ysize);
    }
    //@}

    /**
     * is Point p inside image.
     */
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

    /**
     *  Check if given window lies fully inside an image
     */
    bool inside(const Window& window) const
    {
      return inside(window.p1) && inside(window.p2);
    }

    /**
     * set all pixels to the given value.
     */
    int set(int val) const
    {
      return img->set(val);
    }

    /**
     * read image from file.
     */
    int read(const std::string& filename);

    /**
     * write image to file.
     */
    int write(const std::string& filename) const;

    //@{
    /**
     * make sure, the sizes of the images match.
     * throws an exception if sizes do not match
     */
    void checkSizes(const Image& img2) const;
    /**
     * make sure, the sizes and value ranges of the images match.
     * throws an exception if sizes do not match
     */
    void checkImage(const Image& img2) const;
    //@}

  protected:
    ImageBase* img; //*< Pointer to data (ImageBase). */

  public:
    int xsize, ysize; //*< width and height of Image in pixel. */
    int maxval;      //*< maximum value of pixel. */

  protected:
    /*
     * reference counting
     */

    /**
     * free this Image(reference).
     * delete image if no other reference exist.
     */

    void freeimg()
    {
      if (img != nullptr)
        {
          img->refcount--;

          if (img->refcount <= 0)
            {
              delete img;
            }
          img = nullptr;
        }
    }

    /**
     * *this is a new reference to image i
     */
    void assign(ImageBase* i)
    {
      img = i;

      if (img != nullptr)
        {
          img->refcount++;
        }
    }
  };

  // functions to create new Images
  // these functions are (more or less) wrappers for the
  // "named constructors" createImage(..)

  /**
   * Create new Image with given Size and MaxValue
   */
  Image NewImg(int SizeX, int SizeY, int MaxValue = 255,
               const std::string& title = "");

  /**
   * create a new image that has the same size as img.
   * if shallCopyContents is true the pixel data of img are copied
   */
  Image NewImg(const Image& img,
               bool shallCopyContents = false,
               const std::string& title = "");

  /**
   * create a subimage as a part of image img
   */
  Image NewImg(const Image& Img,
               const Window& w,
               const std::string& title = "");

}
#endif // #ifndef __IMAGEC_H
