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

#include <fstream>
#include <sstream>

#include "strtool.h"
#include "Trafo.h"
#include "geo.h"
#include "lmdif.h"
#include "macro.h"
#include "geo.h"
//#include "bairstow.h"

#include "Distortion.h"

using namespace std;

namespace ice
{
  /*************************************************************
   * abstract base class Distortion
   *************************************************************/

  bool Distortion::readPara(std::istream& is, const string& name, double& val)
  {
    string s;
    is >> s ;

    if (s != name + ":")
      {
        return false;
      }

    if (is >> val)
      {
        return true;
      }

    return false;
  }

#define FNAME "Distortion::distort"
  int Distortion::distort(double& xd, double& yd) const
  {
    return distort(xd, yd, xd, yd);
  }

  Vector Distortion::distort(const Vector& v) const
  {
    Vector res(2);

    if (v.size() != 2)
      throw IceException(FNAME, M_WRONG_DIM);

    distort(v[0], v[1], res[0], res[1]);
    return res;
  }

  Point Distortion::distort(const Point& p) const
  {
    Point res;
    distort(p.x, p.y, res.x, res.y);
    return res;
  }

  int Distortion::distort(vector<Point>& pl) const
  {
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        pl[i] = distort(pl[i]);
      }
    return OK;
  }

#undef FNAME
#define FNAME "Distortion::rectify"
  int Distortion::rectify(double& xd, double& yd) const
  {
    return rectify(xd, yd, xd, yd);
  }

  Vector Distortion::rectify(const Vector& v) const
  {
    Vector result = v;

    if (v.size() != 2)
      throw IceException(FNAME, M_WRONG_DIM);

    rectify(v[0], v[1], result[0], result[1]);
    return result;
  }

  Point Distortion::rectify(const Point& p) const
  {
    Point result;
    rectify(p.x, p.y, result.x, result.y);
    return result;
  }

#undef FNAME
#define FNAME "Distortion::rectifyImage"
  Image Distortion::rectifyImage(const Image& source, int mode) const
  {
    Image dest = NewImg(source);
    rectifyImage(source, dest, mode);
    CopyImg(dest, source);
    return source;
  }

  int Distortion::rectifyImage(const Image& source, const Image& dest, int mode) const
  {
    if (!IsImg(source) || !IsImg(dest))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (dest.maxval != source.maxval)
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    if (source == dest)
      {
        Image newimg = NewImg(source);
        rectifyImage(source, newimg, mode);
        CopyImg(newimg, dest);
        return OK;
      }

    for (int yd = 0; yd < dest->ysize; yd++)
      for (int xd = 0; xd < dest->xsize; xd++)
        {
          double xo, yo;
          distort(xd, yd, xo, yo);
          int xoi = RoundInt(xo);
          int yoi = RoundInt(yo);
          // cout << xoi << " " << yoi << endl;
          if (source.inside(xoi, yoi))
            {
              if (mode == DEFAULT)
                {
                  PutVal(dest, xd, yd, GetVal(source, xoi, yoi));
                }
              else
                {
                  PutVal(dest, xd, yd, RoundInt(GetInterpolVal(source, xo, yo)));
                }
            }
        }

    return OK;
  }
#undef FNAME

  class DistError: public LMFunctionObject
  {
  private:
    Distortion& di;
    Vector& para;

    const vector<Point>& marker;
    const vector<Point>& orig;

    int points;
    int dipara;
  public:
    DistError(Distortion& di, Vector& para,
              const vector<Point>& marker, const vector<Point>& orig):
      di(di), para(para), marker(marker), orig(orig)
    {
      points = marker.size();
      dipara = di.makeVector().size();
    }

    int operator()(ice::Vector& res) const
    {
      // cout << para << endl;

      int pidx = 0;

      // erster teilvektor für Verzeichnung
      Vector paravec(dipara);
      for (int i = 0; i < dipara; i++)
        {
          paravec[i] = para[pidx++];
        }

      di.set(paravec);

      double m11 = para[pidx++];
      double m12 = para[pidx++];
      double m13 = para[pidx++];
      double m21 = para[pidx++];
      double m22 = para[pidx++];
      double m23 = para[pidx++];
      double m31 = para[pidx++];
      double m32 = para[pidx++];

      int r = 0;

      for (int i = 0; i < points; i++)
        {
          double xo = orig.at(i).x;
          double yo = orig.at(i).y; // Original-Punkte

          double xm = marker.at(i).x;
          double ym = marker.at(i).y; // Beobachtete Punkte

          double zh =  m31 * xo + m32 * yo + 1.0;        // apply Homography
          double xi = (m11 * xo + m12 * yo + m13) / zh;
          double yi = (m21 * xo + m22 * yo + m23) / zh;

          double xv, yv;
          di.distort(xi, yi, xv, yv);           // apply Distortion

          res.at(r) = xv - xm; // error vector
          r++;
          res.at(r) = yv - ym;
          r++;
        }

      return 1;
    }

    int funcdim() const
    {
      return 2 * points;
    }
  };

