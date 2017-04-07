#include <iostream>
#include <sstream>

#include "macro.h"
#include "MatrixAlgebra.h"

#include "Distortion1.h"
#include "Distortion2.h"
#include "Distortion3.h"

#include "Camera.h"

using namespace std;

// Konstruktoren
namespace ice
{
  void Camera::newdist(int dtyp)
  {
    disttyp = dtyp;

    switch (dtyp)
      {
      case 1:
        dist = new Distortion1();
        break;
      case 2:
        dist = new Distortion2();
        break;
      case 3:
        dist = new Distortion3();
        break;
      default:
        cerr << "Wrong Distortiontyp" << endl;
        break;
      }
  }

  void Camera::newdist(int dtyp, Distortion* d)
  {
    disttyp = dtyp;

    switch (dtyp)
      {
      case 1:
        dist = new Distortion1(*(Distortion1*)(d));
        break;
      case 2:
        dist = new Distortion2(*(Distortion2*)(d));
        break;
      case 3:
        dist = new Distortion3(*(Distortion3*)(d));
        break;
      }
  }

  void Camera::assign(const Distortion& d)
  {
    switch (disttyp)
      {
      case 1:
        dist = new Distortion1((Distortion1&)d);
        break;
      case 2:
        dist = new Distortion2((Distortion2&)d);
        break;
      case 3:
        dist = new Distortion3((Distortion3&)d);
        break;
      }
  }

  Camera::Camera(int dtyp): f(1000), a(1.0), s(0), u0(0), v0(0),
    dx(0), dy(0), dz(1000),
    alpha(0), beta(0), gamma(0),
    c_val(false)
  {
    newdist(dtyp);
  }

  Camera::Camera(const Camera& c): f(c.f), a(c.a), s(c.s), u0(c.u0), v0(c.v0),
    dx(c.dx), dy(c.dy), dz(c.dz),
    alpha(c.alpha), beta(c.beta), gamma(c.gamma),
    c_val(false)
  {
    newdist(c.disttyp, c.dist);
  }

  void Camera::swap(Camera& c)
  {
    std::swap(f, c.f);
    std::swap(a, c.a);
    std::swap(s, c.s);
    std::swap(u0, c.u0);
    std::swap(v0, c.v0);
    std::swap(dx, c.dx);
    std::swap(dy, c.dy);
    std::swap(dz, c.dz);
    std::swap(alpha, c.alpha);
    std::swap(beta, c.beta);
    std::swap(gamma, c.gamma);
    c_val = false;
    c.c_val = false;
    std::swap(disttyp, c.disttyp);
    std::swap(dist, c.dist);
  }

  Camera& Camera::operator=(Camera c)
  {
    swap(c);
    return *this;
  }

// create camera description as string
  string Camera::toString(const string& del) const
  {
    ostringstream os;

    os << "f: " << f << " ";
    os << "a: " << a << " ";
    os << "s: " << s << del ;

    os << "u0: " << u0 << " ";
    os << "v0: " << v0 << del ;

    os << "dx: " << dx << " ";
    os << "dy: " << dy << " ";
    os << "dz: " << dz << " " << del;

    os << "fx: " << Degree(gamma) << " ";
    os << "fy: " << Degree(beta) << " ";
    os << "fz: " << Degree(alpha) << del ;

    os << "dist: " << del << dist->toString() << del;

    return os.str();
  }

// create transformation from camera parameters
  void Camera::create_trans() const
  {
    // must be called, before trafo tr is used
    // all methods, that change parameters, must
    // set c_val to false!
    if (c_val)
      {
        return;
      }

    // Trafo zusammensetzen
    tr = Trafo(3, 3);

    // Bewegung Weltkoordinaten -> Kamerakoordinaten
    // x_w -> x_k
    tr.shift(-dx, -dy, -dz);

    tr.rotateZ(alpha);
    tr.rotateY(beta);
    tr.rotateX(gamma);

    // Transformation Kamerakoordinaten -> (unverzerrte) Bildkoordinaten
    // x_k -> uv
    // effective Kamera-Matrix
    //
    //    /   f    f*s    u0   \     -
    //    |   0   -a*f    v0   |     -
    //    \   0    0      1    /     -
    //

    tr.Projective(); // 3d -> 2d
    tr.shearX(s);
    tr.scale(0, 0, f, -a * f);
    tr.shift(u0, v0);

    c_val = true;
  }

// put camera parameters into Vector (for lmdiff,...)
  Vector Camera::MakeVector(int what) const
  {
    Vector res(0);

    if ((what == internal) || (what == all))
      {
        res.Append(f);
        res.Append(a);
        res.Append(s);

        res.Append(u0);
        res.Append(v0);
      }

    if ((what == external) || (what == all))
      {
        res.Append(dx);
        res.Append(dy);
        res.Append(dz);

        res.Append(alpha);
        res.Append(beta);
        res.Append(gamma);
      }

    if ((what == all) || (what == internal))
      {
        res.Append(dist->makeVector());
      }

    return res;
  }

// Parameter nach Vektor setzen
  void Camera::set(const Vector& res, int what)
  {
    int i = 0;

    if ((what == internal) || (what == all))
      {
        f = res[i];
        i++;
        a = res[i];
        i++;
        s = res[i];
        i++;
        u0 = res[i];
        i++;
        v0 = res[i];
        i++;
      }

    if ((what == external) || (what == all))
      {
        dx = res[i];
        i++;
        dy = res[i];
        i++;
        dz = res[i];
        i++;
        alpha = res[i];
        i++;
        beta = res[i];
        i++;
        gamma = res[i];
        i++;
      }

    if ((what == all) || (what == internal))
      {
        switch (disttyp)
          {
          case 1:
            dist->set(res(i, i + 3));
            i += 4;
            break;
          case 2:
            dist->set(res(i, i + 4));
            i += 5;
            break;
          case 3:
            dist->set(res(i, i + 4));
            i += 5;
            break;
          }
      }

    c_val = false;
  }

