#include <image.h>

int main(int argc, char** argv)
{
  ColorImage img;
  img.read("test_rgb.jpg");
  Show(ON, img, "original");
  Statistics stat(3);
  WindowWalker w(img);
  for (w.init(); !w.ready(); w.next())
    {
      ColorValue cv = img.getPixel(w);
      vector<double> v(3);
      for (int i = 0; i < 3; i++)
        v[i] = cv[i];
      stat.put(v);
    }

  Matrix kmatrix(KLT(stat));
  cout << kmatrix << endl;

  matrix<double> mat = kmatrix.toMatrix();

  ColorImage img2;
  img2.create(img);
  Show(ON, img2, "KLT");

  vector<Image> imgs(3);
  for (int i = 0; i < 3; i++)
    {
      imgs[i].create(img2.xsize, img2.ysize, 255);
      Show(ON, imgs[i], "v" + to_string(i));
    }

  for (w.init(); !w.ready(); w.next())
    {
      ColorValue cv = img.getPixel(w);
      vector<double> v(3);
      for (int i = 0; i < 3; i++)
        {
          v[i] = cv[i];
        }
      v = mat * v;
      for (int i = 0; i < 3; i++)
        {
          cv[i] = v[i];
          imgs[i].setPixelLimited(w, cv[i]);
        }
      img2.setPixelLimited(w, cv);
    }
  GetChar();
  return OK;
}
