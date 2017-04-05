#include <image.h>
#include <diff.h>

int main(int argc, char** argv)
{
  Image img1;
  string fn1;
  Image img2;
  string fn2;
  int mode;

  int rc;

  while ((rc = getopt(argc, argv, "m:")) >= 0)
    {
      switch (rc)
        {
        case 'm':
          mode = atol(optarg);
          break;
        }
    }

  if (argc > optind)
    {
      fn1 = argv[optind];
      fn2 = fn1;
    }

  if (argc > optind + 1)
    {
      fn2 = argv[optind + 1];
    }

  img1 = ReadImg(fn1);
  img2 = ReadImg(fn2);
  Image img3 = NewImg(img1);

  Show(ON, img1, fn1);
  Show(ON, img2, fn2);
  Show(ON, img3, "Diff");

  bool ready = false;

  while (!ready)
    {
      subImg(img1, img2, img3);
      Printf("dif: %g ",  Diff(img1, img2, 0));
      Printf("abs: %g ",  Diff(img1, img2, 1));
      Printf("sqr: %g\n", Diff(img1, img2, 2));
      rc = GetChar();

      switch (rc)
        {
        case 'q':
        case 'Q':
          ready = true;
          break;
        case 'X':
          shift(img2, img2, 1, 0, 0);
          break;
        case 'x':
          shift(img2, img2, -1, 0, 0);
          break;
        case 'Y':
          shift(img2, img2, 0, 1, 0);
          break;
        case 'y':
          shift(img2, img2, 0, -1, 0);
          break;
        }
    }

  return OK;
}
