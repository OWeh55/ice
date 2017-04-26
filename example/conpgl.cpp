/* Pegelorientierte Kontursuche */
#include <stdio.h>
#include <math.h>
#include <image.h>

using namespace ice;

/************************************************/
int main(int argc, char* argv[])
{
  Image img, imgm, imgl, imgm2;
  Contur c;
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
    img = ReadImg(fn);
  else
    {
      img = NewImg(500, 500, 255);
    }

  imgm = NewImg(img->xsize, img->ysize, 3);
  imgm2 = NewImg(img->xsize, img->ysize, 3);

  imgl = NewImg(img->xsize, img->ysize, 3);

  clearImg(imgm);
  clearImg(imgm2);

  if (fn.empty())
    {
      clearImg(img);
      Circle circle(100, 100, 80);
      draw(circle, img, 222, 222);

      Circle circle1(300, 100, 80);
      draw(circle1, img, 222, 222);


      Circle circle2(55, 100, 50);
      draw(circle2, imgm, 2, 2);
      draw(circle2, imgm2, 2, 2);
    }

  Show(OVERLAY, img, imgm);

  if (pgl < 0)
    pgl = CalcThreshold(img);

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
      c = CalcContur(of, ps);
#else
      c = CalcContur(img, imgm, NULL, pgl, ps, 0, isunknown, isunknown);
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
          GetChar();
        }

      ps.x += 1;
      //    ps[0]=0; ps[1]=0;
    }

  cout << "Zeit: " << TimeD() - ta << endl;

  GetChar();

  Show(OVERLAY, img, imgl);
  LocalSeg(img, imgl, 15, level);

  Show(OVERLAY, img, imgm2);

  ps = IPoint(0, 0);

  ta = TimeD();

  while (SearchStart(imgl, imgm2, LocalSegObj, pgl, 1, ps, HORZ) == OK)
    {
      //    Printf("%d %d\n",ps[0],ps[1]);
      //     PutVal(imgm1,ps[0],ps[1],1);
      //     GetChar();
      //     PutVal(imgm1,ps[0],ps[1],0);
#if 1
      MarkedImageObjectFunctor of(imgl, imgm2, LocalSegObj);
      c = CalcContur(of, ps);
#else
      c = CalcContur(imgl, imgm2, LocalSegObj, pgl, ps, 0, isunknown, isunknown);
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
          GetChar();
        }
    }

  cout << "Zeit: " << TimeD() - ta << endl;

  GetChar();

  Display(OFF);

  return 0;
}