#define FNAME "Distortion::calculate"
  int Distortion::calculate(const vector<Point>& marker,
                            const vector<Point>& orig,
                            Trafo& tr, Point& center)
  {
    Vector dipara = makeVector();
    int diparams = dipara.size();

    Vector parvec(diparams + 8);

    // Start value
    // given center
    dipara[0] = center.x;
    dipara[1] = center.y;

    // no distortion
    for (int i = 2; i < diparams; i++)
      {
        dipara[i] = 0.0;
      }

    set(dipara);

    // init parameter vector
    int k = 0;

    // distortion
    for (int i = 0; i < diparams; i++)
      {
        parvec[k++] = dipara[i];
      }

    matrix<double> tm = tr.getMatrix();

    // homographie
    parvec[k++] = tm[0][0] / tm[2][2];
    parvec[k++] = tm[0][1] / tm[2][2];
    parvec[k++] = tm[0][2] / tm[2][2];
    parvec[k++] = tm[1][0] / tm[2][2];
    parvec[k++] = tm[1][1] / tm[2][2];
    parvec[k++] = tm[1][2] / tm[2][2];
    parvec[k++] = tm[2][0] / tm[2][2];
    parvec[k++] = tm[2][1] / tm[2][2];

    vector<double*> op;

    for (unsigned int i = 0; i < parvec.size(); i++)
      {
        op.push_back(&parvec[i]);
      }

    int inumber;

    int rc = LMDif(op, DistError(*this, parvec, marker, orig), 10000, inumber);

    if (rc > 4)
      throw IceException(FNAME, M_NUM_INSTABILITY);
    else
      {
        // store calculated parameters of distortion
        k = 0;

        for (int i = 0; i < diparams; i++)
          {
            dipara[i] = parvec[k++];
          }

        center.x = dipara[0];
        center.y = dipara[1];

        set(dipara);

        // .. and projective transform
        tm[0][0] = parvec[k++];
        tm[0][1] = parvec[k++];
        tm[0][2] = parvec[k++];
        tm[1][0] = parvec[k++];
        tm[1][1] = parvec[k++];
        tm[1][2] = parvec[k++];
        tm[2][0] = parvec[k++];
        tm[2][1] = parvec[k++];
        tm[2][2] = 1.0;
        tr = Trafo(tm);
      }

    return OK;
  }

  // error function for multiple reference lists
  class DistErrorM: public LMFunctionObject
  {
  private:
    Distortion& di;
    Vector& para;
    const vector<vector<Point> >& marker;
    const vector<vector<Point> >& orig;
    int nrlist;
    int nrpoint;
    int nr_distortion_parameter;
  public:
    DistErrorM(Distortion& di, Vector& para,
               const vector<vector<Point> >& marker,
               const vector<vector<Point> >& orig):
      di(di), para(para), marker(marker), orig(orig)
    {
      nrlist = marker.size();

      nrpoint = 0;
      for (int i = 0; i < nrlist; i++)
        {
          nrpoint += marker[i].size();
        }

      nr_distortion_parameter = di.makeVector().size();
    }

    int operator()(ice::Vector& res) const
    {
      //      cout << para << endl;
      // erster teilvektor für Verzeichnung
      Vector distortionParameter(nr_distortion_parameter);

      int parameterIndex = 0;

      for (int k = 0; k < nr_distortion_parameter; k++)
        {
          distortionParameter[k] = para[parameterIndex++];
        }

      di.set(distortionParameter);

      int residx = 0;

      for (int listnr = 0; listnr < nrlist; listnr++)
        {
          double m11 = para[parameterIndex++];
          double m12 = para[parameterIndex++];
          double m13 = para[parameterIndex++];
          double m21 = para[parameterIndex++];
          double m22 = para[parameterIndex++];
          double m23 = para[parameterIndex++];
          double m31 = para[parameterIndex++];
          double m32 = para[parameterIndex++];

          for (unsigned int n = 0; n < marker[listnr].size(); n++)
            {
              double xo = orig[listnr][n].x;
              double yo = orig[listnr][n].y; // Original-Punkte

              double xm = marker[listnr][n].x;
              double ym = marker[listnr][n].y; // beobachtete Punkte

              double zh =  m31 * xo + m32 * yo + 1.0;        // apply Homography
              double xi = (m11 * xo + m12 * yo + m13) / zh;
              double yi = (m21 * xo + m22 * yo + m23) / zh;

              double xv, yv;
              di.distort(xi, yi, xv, yv);           // apply Distortion

              res[residx++] = xv - xm; // error vector
              res[residx++] = yv - ym;
            }
        }
      return 1;
    }

    int funcdim() const
    {
      return 2 * nrpoint;
    }
  };

  int Distortion::calculate(const std::vector<std::vector<Point> >& marker,
                            const std::vector<std::vector<Point> >& orig)
  {
    Vector distortion_parameters = makeVector();
    int nr_distortion_parameter = distortion_parameters.size();

    int nrlist = marker.size();
    if ((int)orig.size() != nrlist)
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

    Vector parameterVector(nr_distortion_parameter + 8 * nrlist);

    // Start values

    // distortion

    // estimate center from pointlist
    int ct = 0;
    double xs = 0;
    double ys = 0;
    for (int listnr = 0; listnr < nrlist; listnr++)
      {
        int nrpoint = marker[listnr].size();
        for (int pointnr = 0; pointnr < nrpoint; pointnr++)
          {
            xs += marker[listnr][pointnr].x;
            ys += marker[listnr][pointnr].y;
            ct++;
          }
      }

    distortion_parameters[0] = xs / ct;
    distortion_parameters[1] = ys / ct;

    // no distortion
    for (int i = 2; i < nr_distortion_parameter; i++)
      {
        distortion_parameters[i] = 0.0;
      }

    set(distortion_parameters);

    // init parameter vector
    int k = 0;

    // distortion
    for (int i = 0; i < nr_distortion_parameter; i++)
      {
        parameterVector[k++] = distortion_parameters[i];
      }

    // homographies
    for (int listnr = 0; listnr < nrlist; listnr++)
      {
        // startlösung wird ohne Berücksichtigung der Verzeichnung berechnet
        Trafo tr = matchPointLists(orig[listnr], marker[listnr]);
        Matrix tm(tr.getMatrix());
        // cout << tm << endl;
        parameterVector[k++] = tm[0][0] / tm[2][2];
        parameterVector[k++] = tm[0][1] / tm[2][2];
        parameterVector[k++] = tm[0][2] / tm[2][2];
        parameterVector[k++] = tm[1][0] / tm[2][2];
        parameterVector[k++] = tm[1][1] / tm[2][2];
        parameterVector[k++] = tm[1][2] / tm[2][2];
        parameterVector[k++] = tm[2][0] / tm[2][2];
        parameterVector[k++] = tm[2][1] / tm[2][2];
      }

    vector<double*> op;

    for (unsigned int i = 0; i < parameterVector.size(); i++)
      {
        op.push_back(&parameterVector[i]);
      }

    int inumber;

    int rc = LMDif(op, DistErrorM(*this, parameterVector, marker, orig), 10000, inumber);
#if 0
    cout << LMDifMessage(rc) << endl;
    cout << inumber << " Iterationen" << endl;
#endif
    if (rc > 4)
      throw IceException(FNAME, M_NUM_INSTABILITY);
    else
      {
        // store calculated parameters of distortion
        k = 0;

        for (int i = 0; i < nr_distortion_parameter; i++)
          {
            distortion_parameters[i] = parameterVector[k++];
          }

        set(distortion_parameters);
      }

    return OK;
  }

  int Distortion::calculate(const Matrix& marker, const Matrix& orig,
                            Trafo& tr, double mx, double my)
  {
    vector<Point> mpl(marker.rows());
    for (int i = 0; i < marker.rows(); i++)
      {
        mpl[i] = Point(marker[i][0], marker[i][1]);
      }

    vector<Point> opl(orig.rows());
    for (int i = 0; i < orig.rows(); i++)
      {
        opl[i] = Point(orig[i][0], orig[i][1]);
      }

    Point center(mx, my);
    RETURN_ERROR_IF_FAILED(calculate(mpl, opl, tr, center));

    return OK;
  }

  int Distortion::calculate(const vector<Point>& marker,
                            const vector<Point>& orig,
                            Trafo& tr)
  {
    // Schätzung: Mittelpunkt der Verzeichnung ist Mittelpunkt der Markermenge

    int npoints = marker.size();

    Point center(0, 0);

    for (int i = 0; i < npoints; i++)
      {
        center += marker[i];
      }

    center /= npoints;

    return calculate(marker, orig, tr, center);
  }

  int Distortion::calculate(const Matrix& marker, const Matrix& orig, Trafo& tr)
  {
    vector<Point> mpl(marker.rows());
    for (int i = 0; i < marker.rows(); i++)
      {
        mpl[i] = Point(marker[i][0], marker[i][1]);
      }

    vector<Point> opl(orig.rows());
    for (int i = 0; i < orig.rows(); i++)
      {
        opl[i] = Point(orig[i][0], orig[i][1]);
      }

    RETURN_ERROR_IF_FAILED(calculate(mpl, opl, tr));
    return OK;
  }

  int Distortion::calculate(const vector<Point>& marker, const vector<Point>& orig)
  {
    Trafo tr = matchPointLists(orig, marker, TRM_PROJECTIVE);
    return calculate(marker, orig, tr);
  }

  int Distortion::calculate(const Matrix& marker, const Matrix& orig)
  {
    vector<Point> mpl(marker.rows());
    for (int i = 0; i < marker.rows(); i++)
      {
        mpl[i] = Point(marker[i][0], marker[i][1]);
      }

    vector<Point> opl(orig.rows());
    for (int i = 0; i < orig.rows(); i++)
      {
        opl[i] = Point(orig[i][0], orig[i][1]);
      }

    RETURN_ERROR_IF_FAILED(calculate(mpl, opl));
    return OK;
  }

  int Distortion::calculate(const Matrix& marker, const Matrix& orig, double xm, double ym)
  {
    Trafo tr = MatchPointlists(orig, marker, TRM_PROJECTIVE);
    return calculate(marker, orig, tr, xm, ym);
  }
#undef FNAME
}
