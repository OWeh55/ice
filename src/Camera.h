#ifndef CAMERA_H_
#define CAMERA_H_

#include "Trafo.h"
#include "Distortion.h"
#include "Vector3d.h"
#include "line3d.h"

namespace ice
{
  // intrinsic parameters
#define CAM_FOCAL_LENGTH 0
#define CAM_ASPECT_RATIO 1
#define CAM_SKEW 2
#define CAM_PRINCIPAL_POINT_U 3
#define CAM_PRINCIPAL_POINT_V 4

  // extrinsic parameters
  // shift
#define CAM_DX 5
#define CAM_DY 6
#define CAM_DZ 7
  // rotation
#define CAM_BETA 9
#define CAM_ALPHA 10
#define CAM_GAMMA 8

  // distortion models
  // center of distortion
#define CAM_DIST_X 11
#define CAM_DIST_Y 12
  // nonlinearity parameters
#define CAM_DIST_D2 13
#define CAM_DIST_D4 14
  // models Distortion2 / Distortion3
#define CAM_DIST_D3 15
#define CAM_DIST_D6 15

  class Camera
  {
  public:
    // constant to select subsets of parameters
    enum { intrinsic = 1, extrinsic = 2, all = 3 };

  private:
    // intrinsic parameters
    double f; // focal length
    double a; // aspect ration
    double s; // skew
    double u0, v0; // principal point
    // extrinsic parameters
    double dx, dy, dz; // translation
    double alpha, beta, gamma; // rotation around z-, y- and x-axis

    mutable Trafo tr; // transformation matrix without distortion
    mutable bool trValid; // matrix valid ?

    int disttyp;
    Distortion* dist;

    virtual void create_trans() const; // parameters -> Trafo
    virtual void newdist(int dtyp);
    virtual void newdist(int dtyp, Distortion* d);
    virtual void assign(const Distortion& d);

  public:

    Camera(int dtyp = 1);
    Camera(const Camera&);

    virtual ~Camera()
    {
      delete dist;
    }

    void swap(Camera& c);

    Camera& operator=(Camera c);

    // serialize variables
    virtual Vector makeVector(int what = all) const;
    virtual std::vector<double> makeVectorDouble(int what = all) const;
    // deserialize variables
    virtual void set(const Vector& res, int what = all); // Parameter nach Vektor setzen
    virtual void set(const std::vector<double>& res, int what = all); // Parameter nach vector<double> setzen

    virtual void set(double fp, double ap, double sp, double u0p, double v0p,
                     const Distortion& d);
    virtual void set(double fp, double ap, double sp, double u0p, double v0p);

    virtual void setExt(double dxp, double dyp, double dzp,
                        double ap, double bp, double cp);

    virtual void get(double& fp, double& ap, double& sp,
                     double& u0p, double& v0p) const;

    virtual void getExt(double& dxp, double& dyp, double& dzp,
                        double& ap, double& bp, double& cp) const;

    virtual Trafo& getTrafo() const;

    virtual void setDist(const Distortion& d);
    virtual int DistType() const
    {
      return disttyp;
    }

    virtual const Distortion& Dist() const
    {
      return *dist;
    }
    virtual Distortion& Dist()
    {
      return *dist;
    }

    // Textdarstellung der Parameter
    virtual std::string toString(const std::string& del = "\n") const;

    // Abbildung des 3D Vektors v -> 2D
    virtual Vector transform(const Vector& v) const;
    virtual Point transform(const Vector3d& v) const;

    // Abbildung des 3D Vektors (x,y,z) -> 2D (u,v)
    virtual void transform(double x, double y, double z, double& u, double& v) const;

    // Sehstrahl berechnen
    virtual Line3d Ray(const Point& bp) const;
    virtual Line3d Ray(double u, double v) const;
    virtual Line3d Ray(const Vector& bp) const;
  };
}
#endif
