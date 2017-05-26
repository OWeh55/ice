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
#ifndef __IMAGEBASE_H
#define __IMAGEBASE_H

#include <string>
#include <list>
#include <limits>

#include "defs.h"
#include "Window.h"

namespace ice
{
  class Image;

  /**  Abstract base class for all kind of images.
    This class handles all methods for images, which may abstract from
    special datatype of pixels.
    You will typically use Image instead of ImageBase in applications.
    Note, that ImageBase is "the real thing", not only a
    kind of reference like Image.
  */
  class ImageBase
  {
  public:
    /** is this a sub image.
      if the image is a subimage it refers to a part of another image
    */
    virtual bool isSubImage() const
    {
      return parent != NULL;
    }

    /**
     * width of the image in pixels.
     * this variable is public for compatibility issues
     */
    const int xsize;

    /**
     * height of the image in pixels.
     * this variable is public for compatibility issues
     */
    const int ysize;

    /**
     * maximum value of the image.
     * this variable is public for compatibility issues
     */
    const int maxval;

  public:
    /**
     * get the title of the image.
     * The title is a string and used for display
     */
    const std::string& getTitle() const
    {
      return title;
    }

    /**
     * set the title of the image.
     * @param title new title of the image
     */
    void setTitle(const std::string& title)
    {
      this->title = title;
    }

    //------------------------------------------------------------

    /**
     * set values for a pixel at (posX, posY)
     * this is totally unchecked !!
     * @param posX, posY coordinates
     */
    virtual void setP(int PosX, int PosY,
                      int Value) = 0;

    /**
     * get values for a pixel at (posX, posY)
     * this is totally unchecked !!
     * @param posX, posY coordinates
     */
    virtual int getP(int PosX, int PosY) const = 0;

    // functions for organizing low level access to image data
    /**
     * Get type of image.
     */
    virtual int ImageType() const = 0;

    /**
     * Get the number of bytes used to store one intensity value.
     */
    virtual int getBytesPerPoint() const = 0;

    /**
     * Get a pointer to the pixel data block
     */
    virtual void* getDataPtr() = 0;
    /**
     * Get a pointer to the pixel data block
     */
    virtual const void* getDataPtr() const = 0;

    /**
     * Set all image values.
     * @param grayvalue value each pixel is set to.
     */
    virtual int set(int grayvalue);

    /**
     * invert all image values.
     */
    virtual int invert();

    /**
     * Set all image values to 0.
     */
    virtual void clear()
    {
      set(0);
    };

  protected:
    /**
     * Constructor.
     * Constructor and destructor are made inaccesible,
     * so that images may be constructed only via friend class Image
     */
    ImageBase(int SizeX, int  SizeY,
              int MaxIntensity,
              const std::string& title, ImageBase* parent);
    /**
     * Destructor.
     * Constructor and destructor are made inaccesible,
     * so that images may be constructed only via friend class Image
     */
    virtual ~ImageBase();

    /**
     * Copies the whole pixel array from source.
     * Copy assumes, that src has same type and size as *this.
     */
    virtual void copyData(ImageBase* src) = 0;

  public:

    /**
     * type of function to be called by destructor
     * This is typically used by visualisation to close open windows and
     * is not usable for different purposes
     */
    typedef void (*DestroyFPtr)(ImageBase*);

  protected:

    int VisNumber; /**< Number of current visualisations */

  protected:
    /**
     * pointer to the registered call back function
     * must be set to nullptr by constructor
     */
    DestroyFPtr dfp = nullptr;

  public:

    /*
     * set call back function, that is called, if the image is destroyed.
     * used by visualisation to switch off before the image is gone
     */

    virtual void startVis(DestroyFPtr af)
    {
      VisNumber++;
      dfp = af;
    }

    virtual void stopVis()
    {
      VisNumber++;

      if (VisNumber == 0)
        {
          dfp = nullptr;
        }
    }

    /* do the callback */
    virtual void destroy()
    {
      if (dfp != nullptr)
        {
          (*dfp)(this);
        }

      dfp = nullptr;
      VisNumber = 0;
    }

  protected:
    //! Reference counter
    /*!
      counts the number of "references" (of type Image) that point
      to this instance
    */
    int refcount; // number of references to Image
    //! Pointer to parent
    /*! in case of an subimage this pointer points to the base image.
      it is nullptr otherwise
    */
    ImageBase* parent;
    /**
     * Title of the image.
     * This is used for visualisation.
     */
    std::string title;

    friend class Image;
  };

  void DestroyWindows(ice::ImageBase* img);
}
#endif // #ifndef __IMAGEBASE_H
