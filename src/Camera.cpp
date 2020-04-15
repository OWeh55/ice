#include <iostream>
#include <sstream>

#include "macro.h"
#include "MatrixAlgebra.h"
#include "matrixtools.h"

#include "Distortion1.h"
#include "Distortion2.h"
#include "Distortion3.h"

#include "Camera.h"

using namespace std;

// Konstruktoren
namespace ice
{
  void Camera::newDistortion(int dtyp)
  {
    distortionType = dtyp;

    switch (dtyp)
      {
      case 0:
        dist = nullptr;
        break;
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
        throw IceException("Camera::newDistortion", "Wrong distortion type");
        break;
      }
  }

  void Camera::newDistortion(int dtyp, Distortion* d)
  {
    distortionType = dtyp;

    switch (dtyp)
      {
      case 0:
        dist = nullptr;
        break;
      case 1:
        dist = new Distortion1(*(Distortion1*)(d));
        break;
      case 2:
        dist = new Distortion2(*(Distortion2*)(d));
        break;
      case 3:
        dist = new Distortion3(*(Distortion3*)(d));
        break;
      default:
        throw IceException("Camera::newDistortion", "Wrong distortion type");
      }
  }

  void Camera::assign(const Distortion& d)
  {
    switch (distortionType)
      {
      case 0:
        throw IceException("Camera::assign", "Camera has no distortion");
        break;
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
    trValid(false)
  {
    newDistortion(dtyp);
  }

  Camera::Camera(const Camera& c): f(c.f), a(c.a), s(c.s), u0(c.u0), v0(c.v0),
    dx(c.dx), dy(c.dy), dz(c.dz),
    alpha(c.alpha), beta(c.beta), gamma(c.gamma),
    trValid(false)
  {
    newDistortion(c.distortionType, c.dist);
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
    trValid = false;
    c.trValid = false;
    std::swap(distortionType, c.distortionType);
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

    if (dist != nullptr)
      os << "dist: " << del << dist->toString() << del;
    else
      os << "no distortion" << del;
    return os.str();
  }

// create transformation from camera parameters
  void Camera::createTrans() const
  {
    // must be called, before trafo tr is used
    // all methods, that change parameters, must
    // set trValid to false!
    if (trValid)
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
    // std::cout << tr.getMatrix() << std::endl;
    tr.projective(); // 3d -> 2d
    // std::cout << tr.getMatrix() << std::endl;
    tr.shearX(s);
    tr.scale(0, 0, f, -a * f);
    // std::cout << tr.getMatrix() << std::endl;
    tr.shift(u0, v0);
    // std::cout << tr.getMatrix() << std::endl;
    trValid = true;
  }

// put camera parameters into Vector (for lmdiff,...)
  Vector Camera::makeVector(int what) const
  {
    Vector res(0);

    if ((what == intrinsic) || (what == all))
      {
        res.Append(f);
        res.Append(a);
        res.Append(s);

        res.Append(u0);
        res.Append(v0);
      }

    if ((what == extrinsic) || (what == all))
      {
        res.Append(dx);
        res.Append(dy);
        res.Append(dz);

        res.Append(alpha);
        res.Append(beta);
        res.Append(gamma);
      }

    if (((what == all) || (what == intrinsic)) && distortionType != 0)
      {
        res.Append(dist->makeVector());
      }

    return res;
  }

  vector<double> Camera::makeVectorDouble(int what) const
  {
    vector<double> res;

    if ((what == intrinsic) || (what == all))
      {
        res.push_back(f);
        res.push_back(a);
        res.push_back(s);

        res.push_back(u0);
        res.push_back(v0);
      }

    if ((what == extrinsic) || (what == all))
      {
        res.push_back(dx);
        res.push_back(dy);
        res.push_back(dz);

        res.push_back(alpha);
        res.push_back(beta);
        res.push_back(gamma);
      }

    if (((what == all) || (what == intrinsic)) && distortionType != 0)
      {
        vector<double> distortionParameter = dist->makeVectorDouble();
        for (int i = 0; i < distortionParameter.size(); i++)
          res.push_back(distortionParameter[i]);
      }

    return res;
  }

// Parameter nach Vektor setzen
  void Camera::set(const Vector& res, int what)
  {
    int i = 0;

    if ((what == intrinsic) || (what == all))
      {
        f = res[i++];
        a = res[i++];
        s = res[i++];
        u0 = res[i++];
        v0 = res[i++];
      }

    if ((what == extrinsic) || (what == all))
      {
        dx = res[i++];
        dy = res[i++];
        dz = res[i++];
        alpha = fmod(res[i++], 2 * M_PI);
        beta = fmod(res[i++], 2 * M_PI);
        gamma = fmod(res[i++], 2 * M_PI);
      }

    if ((what == all) || (what == intrinsic))
      {
        switch (distortionType)
          {
          case 0:
            /* nothing to do */
            break;
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

    trValid = false;
  }

// Parameter nach vector<double> setzen
  void Camera::set(const vector<double>& res, int what)
  {
    int i = 0;

    if ((what == intrinsic) || (what == all))
      {
        f = res[i++];
        a = res[i++];
        s = res[i++];
        u0 = res[i++];
        v0 = res[i++];
      }

    if ((what == extrinsic) || (what == all))
      {
        dx = res[i++];
        dy = res[i++];
        dz = res[i++];
        alpha =  fmod(res[i++], 2 * M_PI);
        beta =  fmod(res[i++], 2 * M_PI);
        gamma =  fmod(res[i++], 2 * M_PI);
      }

    if (((what == all) || (what == intrinsic)) && distortionType != 0)
      {
        vector<double> distortionParameter(5);
        for (int k = i; k < res.size() - i; k++)
          distortionParameter[k] = res[i + k];
        dist->set(distortionParameter);
      }

    trValid = false;
  }

  void Camera::set(double fp, double ap, double sp, double u0p, double v0p)
  {
    f = fp;
    a = ap;
    s = sp;

    u0 = u0p;
    v0 = v0p;

    trValid = false;
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
    trValid = false;
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
    trValid = false;
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
    createTrans();
    return tr;
  }

#define FNAME "Camera::transform"
  Vector Camera::transform(const Vector& v) const
  {
    Vector vh(2);
    createTrans();

    if (v.size() != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    vh = tr * v;
    if (dist != nullptr)
      vh = dist->distort(vh);
    return vh;
  }

  Point Camera::transform(const Vector3d& p) const
  {
    createTrans();
    Point res;
    ice::transform(tr, p.x, p.y, p.z, res.x, res.y);
    if (dist != nullptr)
      dist->distort(res.x, res.y);
    return res;
  }

  void Camera::transform(double x, double y, double z, double& u, double& v) const
  {
    createTrans();
    ice::transform(tr, x, y, z, u, v);
    dist->distort(u, v);
  }
#undef FNAME
#define FNAME "Camera::Ray"
  Line3d Camera::Ray(const Point& bp) const
  {
    try
      {
        createTrans(); // Transformation erzeugen
        Vector bpu(bp);
        if (dist != nullptr)
          dist->rectify(bpu[0], bpu[1]); // Unverzeichneter Bildpunkt
        bpu.Append(1); // in homogene Koordinaten umwandeln
        Matrix T(tr.getMatrix()); // Transformationsmatrix
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