  void Camera::set(double fp, double ap, double sp, double u0p, double v0p)
  {
    f = fp;
    a = ap;
    s = sp;

    u0 = u0p;
    v0 = v0p;

    c_val = false;
  }

  void Camera::get(double& fp, double& ap, double& sp,
                   double& u0p, double& v0p) const
  {
    fp = f;
    ap = a;
    sp = s;

    u0p = u0;
    v0p = v0;
  }

  void Camera::set(double fp, double ap, double sp,
                   double u0p, double v0p, const Distortion& d)
  {
    set(fp, ap, sp, u0p, v0p);
    assign(d);
    c_val = false;
  }

  void Camera::setExt(double dxp, double dyp, double dzp,
                      double ap, double bp, double cp)
  {
    dx = dxp;
    dy = dyp;
    dz = dzp;
    alpha = ap;
    beta = bp;
    gamma = cp;
    c_val = false;
  }

  void Camera::getExt(double& dxp, double& dyp, double& dzp,
                      double& ap, double& bp, double& cp) const
  {
    dxp = dx;
    dyp = dy;
    dzp = dz;
    ap = alpha;
    bp = beta;
    cp = gamma;
  }

  void Camera::setDist(const Distortion& d)
  {
    assign(d);
  }

  Trafo& Camera::getTrafo() const
  {
    create_trans();
    return tr;
  }

#define FNAME "Camera::Transform"
  Vector Camera::Transform(const Vector& v) const
  {
    Vector vh(2);
    create_trans();

    if (v.size() != 3)
      throw IceException(FNAME, M_WRONG_DIM);

    vh = tr * v;
    vh = dist->Distort(vh);
    return vh;
  }

  Point Camera::Transform(const Vector3d& p) const
  {
    create_trans();
    Point res;
    ice::transform(tr, p.x, p.y, p.z, res.x, res.y);
    dist->Distort(res.x, res.y);
    return res;
  }

  void Camera::Transform(double x, double y, double z, double& u, double& v) const
  {
    create_trans();
    ice::transform(tr, x, y, z, u, v);
    dist->Distort(u, v);
  }
#undef FNAME
#define FNAME "Camera::Ray"
  Line3d Camera::Ray(const Point& bp) const
  {
    try
      {
        create_trans(); // Transformation erzeugen
        Vector bpu(dist->Rect(bp));// Unverzeichneter Bildpunkt
        bpu.Append(1); // in homogene Koordinaten umwandeln
        Matrix T = tr.Tmatrix(); // Transformationsmatrix
        Matrix A = T(0, 0, 2, 2); // Zerlegen T = (A|a)
        Vector a(3);

        for (int i = 0; i < 3; i++)
          {
            a[i] = T[i][3];
          }

        A = -Inverse(A);
        Vector c1 = A * a;   // Projektionszentrum
        Vector c2 = A * bpu + c1; // Zweiter Punkt im Raum
        return Line3d(Vector3d(c1), Vector3d(c2)); //Strahl zurückgeben
      }
    RETHROW;
  }

  Line3d Camera::Ray(double u, double v) const
  {
    return Ray(Point(u, v));
  }

  Line3d Camera::Ray(const Vector& bp) const
  {
    return Ray(Point(bp));
  }
#undef FNAME
}
