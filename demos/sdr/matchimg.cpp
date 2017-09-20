#include "image.h"
#include "smatch.h"

// das ist eine Demo-Variante der ICE-Funktion!!

using namespace ice;

#define FNAME "DetectShift1"
int DetectShift1(Image img1, Image img2, Image imgr,
                 double& dx, double& dy, double& val,
                 double beta)
{
  int sx = img1.xsize;
  int sy = img1.ysize;

//  cout << sx << " " << sy << " " << wsizex(img2) << " " << wsizey(img2) << endl;
//  cout << wsizex(imgr) << " " << wsizey(imgr) << endl;

  if ((sx != img2.xsize) || (sy != img2.ysize))
    {
      Message(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
      return WRONG_PARAM;
    }

  IfFailed(InvConvolutionImg(img1, img2, imgr, 0.0, beta, MD_IGNORE_BIAS))
  {
    Message(FNAME, M_0, ERROR);
    return ERROR;
  }

  IfFailed(val = PeakValuation(imgr, Image(), dx, dy))
  {
    Message(FNAME, M_0, ERROR);
    return ERROR;
  }

  dx -= sx / 2;
  dy -= sy / 2; // Peak coordinates -> Shift (of windows)
//  Wait(0);
//  Show(OFF,imgr);
//  FreeImg(imgr);
  return OK;
}
#undef FNAME

#define MINVAL (0.8)

#define FNAME "DetectTrafo1"

int inside(const Image& img, const Trafo& tr, int x, int y)
{
  double xd, yd;
  Transform(tr, xd, yd);
  return img.inside(xd, yd);
}

void TransformWindow(const Trafo tr,
                     int wxi, int wyi, int wxa, int wya,
                     int& wxit, int& wyit, int& wxat, int& wyat)
{
  double xt, yt;
  Transform(tr, wxi, wyi, xt, yt);
  wxat = wxit = xt;
  wyat = wyit = yt;
  Transform(tr, wxi, wya, xt, yt);
  wxit = Min<double>(wxit, xt);
  wxat = Max<double>(wxat, xt);
  wyit = Min<double>(wyit, yt);
  wyat = Max<double>(wyat, yt);
  Transform(tr, wxa, wya, xt, yt);
  wxit = Min<double>(wxit, xt);
  wxat = Max<double>(wxat, xt);
  wyit = Min<double>(wyit, yt);
  wyat = Max<double>(wyat, yt);
  Transform(tr, wxa, wyi, xt, yt);
  wxit = Min<double>(wxit, xt);
  wxat = Max<double>(wxat, xt);
  wyit = Min<double>(wyit, yt);
  wyat = Max<double>(wyat, yt);
}

void SetWindow2Img(Image img1, Image img2, int maxx, int maxy)
{
  int sx = wsizex(img1);
  if (wsizex(img2) < sx) sx = wsizex(img2);
  if (maxx < sx) sx = maxx;

  int sy = wsizey(img1);
  if (wsizey(img2) < sy) sy = wsizey(img2);
  if (maxy < sy) sy = maxy;

  int sx2, sy2;
  for (sx2 = 1; sx2 * 2 <= sx; sx2 += sx2);
  for (sy2 = 1; sy2 * 2 <= sy; sy2 += sy2);

  img1->wxi += (wsizex(img1) - sx2) / 2;
  img1->wyi += (wsizey(img1) - sy2) / 2;
  img1->wxa = img1->wxi + sx2 - 1;
  img1->wya = img1->wyi + sy2 - 1;

  img2->wxi += (wsizex(img2) - sx2) / 2;
  img2->wyi += (wsizey(img2) - sy2) / 2;
  img2->wxa = img2->wxi + sx2 - 1;
  img2->wya = img2->wyi + sy2 - 1;
}

int DetectTrafo1(Image img1, Image img2, Image himg,
                 Image mark,
                 Trafo& tr,
                 double beta, int ct,
                 int mode)
{
  double dx, dy, val;

  int sx1, sy1;
  int sx2, sy2;
  int xi, yi;
  int wxi1, wyi1, wxa1, wya1;
  int wxi2, wyi2, wxa2, wya2;

  wxi1 = img1->wxi;
  wyi1 = img1->wyi;
  wxa1 = img1->wxa;
  wya1 = img1->wya;
  wxi2 = img2->wxi;
  wyi2 = img2->wyi;
  wxa2 = img2->wxa;
  wya2 = img2->wya;
  sx1 = wsizex(img1);
  sy1 = wsizey(img1);
  sx2 = wsizex(img2);
  sy2 = wsizey(img2);

  // if ((img2->xsize!=himg->xsize)||(img2->ysize!=himg->ysize))
//   {
//     Message(FNAME,M_WRONG_IMGSIZE,WRONG_PARAM);
//     return WRONG_PARAM;
//   }

  Trafo tri; // Inverse Transformation zu tr;
  Trafo trd; // "Korrektur-Transformation" während Iteration

  if ((mode & DT_REFINE) == 0)
    {
      tr = Trafo(); /* new transformation */

      if (sx1 < sx2)
        {
          img2->wxi += (sx2 - sx1) / 2;
          img2->wxa = img2->wxi + sx1 - 1;
        }
      if (sx2 < sx1)
        {
          img1->wxi += (sx1 - sx2) / 2;
          img1->wxa = img1->wxi + sx2 - 1;
        }
      if (sy1 < sy2)
        {
          img2->wyi += (sy2 - sy1) / 2;
          img2->wya = img2->wyi + sy1 - 1;
        }
      if (sy2 < sy1)
        {
          img1->wyi += (sy1 - sy2) / 2;
          img1->wya = img1->wyi + sy2 - 1;
        }

      if (!ignoreglobal)
        {
          SetWindow2Img(img1, img2, GLOBALSHIFTSIZE, GLOBALSHIFTSIZE);

          Image im = NewImg(Iglobalshift, false);
          ClearImg(im);
          if (cross)
            {
              Line(0, im->ysize / 2, im->xsize - 1, im->ysize / 2, 1, im);
              Line(im->xsize / 2, im->ysize - 1, im->xsize / 2, 0, 1, im);
            }
          Show(OVERLAY, Iglobalshift, im);
          // Detection der globalen Verschiebung
          DetectShift1(img1, img2, Iglobalshift, dx, dy, val, beta * 3);

          Printf("globale Verschiebung: %5.1f %5.1f (%5.3f)\n", dx, dy, val);
          Wait(0);
          if (val < MINVAL)
            {
              Message(FNAME, "Can't detect global shift", ERROR);
              return ERROR;
            }
          Show(OFF, Iglobalshift);
          tr.Shift(dx, dy);
          SetWindowImg(img1);
          SetWindowImg(img2);
        }
    }

//  int bs=Min(Min(sx1,sy1),Min(sx2,sy2))/5;
//  int blocksize=32;
//  while (blocksize*2<=bs) blocksize=blocksize+blocksize;
  int blocksize = LOCALSHIFTSIZE;
  Image im = NewImg(Ilocalshift, false);
  ClearImg(im);
  if (cross)
    {
      Line(0, im->ysize / 2, im->xsize - 1, im->ysize / 2, 1, im);
      Line(im->xsize / 2, im->ysize - 1, im->xsize / 2, 0, 1, im);
    }
  ClearImg(Ilocalshift);
  Show(OVERLAY, Ilocalshift, im);
  int count = 0;
  while (count < ct)
    {
      SetWindowImg(himg);
      Transform(tr, img1, himg, INTERPOL);
//    CopyImg(img1,mark);
      SetImg(mark, mark->maxval);
      Wait(1);
      tri = tr;
      tri.Invert();
      xi = sx2 / blocksize;
      yi = sy2 / blocksize;
      if ((xi > 0) && (yi > 0))
        {
          // Verfeinerung
          int i, j;
          int x1, x2, y1, y2;
          double xm, ym;
          Matrix pl1(0, 2); // Punktlisten
          Matrix pl2(0, 2);
          for (j = 0; j < yi; j++)
            for (i = 0; i < xi; i++)
              {
                x1 = i * blocksize + wxi2;
                y1 = j * blocksize + wyi2;
                x2 = x1 + blocksize - 1;
                y2 = y1 + blocksize - 1;
                if (
                  insidew(img1, tri, x1, y1) &&
                  insidew(img1, tri, x1, y2) &&
                  insidew(img1, tri, x2, y1) &&
                  insidew(img1, tri, x2, y2)
                )
                  {
                    SetWindowImg(himg, x1, y1, x2, y2);
                    SetWindowImg(img2, x1, y1, x2, y2);
                    {
                      int x, y;
                      for (y = y1; y <= y2; y++)
                        for (x = x1; x <= x2; x++) PutVal(mark, x, y, 0);
                    }
                    DetectShift1(himg, img2, Ilocalshift, dx, dy, val, beta);
                    Printf("Lokale Verschiebung: %5.1f %5.1f (%5.3f)\n", dx, dy, val);

                    Wait(0);

                    if (val > MINVAL)
                      {
                        xm = (x1 + x2) / 2.0;
                        ym = (y1 + y2) / 2.0;
                        pl1 = pl1 && Vector(xm, ym);
                        pl2 = pl2 && Vector(xm + dx, ym + dy);
                      }
                    {
                      int x, y;
                      for (y = y1; y <= y2; y++)
                        for (x = x1; x <= x2; x++) PutVal(mark, x, y, mark->maxval/*GetVal(img1,x,y)*/);
                    }
                  }
              }
          if (pl1.rows() > 4)
            {
//  cout << "iter: " << count << endl;
//      cout << pl1.rows() << "Punkte " << endl;
              if (count < 3)
//    trd=MatchPointlistsLinOpt(pl1,pl2);// erste iterationen nur affin
                trd = MatchPointlists(pl1, pl2); // erste iterationen nur affin
              else
//    trd=MatchPointlistsLinOpt(pl1,pl2,TRM_PROJECTIVE);// ..dann projektiv zulassen
                trd = MatchPointlists(pl1, pl2, TRM_PROJECTIVE); // ..dann projektiv zulassen
              tr = tr * trd;
            }
        }
      count++;
    }
  Wait(1);
  Show(OFF, Ilocalshift);
  SetWindowImg(img2, wxi2, wyi2, wxa2, wya2);
  return OK;
}

int DetectTrafo1(Image img1, Image img2, Trafo& tr,
                 double beta, int ct,
                 int mode)
{
  Image himg = NewImg(img2->xsize, img2->ysize, img1->maxval); // Transform. d. 1. Bild
  Image mark = NewImg(img2->xsize, img2->ysize, img1->maxval); // Transform. d. 1. Bild
  IfFailed(DetectTrafo1(img1, img2, himg, mark, tr, beta, ct, mode))
  {
    FreeImg(himg);
    Message(FNAME, M_0, ERROR);
    return ERROR;
  }
  return OK;
}

#undef FNAME
