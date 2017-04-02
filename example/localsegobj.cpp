#include <image.h>

#define XS 1024
#define YS 1024

typedef object_rc(*of)(const Image& iv, int x, int y, int thr);

void search_and_mark(const Image& m1, Image& imgo, of suche, of folge)
{
  IPoint ps(0, 0);
  vector<Contur> objects;
  vector<Contur> holes;
  Contur c;
  ClearImg(imgo);

  while (SearchStart(m1, imgo, suche, 123, 5, ps) == OK)
    {
      // Bild nach Startpunkt durchsuchen
      c = CalcContur(m1, imgo, folge, 123, ps); // Kontur verfolgen

      if (c.isValid())
        {
          if (c.isClosed())                  // geschlossene Konturen füllen
            {
              if (c.isHole())
                {
                  holes.push_back(c);
                  FillRegion(c, 3, imgo);        // Loch = blau
                }
              else
                {
                  objects.push_back(c);          // Objekt = gruen
                  FillRegion(c, 2, imgo);
                }
            }

          MarkContur(c, 1, imgo);              // jede Kontur markieren
        }
      else PutVal(imgo, ps.X(), ps.Y(), 1);  // als bearbeitet markieren
    }
}

int main(int argc, char* argv[])
{
  Image i1;

  if (argc > 1)
    {
      string fn = argv[1];
      i1 = ReadImg(fn);
    }
  else
    {
      // artificial image
      i1 = NewImg(XS, YS, 255);

      for (int y = 0; y < i1.ysize; y++)
        for (int x = 0; x < i1.xsize; x++)
          {
            PutVal(i1, x, y, 255 - (y / 4));
          }

      for (unsigned int i = 0; i < 230; i++)
        {
          int x = Random(XS - 100);
          int y = Random(YS - 100);
          int rad = Random(29);
          int grw = Random(255);
          Circle c(x, y, rad);
          draw(c, i1, grw, grw);
        }
    }

  Image m1 = NewImg(i1);
  ClearImg(m1);

//  Image mi=NewImg(i1);
//  Image ma=NewImg(i1);

//  Show(ON,mi);
//  Show(ON,ma);

//  MinMaxImg(i1,5,5,mi,ma);

  Show(OVERLAY, i1, m1, "LocalSeg");

  LocalSeg(i1, m1, 7, 25, 15);

  Image imgo = NewImg(m1);                 // Markierungsbild anlegen
  ClearImg(imgo);                          // und löschen
  Show(OVERLAY, i1, imgo, "High/High");      // Bilddarstellung

  search_and_mark(m1, imgo, LocalSegObjHigh, LocalSegObjHigh);

  Image imgo2 = NewImg(m1);                 // Markierungsbild anlegen
  ClearImg(imgo2);                          // und löschen
  Show(OVERLAY, i1, imgo2, "High/Low");      // Bilddarstellung

  search_and_mark(m1, imgo2, LocalSegObjHigh, LocalSegObj);

  Image imgo3 = NewImg(m1);                 // Markierungsbild anlegen
  ClearImg(imgo3);                          // und löschen
  Show(OVERLAY, i1, imgo3, "Low/Low");      // Bilddarstellung

  search_and_mark(m1, imgo3, LocalSegObj, LocalSegObj);

  GetChar();
  return OK;
}
