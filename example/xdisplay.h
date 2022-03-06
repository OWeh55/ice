#ifndef XDISPLAY_H
#define XDISPLAY_H

#include <image.h>

// helper class for display of n images
// allow for interactive settings

class XDisplay
{
public:
  enum Mode { mUNKNOWN, mSIGNED, mUNSIGNED, mFREE };

  XDisplay(int xSize, int ySize, int nImages);

  void show();
  void show(int i, const Image& img);
  void show(int i, const ImageD& img, bool log = false, Mode mode = mUNKNOWN);
  void setTitle(int i, const string& title);
  void drawText(int i, const string& title, int x, int y);
  void setFactor(int i, double f);
  void setZoom(int i, int f);
  void clearMarker(int i);
private:
  void copyShow(int i);
  double getZoom(int xs, int ys) const;
  void estimateScaling(int i);

  // images for display Show(OVERLAY,dImage,mImage)
  Image dImage;
  Image mImage;

  vector<Window> windows;
  // sub images for display images
  vector<Image> pImages;

  // source images to display
  vector<ImageD> sImages;

  vector<double> zooms;
  vector<double> shiftxs;
  vector<double> shiftys;

  vector<double> factors;
  vector<double> offsets;
  vector<Mode> modes;

  int iXSize;
  int iYSize;
};

#endif
