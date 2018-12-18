#include <fftw3.h>

#include <image3d.h>

#include <fourier.h>

#include "fft3d.h"

using namespace ice;
using namespace std;

class Transformer
{
public:
  Transformer(int anz, bool forward = true): size(anz), forward(forward)
  {
    // cerr << size << endl;
    alloc();

    // planning is not thread save, therefore we need to declare it critical
    #pragma omp critical
    {
      fftw_p = fftw_plan_dft_1d(size, in, out,
                                forward ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_MEASURE);
    }
  }

  // copy c'tor
  Transformer(const Transformer& tr): size(tr.size), forward(tr.forward)
  {
    alloc();
    // unknown: can plan be copied ?
#if 0
    fftw_p = tr.fftw_p;
#else
    // or needs replaning
    #pragma omp critical
    {
      fftw_p = fftw_plan_dft_1d(size, in, out,
                                forward ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_MEASURE);
    }
#endif
  }


  ~Transformer()
  {
    fftw_free(in);
    fftw_free(out);

    delete [] areal;
    delete [] aimag;

    // free plan
    fftw_destroy_plan(fftw_p);
  }

  void Transform()
  {
    // import data 2*double -> complex
    int sourceIndex = size / 2;
    int destinationIndex = 0;
    while (destinationIndex < size)
      {
        in[destinationIndex][0] = areal[sourceIndex];
        in[destinationIndex][1] = aimag[sourceIndex];
        destinationIndex++;
        sourceIndex = (sourceIndex + 1) % size;
      }

    fftw_execute_dft(fftw_p, in, out);

    // normalize and export data complex -> 2*double
    double fftnorm = 1.0 / sqrt((double)size);
    sourceIndex = (size + 1) / 2;
    destinationIndex = 0;
    while (destinationIndex < size)
      {
        areal[destinationIndex] = out[sourceIndex][0] * fftnorm;
        aimag[destinationIndex] = out[sourceIndex][1] * fftnorm;
        destinationIndex++;
        sourceIndex = (sourceIndex + 1) % size;
      }
  }
private:
  void alloc()
  {
    in = fftw_alloc_complex(size);
    out = fftw_alloc_complex(size);
    areal = new double[size];
    aimag = new double[size];
  }

  fftw_complex* in;
  fftw_complex* out;

  int size;
  bool forward;
  fftw_plan fftw_p;

public:
  double* areal;
  double* aimag;
};

void fft(const Matrix3d& src, Matrix3d& real,
         Matrix3d& imag, bool forward) // real to complex fft
{
  int sx = src.sizeX();
  int sy = src.sizeY();
  int sz = src.sizeZ();
  real = src;
  imag.resize(sx, sy, sz);
  imag.set(0);
  fft(real, imag, forward);
}

void fft(Matrix3d& real, Matrix3d& imag, bool forward)
{
  int sx = real.sizeX();
  int sy = real.sizeY();
  int sz = real.sizeZ();

  if (!(sx == imag.sizeX() && sy == imag.sizeY() && sz == imag.sizeZ()))
    throw "fft - matrix sizes differ";

  // fft in drei Richtungen..
  // ..Z

  if (sz > 1)
    {
      // cout << "z"; cout.flush();
      Transformer tr(sz, forward);
      #pragma omp parallel for schedule(dynamic,40) firstprivate(tr)
      for (int x = 0; x < sx; x++)
        for (int y = 0; y < sy; y++)
          {
            real.getXY(x, y, tr.areal);
            imag.getXY(x, y, tr.aimag);
            tr.Transform();
            real.setXY(x, y, tr.areal);
            imag.setXY(x, y, tr.aimag);
          }
      //      cout << endl;
    }

  // ..Y
  if (sy > 1)
    {
      //      cout << "y";cout.flush();
      Transformer tr(sy, forward);
      #pragma omp parallel for schedule(dynamic,40) firstprivate(tr)
      for (int x = 0; x < sx; x++)
        for (int z = 0; z < sz; z++)
          {
            real.getXZ(x, z, tr.areal);
            imag.getXZ(x, z, tr.aimag);
            tr.Transform();
            real.setXZ(x, z, tr.areal);
            imag.setXZ(x, z, tr.aimag);
          }
      //      cout << endl;
    }

  // ..X
  if (sx > 1)
    {
      //      cout << "x";cout.flush();
      Transformer tr(sx, forward);
      #pragma omp parallel for schedule(dynamic,40) firstprivate(tr)
      for (int y = 0; y < sy; y++)
        for (int z = 0; z < sz; z++)
          {
            real.getYZ(y, z, tr.areal);
            imag.getYZ(y, z, tr.aimag);
            tr.Transform();
            real.setYZ(y, z, tr.areal);
            imag.setYZ(y, z, tr.aimag);
          }
      //      cout << endl;
    }
}
