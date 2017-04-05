/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//**************************************************************
// Bestimmung von Schwellen für Kontur- Kanten- und Liniensuche
// (Methode von Otsu)
// m. schubert 2/97
// Ortmann 11/99,9/07
//**************************************************************

#include <limits>
#include <vector>

#include "IceException.h"
#include "macro.h"
#include "hist.h"
#include "threshld.h"

using namespace std;
namespace ice
{
//
// Schwelle aus Histogramm bestimmen
//
#define FNAME "CalcThreshold"
  double CalcThreshold(const Hist& hist, double& discmax)
  {
    double m1;
    double s1, c1, mu1, mu2;
    double sig1, sig2, disc, p1, p2;
    double valmin, valmax;
    double ct;
    int topt;

    if (!hist.valid())
      throw IceException(FNAME, M_NOT_INITIALISED);

    int n = hist.classes();

    double m2 = 0;
    double s2 = 0;
    double c2 = 0; // analyse histogram and calculate starting
    // value for parameters of second class
    int nn = 0;    // number of used histogram classes

    for (int i = 1; i <= n; i++)
      {
        ct = hist.Count(i);

        if (ct > 0)
          {
            nn++;
          }

        c2 += ct;
        m2 += ct * i;
        s2 += ct * i * i;
      }

    if (nn == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    if (nn == 1)
      {
        discmax = 0.0;

        int i = 1;
        while (hist.Count(i) == 0)
          {
            i++;
          }

        hist.ClassValue(i, valmin, valmax);
        return (valmax + valmin) / 2.0;
      }

    if (nn == 2)
      {
        // two histogram classes -> two classes after binarization
        int i1 = -1;
        int i2 = -1;

        for (int i = 1; i <= n; i++)
          {
            ct = hist.Count(i);

            if (ct > 0)
              {
                if (i1 < 0)
                  {
                    i1 = i;
                  }
                else
                  {
                    i2 = i;
                  }
              }
          }

        discmax = 1000; // a high value
        hist.ClassValue((i1 + i2 + 1) / 2, valmin, valmax);
        return valmin;
      }

    m1 = 0;
    s1 = 0;
    c1 = 0;
    topt = -1;
    discmax = 0;

    for (int t = 2; t <= n; t++)
      {
        ct = hist.Count(t - 1);
        c1 += ct;
        m1 += ct * (t - 1);
        s1 += ct * (t - 1) * (t - 1);
        c2 -= ct;
        m2 -= ct * (t - 1);
        s2 -= ct * (t - 1) * (t - 1);

        if ((c1 > 0) && (c2 > 0))
          {
            p1 = c1 / (c1 + c2);
            p2 = 1 - p1;
            mu1 = m1 / c1;
            sig1 = s1 / c1 - mu1 * mu1;
            mu2 = m2 / c2;
            sig2 = s2 / c2 - mu2 * mu2;
            disc = p1 * p2 * Sqr(mu1 - mu2) / (p1 * sig1 + p2 * sig2 + 1);

            if (disc > discmax)
              {
                discmax = disc;
                topt = t;
              }
          }
      }

    hist.ClassValue(topt, valmin, valmax);
    return valmin;
  }

  double CalcThreshold(const Hist& hist)
  {
    double disc;
    return CalcThreshold(hist, disc);
  }

  int CalcThreshold(const Histogram& hist, double& discmax)
  {
    double m1, m2;
    double s1, s2, c1, c2;

    int topt;

    if (!hist.isValid())
      throw IceException(FNAME, M_NOT_INITIALISED);

    int n = hist.nClasses();

    m2 = 0;
    s2 = 0;
    c2 = 0; // analyse histogram and calculate starting
    // value for parameters of second class

    int nn = 0; // number of used histogram classes

    for (int i = 0; i < n; i++)
      {
        double ct = hist.getCount(i);

        if (ct > 0)
          {
            nn++;
          }

        c2 += ct;
        m2 += ct * i;
        s2 += ct * i * i;
      }

    if (nn == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    if (nn == 1)
      {
        discmax = 0.0;
        int i = 0;

        while (hist.getCount(i) == 0)
          {
            i++;
          }

        return i;
      }

    if (nn == 2)
      {
        // two histogram classes -> two classes after binarization
        int i = 0;
        while (hist.getCount(i) == 0)
          {
            i++;
          }
        int i1 = i;
        double c1 = hist.getCount(i1);
        i++;
        while (hist.getCount(i) == 0)
          {
            i++;
          }
        int i2 = i;
        double c2 = hist.getCount(i2);
        double ct = c1 + c2;
        // p1 * p2 * Sqr(mu1 - mu2) / (p1 * sig1 + p2 * sig2 + 1);

        discmax = c1 * c2 / (ct * ct) * (i2 - i1);
        return (i1 + i2 + 1) / 2;
      }

    m1 = 0;
    s1 = 0;
    c1 = 0;

    topt = -1;
    discmax = 0;

    for (int t = 0; t < n; t++)
      {
        double ct = hist.getCount(t);
        double ctt = ct * t;
        double cttt = ctt * t;

        c1 += ct;
        m1 += ctt;
        s1 += cttt;

        c2 -= ct;
        m2 -= ctt;
        s2 -= cttt;

        if ((c1 > 0) && (c2 > 0))
          {
            double p1 = c1 / (c1 + c2);
            double p2 = 1.0 - p1;
            double mu1 = m1 / c1;
            double sig1 = s1 / c1 - mu1 * mu1;
            double mu2 = m2 / c2;
            double sig2 = s2 / c2 - mu2 * mu2;
            double disc = p1 * p2 * Sqr(mu1 - mu2) / (p1 * sig1 + p2 * sig2 + 1);

            if (disc > discmax)
              {
                discmax = disc;
                topt = t + 1;
              }
          }
      }

    return topt;
  }

  int CalcThreshold(const Histogram& hist)
  {
    double disc;
    return CalcThreshold(hist, disc);
  }

  double calcDisc(const vector<double>& value, double threshold)
  {
    double c1 = 0, m1 = 0, s1 = 0;
    double c2 = 0, m2 = 0, s2 = 0;
    for (unsigned int i = 0; i < value.size(); ++i)
      {
        double t = value[i];
        if (t < threshold)
          {
            c1 += 1;
            m1 += t;
            s1 += t * t;
          }
        else
          {
            c2 += 1;
            m2 += t;
            s2 += t * t;
          }
      }

    double disc = 0;
    if (c1 > 0 && c2 > 0)
      {
        double p1 = c1 / (c1 + c2);
        double p2 = 1.0 - p1;
        double mu1 = m1 / c1;
        double sig1 = s1 / c1 - mu1 * mu1;
        double mu2 = m2 / c2;
        double sig2 = s2 / c2 - mu2 * mu2;
        double deltaMu = mu1 - mu2;
        disc = p1 * p2 * deltaMu * deltaMu / (p1 * sig1 + p2 * sig2 + 1);
      }
    return disc;
  }

  double CalcThreshold(const vector<double>& value, double& discmax)
  {
    discmax = 0;
    double threshold = value[0];
    for (unsigned int i = 0; i < value.size(); ++i)
      {
        double t = value[i];
        double disc = calcDisc(value, t);
        if (disc > discmax)
          {
            threshold = t;
            discmax = disc;
          }
      }
    if (discmax > 0)
      {
        double max1 = std::numeric_limits<double>::min();
        for (unsigned int i = 0; i < value.size(); ++i)
          {
            double t = value[i];
            if (t < threshold)
              {
                max1 = t;
              }
          }
        threshold = (threshold + max1) / 2;
      }
    return threshold;
  }

  double calcDisc(const vector<int>& value, int threshold)
  {
    long int c1 = 0, m1 = 0, s1 = 0;
    long int c2 = 0, m2 = 0, s2 = 0;
    for (unsigned int i = 0; i < value.size(); ++i)
      {
        int t = value[i];
        if (t < threshold)
          {
            c1 += 1;
            m1 += t;
            s1 += t * t;
          }
        else
          {
            c2 += 1;
            m2 += t;
            s2 += t * t;
          }
      }

    double disc = 0;
    if (c1 > 0 && c2 > 0)
      {
        double p1 = static_cast<double>(c1) / (c1 + c2);
        double p2 = 1.0 - p1;
        double mu1 = static_cast<double>(m1) / c1;
        double sig1 = static_cast<double>(s1) / c1 - mu1 * mu1;
        double mu2 = static_cast<double>(m2) / c2;
        double sig2 = static_cast<double>(s2) / c2 - mu2 * mu2;
        double deltaMu = mu1 - mu2;
        disc = p1 * p2 * deltaMu * deltaMu / (p1 * sig1 + p2 * sig2 + 1);
      }
    return disc;
  }

  int CalcThreshold(const vector<int>& value, double& discmax)
  {
    discmax = 0;
    int threshold = value[0];
    for (unsigned int i = 0; i < value.size(); ++i)
      {
        int t = value[i];
        double disc = calcDisc(value, t);
        if (disc > discmax)
          {
            threshold = t;
            discmax = disc;
          }
      }
    if (discmax > 0)
      {
        int max1 = std::numeric_limits<double>::min();
        for (unsigned int i = 0; i < value.size(); ++i)
          {
            int t = value[i];
            if (t < threshold)
              {
                max1 = t;
              }
          }
        threshold = (threshold + max1 + 1) / 2;
      }
    return threshold;
  }

//
// Schwelle für Konturfolge / Binarisierung
//

  int CalcThreshold(const Image& img, double& discmax, int diff)
  {
    int xi, yi, xa, ya;
    int part = diff;

    xi = 0;
    xa = img->xsize - 1;
    yi = 0;
    ya = img->ysize - 1;

    if ((xa - xi) < 20 * part)
      {
        part = 1;
      }

    if ((ya - yi) < 20 * part)
      {
        part = 1;
      }

    Histogram hist(img, part);

    return CalcThreshold(hist, discmax);
  }

  int CalcThreshold(const Image& img)
  {
    double disc;
    return CalcThreshold(img, disc);
  }
#undef FNAME
//
// Schwelle für Kantenverfolgung SearchGradStart()
//
#define FNAME "GradThreshold"
#define PART 10             // Bildpunktanteil (%) für Gradientenberechnung

  int GradThreshold(const Image& img)
  {
    int x, y, xi, yi, xa, ya;
    double grd;
    Hist h;
    int diffx, diffy;

    xi = 1;
    xa = img->xsize - 2;
    yi = 1;
    ya = img->ysize - 2;

    if ((xi >= xa) || (yi >= ya))
      {
        return (-1);
      }

    diffx = (xa - xi) * PART / 100;
    diffy = (ya - yi) * PART / 100;

    h.Reset((int)ceil((img->maxval + 1) * 1.415)); /* max. Grad: sqrt(2)*maxval */

    for (y = yi; y <= ya; y += diffy)
      {
        for (x = xi; x <= xa; x++)
          {
            grd = (sqrt(Sqr(GetVal(img, x + 1, y) - GetVal(img, x - 1, y)) + \
                        Sqr(GetVal(img, x, y + 1) - GetVal(img, x, y - 1))));
            h.Add(grd);
          }
      }

    for (x = xi; x <= xa; x += diffx)
      {
        for (y = yi; y <= ya; y++)
          {
            grd = (sqrt(Sqr(GetVal(img, x + 1, y) - GetVal(img, x - 1, y)) + \
                        Sqr(GetVal(img, x, y + 1) - GetVal(img, x, y - 1))));
            h.Add(grd);
          }
      }

    return RoundInt(CalcThreshold(h));
  }
#undef PART
#undef FNAME
//
// Schwelle für Linienverfolgung SearchRidgeStart()
//
#define FNAME "RidgeThreshold"
#define PART 10                     // Bildpunktanteil (%) für Laplaceberechng
  int RidgeThreshold(const Image& img)
  {
    int x, y, xi, yi, xa, ya;
    double lpl;
    Hist hist;
    int diffx, diffy;

    xi = 2;
    xa = img->xsize - 3;
    yi = 2;
    ya = img->ysize - 3;

    if ((xi >= xa) || (yi >= ya))
      {
        return (-1);
      }

    diffx = (xa - xi) * PART / 100;
    diffy = (ya - yi) * PART / 100;
    hist.Reset((img->maxval + 1) * 6);

    for (y = yi; y <= ya; y += diffy)
      {
        for (x = xi; x <= xa; x++)
          {
            lpl = (6 * GetVal(img, x, y) - 2 * (GetVal(img, x - 1, y) + GetVal(img, x + 1, y)) - \
                   GetVal(img, x - 2, y) - GetVal(img, x + 2, y));
            hist.Add(lpl);
          }
      }

    for (x = xi; x <= xa; x += diffx)
      {
        for (y = yi; y <= ya; y++)
          {
            lpl = (6 * GetVal(img, x, y) - 2 * (GetVal(img, x, y - 1) + GetVal(img, x, y + 1)) - \
                   GetVal(img, x, y - 2) - GetVal(img, x, y + 2));
            hist.Add(lpl);
          }
      }

    return RoundInt(CalcThreshold(hist) / 6);
  }
#undef PART
#undef FNAME
//
}
