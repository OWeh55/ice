/**
 * This demo shows how to convert
 * a ColorImage to YUV colorspace
 */

#include <iostream>

#include <image.h>

int main(int argc, char** argv)
{
  ColorImage img;
  img.read("test_rgb.jpg");

  int xs = img.xsize;
  int ys = img.ysize;
  int mv = img.maxval;

  Image Y;
  Y.create(xs, ys, mv);
  Show(ON, Y, "Y");
  Image U;
  U.create(xs, ys, mv);
  Show(ON, U, "U");
  Image V;
  V.create(xs, ys, mv);
  Show(ON, V, "V");

  Display(ON);
  Show(ON, img, "original");

  double start = TimeD();
  ColorImageToYuv(img, Y, U, V);

  cout << "converted to YUV in " << TimeD() - start << " seconds" << endl;

  ColorImage imgr;
  imgr.create(img);
  Show(ON, imgr, "zurück");

  start = TimeD();
  YuvToColorImage(Y, U, V, imgr);

  cout << "converted from YUV in " << TimeD() - start << " seconds" << endl;

  // look for differences between the original and the converted
  ColorImage imgdiff;
  imgdiff.create(img);

  subImg(img, imgr, imgdiff);
  cout << "error in R channel: " << findMin(imgdiff.redImage()) << " " << findMax(imgdiff.redImage()) << endl;
  cout << "error in G channel: " << findMin(imgdiff.greenImage()) << " " << findMax(imgdiff.greenImage()) << endl;
  cout << "error in B channel: " << findMin(imgdiff.blueImage()) << " " << findMax(imgdiff.blueImage()) << endl;

  int maxdif = max(findMax(imgdiff.redImage()), max(findMax(imgdiff.greenImage()), findMax(imgdiff.blueImage())));

  cout << "maximaler Fehler: " << maxdif << endl;

  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        ColorValue cv  = ColorValue(255, 255, 255) - imgdiff.getPixel(x, y);
        // cout << cv.red << " " << cv.green << " " << cv.blue << endl;
        imgdiff.setPixelLimited(x, y, cv * imgdiff.maxval / maxdif);
      }

  Show(ON, imgdiff, "Differenz");
  GetChar();
  Display(OFF);
  return 0;
}
