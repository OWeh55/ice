#include <image.h>
#include <dijkstra.h>

void Richtung(const Image& src, Image& dst, Freeman dir, int size)
{
  int s = size / 2;

  for (int y = s; y < src->ysize - s; y++)
    for (int x = s; x < src->xsize - s; x++)
      {
        IPoint p(x, y), p1;
        int val = GetVal(src, p);
        p1 = p;

        for (int i = 0; i < s; i++)
          {
            dir.move(p1);
            val += GetVal(src, p1);
          }

        p1 = p;

        for (int i = 0; i < s; i++)
          {
            dir.Inverse().move(p1);
            val += GetVal(src, p1);
          }

        PutVal(dst, p, val / size);
      }
}

void Linie(const Image& src, Image& dst, int size)
{
  Image h = NewImg(src);
  SetImg(dst, dst->maxval);

  for (int dir = 0; dir < 4; dir++)
    {
      Richtung(src, h, dir, size);
      MinImg(h, dst, dst);
    }
}

int main(int argc, char** argv)
{
  string filename = "test_gray.jpg";

  if (argc > 1)
    filename = argv[1];

  int xs, ys, mv, ch;
  InfImgFile(filename, xs, ys, mv, ch);
  Image img;
  img.create(xs, ys, mv, "Original");
  // Show(ON,img);

  Image h1;
  h1.create(xs, ys, mv, "hilfe");
  ReadImg(filename, img);
#if 0
  Image filt(xs, ys, mv, "Gefiltert");
  CopyImg(img, filt);
  Show(ON, filt);

  for (int i = 0; i < 4; i += 1)
    {
      Richtung(img, filt, i, 3);
      SmearImg(img, h1, 3, 3);
      SubImg(filt, h1, filt, SMD_SHIFT);
      GetChar();
    }

  exit(1);
  GetChar();

  CopyImg(filt, img);
#endif
  //  DoBImg(img,img,1,5);

  //  GetChar();

  //  Image h=NewImg(img);
  //  ErodeImg(img,5,5,h);
  //  SubImg(img,h,img);

  Image mrk;
  mrk.create(xs, ys, 11);
  ClearImg(mrk);
  Show(OVERLAY, img, mrk, "Ergebnis");

  while (true)
    {
      Print("Startpunkt waehlen\n");
      IPoint start = SelPoint(img);

      Print("Endpunkt waehlen\n");
      IPoint ende = SelPoint(img);

      ClearImg(mrk);
      PutVal(mrk, ende.x, ende.y, 1);

      Contur c = Dijkstra(img, start, mrk);
      GetChar();
      ClearImg(mrk);
      MarkContur(c, 1, mrk);
      GetChar();
    }

  return 0;
}
