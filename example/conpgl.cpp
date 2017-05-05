// Pegelorientierte Kontursuche

#include <stdio.h>
#include <math.h>
#include <image.h>

using namespace ice;

/************************************************/
int main(int argc, char* argv[])
{
  Image img, imgm, imgl, imgm2;

  bool fill = false;
  int pgl = -1;
  int level = 20;

  string fn = "";

  int rc;

  while ((rc = getopt(argc, argv, "p:l:f")) >= 0)
    {
      switch (rc)
        {
        case 'p':
          pgl = atol(optarg);
          break;
        case 'l':
          level = atol(optarg);
          break;
        case 'f':
          fill = true;
          break;
        }
    }

  if (argc - optind > 0)
    fn = argv[optind];

  if (!fn.empty())
    img.read(fn);
  else
    {
      img.create(1024, 768, 255);
    }

  imgm.create(img->xsize, img->ysize, 3);
  imgm2.create(img->xsize, img->ysize, 3);

  imgl.create(img->xsize, img->ysize, 3);

  clearImg(imgm);
  clearImg(imgm2);

  if (fn.empty())
    {
      clearImg(img);

      draw(Circle(200, 200, 180), img, 222, 222);

      draw(Circle(55, 100, 50), imgm, 2, 2);
      draw(Circle(55, 100, 50), imgm2, 2, 2);

      draw(Circle(800, 100, 80), img, 222, 222);

      draw(Circle(900, 400, 180), img, 222, 222);

      draw(Circle(500, 100, 80), img, 222, 222);
      draw(Circle(520, 90, 30), img, 22, 22);
    }

  Show(OVERLAY, img, imgm);

  if (pgl < 0)
    pgl = CalcThreshold(img);

  Print("Pegelbasierte Konturfolge\n");
  IPoint ps;

  double ta = TimeD();

  while (SearchStart(img, imgm, NULL, pgl, 1, ps, HORZ) == OK)
    {
      //    Printf("%d %d\n",ps[0],ps[1]);
      //     PutVal(imgm1,ps[0],ps[1],1);
      //     GetChar();
      //     PutVal(imgm1,ps[0],ps[1],0);
#if 1
      MarkedImageObjectFunctor of(img, imgm, pgl);
      Contur c = CalcContur(of, ps);
#else
      Contur c = CalcContur(img, imgm, NULL, pgl, ps, 0, isunknown, isunknown);
#endif

      if (c.isValid())
        {
          MarkContur(c, 1, imgm);

          if (c.Number() > 10)
            {
              Marker(3, c.Start(), 3, 6, imgm);
              Marker(1, c.getPoint(10), 3, 6, imgm);
            }

          Printf("Valid %d number %d closed %d hole %d\n", c.isValid(), c.Number(), c.isClosed(), c.isHole());
          Printf("         Length %lf\n", c.Length());
          // GetChar();
        }

      ps.x += 1;
      //    ps[0]=0; ps[1]=0;
    }

  cout << "Zeit: " << TimeD() - ta << endl;

  GetChar();

  Show(OVERLAY, img, imgl);
  LocalSeg(img, imgl, 15, level);

  Show(OVERLAY, img, imgm2);

  Print("Konturfolge anhand lokaler Segmentierung\n");

  ps = IPoint(0, 0);

  ta = TimeD();

  while (SearchStart(imgl, imgm2, LocalSegObj, pgl, 1, ps, HORZ) == OK)
    {
#if 1
      MarkedImageObjectFunctor of(imgl, imgm2, LocalSegObj);
      Contur c = CalcContur(of, ps);
#else
      Contur c = CalcContur(imgl, imgm2, LocalSegObj, pgl, ps, 0, isunknown, isunknown);
#endif

      if (c.isValid())
        {
          MarkContur(c, 1, imgm2);

          if (fill)
            if (c.isClosed())
              FillRegion(c, 2, imgm2);

          if (c.Length() > 10)
            {
              Marker(3, c.Start(), 3, 6, imgm);
              Marker(1, c.getPoint(10), 3, 6, imgm);
            }

          Printf("Valid %d number %d closed %d hole %d\n", c.isValid(), c.Number(), c.isClosed(), c.isHole());
          Printf("         Length %lf\n", c.Length());
          // GetChar();
        }
    }

  cout << "Zeit: " << TimeD() - ta << endl;

  GetChar();

  Display(OFF);

  return 0;
}
