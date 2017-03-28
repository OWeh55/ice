/**
 * This demo shows how to convert
 * a ColorImage to HSI colorspace
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

  Image hue;
  hue.create(xs, ys, mv);
  Show(ON, hue, "H");
  Image saturation;
  saturation.create(xs, ys, mv);
  Show(ON, saturation, "S");
  Image intensity;
  intensity.create(xs, ys, mv);
  Show(ON, intensity, "I");

  Display(ON);
  Show(ON, img, "original");

  double start = TimeD();
  ColorImageToHsi(img, hue, saturation, intensity);

  cout << "converted to HSI in " << TimeD() - start << " seconds" << endl;

  ColorImage imgr;
  imgr.create(img);
  Show(ON, imgr, "zurück");

  start = TimeD();
  HsiToColorImage(hue, saturation, intensity, imgr);

  cout << "converted from HSI in " << TimeD() - start << " seconds" << endl;

  // look for differences between the original and the converted
  ColorImage imgdiff;
  imgdiff.create(img);

  SubImg(img, imgr, imgdiff);
  cout << "error in R channel: " << findMin(imgdiff.redImage()) << " " << findMax(imgdiff.redImage()) << endl;
  cout << "error in G channel: " << findMin(imgdiff.greenImage()) << " " << findMax(imgdiff.greenImage()) << endl;
  cout << "error in B channel: " << findMin(imgdiff.blueImage()) << " " << findMax(imgdiff.blueImage()) << endl;

  int maxdif = Max(findMax(imgdiff.redImage()), Max(findMax(imgdiff.greenImage()), findMax(imgdiff.blueImage())));

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
