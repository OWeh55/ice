/**************************************************************/
/* Test WriteImg und ReadImg                                         */
/**************************************************************/

#include <stdio.h>
#include <image.h>

#define filename_gray "test.pgm"
#define filename_rgb "test3.ppm"

#define testfilename_gray "test_gray.jpg"
#define testfilename_rgb "test_rgb.jpg"

//#define testfilename_gray "test_gray.tif"
//#define testfilename_rgb "test_rgb.tif"

int main(int argc, char* argv[])
{
  Image p1, p2, p3;
  int xs, ys, mv, ch;
  Display(ON);
  ClearAlpha();

  Printf("Test-Bild %s laden\n", testfilename_gray);
  InfImgFile(testfilename_gray, xs, ys, mv, ch);
  Printf("Bildgroesse %d*%d %d\n", xs, ys, mv);
  p1 = NewImg(xs, ys, mv);
  Show(ON, p1);
  ReadImg(testfilename_gray, p1);

  if (WriteImg(p1, filename_gray) != OK)
    exit(1);

  Printf("Bildfile %s ausgeschrieben\n", filename_gray);
  GetChar();

  p2 = ReadImg(filename_gray);
  Printf("Bildfile %s eingelesen (img=NULL)\n", filename_gray);
  Show(ON, p2);
  GetChar();
  Show(OFF, p2);

#if 0
  p2 = NewImg(210, 210, 255); /* Bild mit anderen Abmessungen */
  Show(ON, p2);
  ReadImg(filename_gray, p2);
  Printf("Bildfile %s eingelesen in vorgegebenes Bild\n", filename_gray);
#endif

  GetChar();

  Printf("Test-Bild %s laden\n", testfilename_rgb);
  InfImgFile(testfilename_rgb, xs, ys, mv, ch);
  Printf("Bildgroesse %d*%d %d\n", xs, ys, mv);

  p1.create(xs, ys, mv);
  p2.create(xs, ys, mv);
  p3.create(xs, ys, mv);

  // cout << p1.xsize << "x" << p1.ysize << " " << p1.maxval << endl;
  // cout << p2.xsize << "x" << p2.ysize << " " << p2.maxval << endl;
  // cout << p3.xsize << "x" << p3.ysize << " " << p3.maxval << endl;

  ReadImg(testfilename_rgb, p1, p2, p3);
  Show(_RGB, p1, p2, p3);
  Show(ON, p1, "rot");
  Show(ON, p2, "gruen");
  Show(ON, p3, "blau");
  Printf("Test-Bild(RGB) %s eingelesen\n", testfilename_rgb);

  if (WriteImg(p1, p2, p3, filename_rgb) != OK) exit(1);

  Printf("Bildfile %s ausgeschrieben\n", filename_rgb);
  GetChar();

  InfImgFile(filename_rgb, xs, ys, mv, ch);

  Printf("Bildgroesse %s =  %d*%d %d\n", filename_rgb, xs, ys, mv);

  if ((ReadImg(filename_rgb, p1, p2, p3)) != OK)
    exit(255);

  Printf("Bildfile %s eingelesen (r,g,b=NULL)\n", filename_rgb);
  Show(_RGB, p1, p2, p3);
  GetChar();
  Show(OFF, p1);

#if 0

  p1 = NewImg(210, 210, 255); /* Bild mit anderen Abmessungen */
  p2 = NewImg(210, 210, 255); /* Bild mit anderen Abmessungen */
  p3 = NewImg(210, 210, 255); /* Bild mit anderen Abmessungen */
  Show(_RGB, p1, p2, p3);

  if (ReadImg(filename_rgb, p1, p2, p3) != OK) exit(255);

  Printf("Bildfile %s eingelesen in vorgegebenes Bild\n", filename_rgb);

  GetChar();

  p1 = NewImg(412, 412, 255);
  p2 = NewImg(412, 412, 255);
  p3 = NewImg(412, 412, 255);
  Show(_RGB, p1, p2, p3);
  ReadImg("test_rgb.jpg", p1, p2, p3, IB_SCALE);
  Print("Einlesen mit IB_SCALE\n");
  GetChar();
  ReadImg("test_rgb.jpg", p1, p2, p3, IB_CROP);
  Print("Einlesen mit IB_CROP\n");
#endif

  GetChar();

  return 0;
}
