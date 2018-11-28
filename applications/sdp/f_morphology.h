#ifndef IM_MORPHOLOGY_H
#define IM_MORPHOLOGY_H

#include "f_filterallchannels1.h"

class DilateImage: public FilterAllChannelsOneInput
{
public:
  DilateImage(): FilterAllChannelsOneInput("dilate", 3, 5, "dilate image with box - "
        "<image> <sizex> <sizey> => <image+>") {}
  FFUNC(DilateImage);
protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int sx = getInputInt(1, 3);
    int sy = getInputInt(2, sx);
    dilateImg(img1, img2, sx, sy);
  }
};

FF(DilateImage)

class ErodeImage: public FilterAllChannelsOneInput
{
public:
  ErodeImage(): FilterAllChannelsOneInput("erode", 3, 5, "erode image with box - "
                                            "<image> <sizex> <sizey> => <image+>") {}
  FFUNC(ErodeImage);
protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int sx = getInputInt(1, 3);
    int sy = getInputInt(2, sx);
    erodeImg(img1, img2, sx, sy);
  }
};

FF(ErodeImage)

class OpeningImage: public FilterAllChannelsOneInput
{
public:
  OpeningImage(): FilterAllChannelsOneInput("opening", 3, 5, "apply opening with box - "
        "<image> <sizex> <sizey> => <image+>") {}
  FFUNC(OpeningImage);
protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int sx = getInputInt(1, 3);
    int sy = getInputInt(2, sx);
    openingImg(img1, img2, sx, sy);
  }
};

FF(OpeningImage)

class ClosingImage: public FilterAllChannelsOneInput
{
public:
  ClosingImage(): FilterAllChannelsOneInput("closing", 3, 5, "apply closing with box - "
        "<image> <sizex> <sizey> => <image+>") {}
  FFUNC(ClosingImage);
protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int sx = getInputInt(1, 3);
    int sy = getInputInt(2, sx);
    closingImg(img1, img2, sx, sy);
  }
};

FF(ClosingImage)

class Rank: public FilterAllChannelsOneInput
{
public:
  Rank(): FilterAllChannelsOneInput("rank", 4, 5, "apply rank filter - "
                                      "<image> <sizex> <sizey> <rank> => <image+>") {}

  FFUNC(Rank);

protected:

  virtual void filterOneChannel(const Image& imgsrc, const Image& img3)
  {
    Image img1 = imgsrc;
    int fsizex = getInputInt(1, 5);
    int fsizey = getInputInt(2, fsizex);
    int defrank = fsizex * fsizey / 2;
    int rank = getInputInt(3, defrank);
    RankImg(img1, fsizex, fsizey, rank, img3);
  }
};

FF(Rank)
#endif
