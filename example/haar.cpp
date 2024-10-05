/********************************************************************
Test for haar transform
********************************************************************/

#include <stdlib.h>
#include <image.h>
#include <math.h>
#include <stdio.h>

int p = 4;

int main(int argc, char* argv[])
{
  if (argc > 1)
    p = atoi(argv[1]);

  Image img1;
  img1.read("test_gray.jpg");
  int xsize = img1.xsize;
  int ysize = img1.ysize;

  for (int i = 0; i < p; i++)
    {
      xsize /= 2;
      ysize /= 2;
    }

  for (int i = 0; i < p; i++)
    {
      xsize *= 2;
      ysize *= 2;
    }

  int p2 = 1;
  for (int i = 0; i < p; i++)
    p2 *= 2;

  Image img2;
  img2.create(xsize, ysize, (img1.maxval + 1) * p2 * p2 - 1);

  Show(GRAY, img1, "original");
  Show(GRAY, img2, "transformiert");

  // Teilfenster mit groesse 2^n * 2^n
  HaarImg(img1(Window(0, 0, xsize - 1, ysize - 1)), img2, p, NORMAL);

  GetChar();

  Image img3;
  img3.create(xsize, ysize, 255);

  HaarImg(img2, img3, p, INVERS);
  Show(GRAY, img3, "zurücktransformiert");
  /*
  vector<int> iv{1,2,3,4, 4,3,2,1, 4,4,3,3, 4,4,1,1};
  cout << iv << endl;
  haar(iv,p);
  cout << iv << endl;
  haarInverse(iv,p);
  cout << iv << endl;
  */
  GetChar();
  return 0;
}
