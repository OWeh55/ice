#ifndef MATRIX3D_H
#define MATRIX3D_H

#include "Vector3d.h"
#include "image3d.h"

namespace ice
{
  template<typename T>
  class matrix3d
  {
  public:
    matrix3d(): xsize(0), ysize(0), zsize(0), data(0) {}

    matrix3d(int xs, int ys, int zs):
      xsize(xs), ysize(ys), zsize(zs), data(new T[xs* ys* zs])
    {
    }

    matrix3d(int xs, int ys, int zs, T val):
      xsize(xs), ysize(ys), zsize(zs), data(new T[xs* ys* zs])
    {
      for (int i = 0; i < xs * ys * zs; i++)
        {
          data[i] = val;
        }
    }

    matrix3d(const matrix3d& src):
      xsize(src.xsize), ysize(src.ysize), zsize(src.zsize), data(new T[xsize* ysize* zsize])
    {
      for (int i = 0; i < xsize * ysize * zsize; i++)
        {
          data[i] = src.data[i];
        }
    }

    void swap(matrix3d& sec)
    {
      std::swap(xsize, sec.xsize);
      std::swap(ysize, sec.ysize);
      std::swap(zsize, sec.zsize);
      std::swap(data, sec.data);
    }

    bool inside(int x, int y, int z) const
    {
      return x >= 0 && x < xsize && y >= 0 && y < ysize && z >= 0 && z < zsize;
    }

    bool inside(IVector3d p) const
    {
      return p.x >= 0 && p.x < xsize && p.y >= 0 && p.y < ysize && p.z >= 0 && p.z < zsize;
    }

    T Min() const
    {
      T min = data[0];
      for (int i = 1; i < xsize * ysize * zsize; i++)
        if (data[i] < min)
          {
            min = data[i];
          }
      return min;
    }

    T Min(int& x, int& y, int& z) const
    {
      int minidx = 0;
      T min = data[0];
      for (int i = 1; i < xsize * ysize * zsize; i++)
        if (data[i] < min)
          {
            min = data[i];
            minidx = i;
          }
      xyz(minidx, x, y, z);
      return min;
    }

    T Max() const
    {
      T max = data[0];
      for (int i = 1; i < xsize * ysize * zsize; i++)
        if (data[i] > max)
          {
            max = data[i];
          }
      return max;
    }

    T Max(int& x, int& y, int& z) const
    {
      int maxidx = 0;
      T max = data[0];
      for (int i = 1; i < xsize * ysize * zsize; i++)
        if (data[i] > max)
          {
            max = data[i];
            maxidx = i;
          }
      xyz(maxidx, x, y, z);
      return max;
    }

    void MaxAndMin(T& max, T& min) const
    {
      min = data[0];
      max = min;

      for (int i = 1; i < xsize * ysize * zsize; i++)
        {
          if (data[i] < min)
            {
              min = data[i];
            }
          if (data[i] > max)
            {
              max = data[i];
            }
        }
    }

    const matrix3d& operator=(const matrix3d& src)
    {
      matrix3d<T> res(src);
      swap(res);
      return *this;
    }

    ~matrix3d()
    {
      delete [] data;
    }

    void resize(int sx, int sy, int sz)
    {
      delete [] data;
      xsize = sx;
      ysize = sy;
      zsize = sz;
      data = new T[xsize * ysize * zsize];
    }

    int sizeX() const
    {
      return xsize;
    }
    int sizeY() const
    {
      return ysize;
    }
    int sizeZ() const
    {
      return zsize;
    }

    T& operator()(int ix, int iy, int iz)
    {
      return data[index(ix, iy, iz)];
    }

    const T& operator()(int ix, int iy, int iz) const
    {
      return data[index(ix, iy, iz)];
    }

    T& operator()(IVector3d p)
    {
      return data[index(p)];
    }

    const T& operator()(IVector3d p) const
    {
      return data[index(p)];
    }

    void getXY(int x, int y, T* v) const
    {
      getdata(v, zsize, x, y, 0, 0, 0, 1);
    }
    void getXZ(int x, int z, T* v) const
    {
      getdata(v, ysize, x, 0, z, 0, 1, 0);
    }
    void getYZ(int y, int z, T* v) const
    {
      getdata(v, xsize, 0, y, z, 1, 0, 0);
    }

    void setXY(int x, int y, T* v)
    {
      setdata(v, zsize, x, y, 0, 0, 0, 1);
    }

    void setXZ(int x, int z, T* v)
    {
      setdata(v, ysize, x, 0, z, 0, 1, 0);
    }

    void setYZ(int y, int z, T* v)
    {
      setdata(v, xsize, 0, y, z, 1, 0, 0);
    }

    void set(T value)
    {
      for (int i = 0; i < xsize * ysize * zsize; i++)
        {
          data[i] = value;
        }
    }

  private:
    int index(int ix, int iy, int iz) const
    {
      return ix + xsize * (iy + ysize * iz);
    }

    int index(IVector3d p) const
    {
      return p.x + xsize * (p.y + ysize * p.z);
    }

    void xyz(int idx, int& x, int& y, int& z) const
    {
      x = idx % xsize;
      idx /= xsize;
      y = idx % ysize;
      z = idx / ysize;
    }

    void getdata(T v[], int nr,
                 int ix, int iy, int iz,
                 int dx, int dy, int dz) const
    {
      int idx = index(ix, iy, iz);
      int didx = index(dx, dy, dz);
      for (int n = 0; n < nr; n++)
        {
          v[n] = data[idx];
          idx += didx;
        }
    }

    void setdata(T v[], int nr,
                 int ix, int iy, int iz,
                 int dx, int dy, int dz)
    {
      int idx = index(ix, iy, iz);
      int didx = index(dx, dy, dz);
      for (int n = 0; n < nr; n++)
        {
          data[idx] = v[n];
          idx += didx;
        }
    }

    int xsize;
    int ysize;
    int zsize;

    T* data;
  };

  typedef matrix3d<double> Matrix3d;
  typedef matrix3d<int> IMatrix3d;

  void Image2Matrix(const ice::Image3d<Image>& img, Matrix3d& m);
  void Matrix2Image(const Matrix3d& m, ice::Image3d<Image>& img, int mode = 0);
}
#endif
