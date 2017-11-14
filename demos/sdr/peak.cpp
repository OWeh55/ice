#include <image.h>
#include <heap.h>

#include "smatch.h"

int insidep(Image img, int xi, int yi)
{
  if (yi < 0) return false;
  if (yi >= img->ysize) return false;
  if (xi < 0) return false;
  if (xi >= img->xsize) return false;
  return true;
}

int NeighboursOK(Image img, Image mrk,
                 int marker, int xa, int ya, int aval)
{
  int ok = true;
  int xi, yi;
  for (yi = ya - 1; yi <= ya + 1; yi++)
    for (xi = xa - 1; xi <= xa + 1; xi++)
      {
        if (insidep(img, xi, yi))
          {
            if (GetVal(mrk, xi, yi) < mrk->maxval - 1)
              {
                if (GetVal(mrk, xi, yi) == 0)
                  {
                    if (GetVal(img, xi, yi) > aval) return false;
                  }
                else if (GetVal(mrk, xi, yi) != marker) return false;
              }
          }
      }
  return ok;
}

void CollectNeighbours(Image img, Image mrk, int xa, int ya, pointheap& pl)
{
  int xi, yi;
  for (yi = ya - 1; yi <= ya + 1; yi++)
    for (xi = xa - 1; xi <= xa + 1; xi++)
      {
        if (insidep(img, xi, yi))
          {
            if (GetVal(mrk, xi, yi) == 0)
              {
                pl.insert(xi, yi, GetVal(img, xi, yi));
                PutVal(mrk, xi, yi, mrk->maxval);
              }
          }
      }
}

void ClearHMarker(Image mrk)
{
  int xa, ya;
  wloop(mrk, xa, ya)
  if (GetVal(mrk, xa, ya) >= mrk->maxval - 1) PutVal(mrk, xa, ya, 0);
}

void MarkMax(Image img, Image mrk, int xmax, int ymax, int marker, int mode)
{
  int nullval = (img->maxval + 1) / 2; // Reference level (logical 0)
  if ((mode & P_POSITIVE) || (mode & P_NEGATIVE)) nullval = 0;

  int mval = GetVal(img, xmax, ymax) - nullval; // height of maximum

  pointheap pl;
  pl.insert(xmax, ymax, GetVal(img, xmax, ymax));

  int xa, ya, vala;
  bool RandErreicht = false;
  while (!RandErreicht && (pl.getmaximum(xa, ya, vala) != ERROR))
    {
      pl.delmaximum();
      PutVal(mrk, xa, ya, mrk->maxval - 1); // point processed
      if (vala - nullval > mval / 5) // point maybe part of maximum
        {
          if (NeighboursOK(img, mrk, marker, xa, ya, vala)) // no higher unmarked point
            {
              PutVal(mrk, xa, ya, marker);
              CollectNeighbours(img, mrk, xa, ya, pl);
            }
          else
            RandErreicht = true;
        }
    }
  ClearHMarker(mrk);
}

void Peak(Image imgt, int mode, double& xmax, double& ymax, double& eval, int mdist)
{
  Image mrk = NewImg(wsizex(imgt), wsizey(imgt), 7);
  Image pimg = NewImg(wsizex(imgt), wsizey(imgt), imgt->maxval);

  int nullval = (imgt->maxval + 1) / 2; // Reference level (logical 0)
  if ((mode & P_POSITIVE) || (mode & P_NEGATIVE)) nullval = 0;

  int x, y;
  int xmaxi = -1, ymaxi = -1;
  int gmax, gmax2;

  ClearImg(mrk);
//  Show(OFF,imgt);
  Show(OVERLAY, pimg, mrk);

  wloop(pimg, x, y)
  {
    int x1, y1, x2, y2;
    int xi, yi;
    int gsum;
    int ct;
    x1 = Max(0, x - mdist);
    y1 = Max(0, y - mdist);
    x2 = Min(imgt->xsize - 1, x + mdist);
    y2 = Min(imgt->ysize - 1, y + mdist);
    ct = 0;
    gsum = 0;
    for (xi = x1; xi <= x2; xi++)
      for (yi = y1; yi <= y2; yi++)
        {
          gsum += GetValw(imgt, xi, yi);
          ct++;
        }
    if ((mode & P_MIN) != 0)
      PutVal(pimg, x, y, pimg->maxval - gsum / ct);
    else
      PutVal(pimg, x, y, gsum / ct);
  }

  gmax = -1;
  wloop(pimg, x, y)
  {
    int v = GetVal(pimg, x, y);
    if (v > gmax)
      {
        gmax = v;
        xmaxi = x;
        ymaxi = y;
      }
  }

  if ((mode & P_EVAL) == 0)
    {
      xmax = xmaxi + imgt->wxi;
      ymax = ymaxi + imgt->wyi;
      FreeImg(pimg);
      FreeImg(mrk);
      return;
    }

  MarkMax(pimg, mrk, xmaxi, ymaxi, 1, mode);

  gmax2 = -1;
  wloop(pimg, x, y)
  {
    if (GetVal(mrk, x, y) == 0)
      {
        int v = GetVal(pimg, x, y);
        if (v > gmax2)
          {
            gmax2 = v;
          }
      }
  }

  Printf("gmax:%d gmax2:%d\n", gmax, gmax2);

  if (mode & P_INTERPOL)
    {
      Polynom2D2O peakfunction;
      peakfunction.FitInit();
      wloop(pimg, x, y)
      {
        if (GetVal(mrk, x, y) == 1)
          {
            peakfunction.FitVal(x, y, GetVal(pimg, x, y));
          }
      }
      double a20, a11, a02, a10, a01, a00;
      peakfunction.Coeff(a00, a10, a01, a20, a11, a02);
      Matrix a(2, 2);
      Vector i(2);
      Vector xv(2);
      a[0][0] = 2 * a20;
      a[0][1] = a11;
      i[0] = -a10;
      a[1][0] = a11;
      a[1][1] = 2 * a02;
      i[1] = -a01;
      xv = SolveLinEqu(a, i);
      Printf("xi:%d yi:%d  xs:%5.2f ys:%5.2f\n", xmaxi, ymaxi, xv[0], xv[1]);
      xmax = xmaxi + imgt->wxi;
      ymax = ymaxi + imgt->wyi;
    }
  else
    {
      xmax = xmaxi + imgt->wxi;
      ymax = ymaxi + imgt->wyi;
    }
//  GetChar();
  eval = 1.0 - ((double)gmax2 - nullval) / (gmax - nullval);
  FreeImg(pimg);
  FreeImg(mrk);
  return;
}

void Peak(Image img, int mode, int& xmax, int& ymax, double& eval, int mdist)
{
  double xm, ym;
  Peak(img, mode, xm, ym, eval, mdist);
  xmax = RoundInt(xm);
  ymax = RoundInt(ym);
}
