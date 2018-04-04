#ifndef SDR_CLASS_H
#define SDR_CLASS_H

#include <complex>
#ifdef USE_CUDA
#include <cufftw.h>
#else
#include <fftw3.h>
#endif
#include <vector>

#include <based.h>
#include <base.h>
//#include <image.h>

using namespace ice;
// optimated class for shift detection by restoration
// uses fftw directly, avoids multiple copying of data
class Sdr
{
public:
  Sdr(int xSize, int ySize, double b = 10, int factor = 1, int window = 1);
  virtual ~Sdr();
  virtual void setCutOff(double f)
  {
    fg = f;
    fg2 = fg * fg;
  }
  virtual void setWindowMode(int m);

  virtual void setBeta(double b)
  {
    beta = b;
    beta2 = b * b;
  }

  virtual double getBeta() const
  {
    return beta;
  }

  virtual void setInput(const Image& img1, const Image& img2);
  virtual void getPeak(ImageD& result);
private:
  // basic parameters set by c'tor
  int xSize;
  int ySize;
  double beta;
  double beta2;
  int factor;

  int nPixel;
  int bigXSize;
  int bigYSize;

  int windowing;

  // quasi fixed parameters
  double fg = 2;        // cutoff frequency of highpass
  double fg2 = fg * fg; // square of cutoff frequency of highpass

  fftw_plan spectrumPlan;
  fftw_plan peakPlan;

  std::vector<double> data;
  std::vector<double> peakdata;
  std::vector<double> xWindow;
  std::vector<double> yWindow;

  static void window0(std::vector<double>& window);
  static void window1(std::vector<double>& window);
  static void window2(std::vector<double>& window);
};

#endif
