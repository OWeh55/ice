#include <image.h>

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  if (argc > 1)
    fn = argv[1];

  int xs, ys, mv, ch;
  InfImgFile(fn, xs, ys, mv, ch);
  cout << fn << ": " << xs << "*" << ys << " " << mv << " " << ch << endl;
  if (ch == 1)
    {
      Image image;
      image.create(xs, ys, mv);
      cout << "Reading" << endl;
      image.read(fn);
      cout << "done" << endl;
      int max = 0;
      WindowWalker w(image);
      for (w.init(); !w.ready(); w.next())
        {
          int value = image.getPixel(w);
          if (value > max)
            max = value;
        }
      cout << "max: " << max << endl;
      Show(GRAY, image);
      GetChar();
    }
  else
    {
      ColorImage cimg;
      cimg.create(xs, ys, mv);
      cimg.read(fn);
      Show(ON, cimg);
      GetChar();
    }
}
