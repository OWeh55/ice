#include <string>

#include <image.h>

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      cerr << "aufruf: background <filename> [size]" << endl;
      exit(1);
    }

  int size = 7;
  if (argc > 2)
    size = atoi(argv[2]);

  string fn = argv[1];
  int xs, ys, mv, ch;
  InfImgFile(fn, xs, ys, mv, ch);
  Image src;
  src.create(xs, ys, mv);
  src.read(fn);
  Image min;
  min.create(src);
  Image open;
  open.create(src);
  Image diff;
  diff.create(src);
  Show(ON, src, "Source");
  Show(ON, min, "Temp");
  Show(ON, open, "Background");
  Show(ON, diff, "Objekte");

  bool median = false;
  bool neg = false;
  bool cont = true;
  do
    {
      Printf("Size: %d ", size);
      if (median)
        {
          Print("Median");
        }
      else
        {
          Print(!neg ? "Opening" : "Closeing");
        }
      Print("\n");
      if (median)
        {
          MedianImg(src, size, open);
        }
      else
        {
          if (!neg)
            {
              erodeImg(src, min, size, size);
              dilateImg(min, open, size, size);
            }
          else
            {
              dilateImg(src, min, size, size);
              erodeImg(min, open, size, size);
            }
        }
      subImg(src, open, diff);
      int ch = GetChar();
      switch (ch)
        {
        case '+':
          size += 2;
          break;
        case '-':
          size -= 2;
          if (size < 3) size = 3;
          break;
        case 'n':
          neg = !neg;
          break;
        case 'm':
          median = !median;
          break;
        case 'q':
        case 'Q':
          cont = false;
        }
    }
  while (cont);
  return 0;
}
