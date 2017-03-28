#ifndef CAMERA_H_
#define CAMERA_H_

#include "Trafo.h"
#include "Distortion.h"
#include "Vector3d.h"
#include "line3d.h"

namespace ice
{
// sets of parameters
#define CAM_BRENNWEITE 0
#define CAM_ANISOTROPIE 1
#define CAM_SCHERUNG 2
#define CAM_HP_U 3
#define CAM_HP_V 4

#define CAM_DX 5
#define CAM_DY 6
#define CAM_DZ 7

#define CAM_GAMMA 8
#define CAM_BETA 9
#define CAM_ALPHA 10

// alle Verzeichnungsmodelle
#define CAM_DIST_X 11
#define CAM_DIST_Y 12
#define CAM_DIST_D2 13
#define CAM_DIST_D4 14
// Verzeichnungsmodelle Distortion2/Distortion3
#define CAM_DIST_D3 15
#define CAM_DIST_D6 15

  class Camera
  {
  public:
    // Konstanten zur Auswahl von Teilparameter-Sätzen
    enum { internal = 1, external = 2, all = 3 };

  private:
    double f; // "Kammerkonstante"
    double a; // Anisotropie
    double s; // Scherung
    double u0, v0; // Hauptpunkt
    // externe Parameter
    double dx, dy, dz; // Translation
    double alpha, beta, gamma; // Rotation um Z-, Y- und X-Achse

    mutable Trafo tr; // Transformation (ohne Verzeichnung)

    mutable bool c_val; // ist die Transformation gültig?

    int disttyp;
    Distortion* dist; // Verzeichnung

    virtual void create_trans() const; // Transformation aus Parametern berechnen
    virtual void newdist(int dtyp);
    virtual void newdist(int dtyp, Distortion* d);
    virtual void assign(const Distortion& d);

  public:

    Camera(int dtyp = 1);

    Camera(const Vector& v, int dtyp = 1): disttyp(dtyp)
    {
      newdist(dtyp);
      Set(v);
    };

    Camera(const Camera&);

    virtual ~Camera()
    {
      delete dist;
    }

    void swap(Camera& c);

    Camera& operator=(Camera c);

    virtual Vector MakeVector(int what = all) const; // Parameter in Vektor speichern
    virtual void Set(const Vector& res, int what = all); // Parameter nach Vektor setzen

    virtual void Set(double fp, double ap, double sp,
                     double u0p, double v0p, const Distortion& d);
    virtual void Set(double fp, double ap, double sp, double u0p, double v0p);

    virtual void SetExt(double dxp, double dyp, double dzp,
                        double ap, double bp, double cp);

    virtual void Get(double& fp, double& ap, double& sp,
                     double& u0p, double& v0p) const;

    virtual void GetExt(double& dxp, double& dyp, double& dzp,
                        double& ap, double& bp, double& cp) const;

    virtual Trafo& GetTrafo() const;

    virtual void SetDist(const Distortion& d);
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
    virtual Vector Transform(const Vector& v) const;
    virtual Point Transform(const Vector3d& v) const;

    // Abbildung des 3D Vektors (x,y,z) -> 2D (u,v)
    virtual void Transform(double x, double y, double z, double& u, double& v) const;

    // Sehstrahl berechnen
    virtual Line3d Ray(const Point& bp) const;
    virtual Line3d Ray(double u, double v) const;
    virtual Line3d Ray(const Vector& bp) const;
  };
}
#endif
