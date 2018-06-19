#include <algorithm>
#include "Sdr.h"

using namespace std;
Sdr::Sdr(int xSize, int ySize, double b, int factor, int windowMode):
  xSize(xSize), ySize(ySize), beta(b), beta2(b * b), factor(factor)
{
  nPixel = xSize * ySize;
  data.resize(nPixel * 2); // nPixel complex numbers
  // create plan for (combined) fft of input images
  spectrumPlan = fftw_plan_dft_2d(ySize, xSize,
                                  reinterpret_cast<fftw_complex*>(data.data()),
                                  reinterpret_cast<fftw_complex*>(data.data()),
                                  FFTW_FORWARD, FFTW_MEASURE);
  // create plan for inverse fft of peak image
  bigXSize = factor * xSize; // extended size for sub-pixel accuracy
  bigYSize = factor * ySize;
  peakdata.resize(bigXSize * bigYSize * 2);
  peakPlan = fftw_plan_dft_2d(bigYSize, bigXSize,
                              reinterpret_cast<fftw_complex*>(peakdata.data()),
                              reinterpret_cast<fftw_complex*>(peakdata.data()),
                              FFTW_BACKWARD, FFTW_MEASURE);

  // windowing
  xWindow.resize(xSize);
  yWindow.resize(ySize);

  setWindowMode(windowMode);
}

Sdr::~Sdr()
{
  fftw_destroy_plan(spectrumPlan);
  fftw_destroy_plan(peakPlan);
}

// prepare windowing function
// 0 - "no windowing"
void Sdr::window0(std::vector<double>& window)
{
  for (unsigned int i = 0; i < window.size(); i++)
    {
      window[i] = 1.0;
    }
}

// 1 - 0.5*(1-cos(phi))
void Sdr::window1(std::vector<double>& window)
{
  for (unsigned int i = 0; i < window.size(); i++)
    {
      double phi = i * 2 * M_PI / window.size();
      window[i] = (1.0 - cos(phi)) / 2 ;
    }
}

// 2 - outer quarters: 0.5*(1-cos(2*phi))
//   - inner quarters: 1.0
void Sdr::window2(std::vector<double>& window)
{
  for (unsigned int i = 0; i < window.size(); i++)
    {
      double phi = i * 2 * 2 * M_PI / window.size();
      if (phi > M_PI && phi < M_PI * 3)
        window[i] = 1.0;
      else
        window[i] = (1.0 - cos(phi)) / 2;
    }
}

void Sdr::setWindowMode(int m)
{
  windowing = m;
  switch (m)
    {
    case 0:
      window0(xWindow);
      window0(yWindow);
      break;
    case 1:
      window1(xWindow);
      window1(yWindow);
      break;
    case 2:
      window2(xWindow);
      window2(yWindow);
      break;
    }
}

void Sdr::setInput(const Image& img1, const Image& img2)
{
  int idx = 0;
  for (int y = 0; y < ySize; y++)
    for (int x = 0; x < xSize; x++)
      {
        if (windowing == 0) // avoid unnecessary multiplication
          {
            data[idx] = img1.getPixelUnchecked(x, y);
            idx++;
            data[idx] = img2.getPixelUnchecked(x, y);
            idx++;
          }
        else
          {
            data[idx] = img1.getPixelUnchecked(x, y) * xWindow[x] * yWindow[y];
            idx++;
            data[idx] = img2.getPixelUnchecked(x, y) * xWindow[x] * yWindow[y];
            idx++;
          }
      }
}

void sdrFormulaPacked(
  double rr, double ir, // complex positiv coefficient
  double rq, double iq, // complex negativ coefficient
  double beta,
  double& rp, double& ip // complex coefficient of peak
)
{
  // complex coefficient for f1 and f2 from coefficient of f1 + i * f2
  double r1 = rr + rq;
  double i1 = ir - iq;
  double r2 = ir + iq;
  double i2 = rq - rr;

  // sdr formula
  double zr = r1 * r2 + i1 * i2;
  double zi = -i1 * r2 + r1 * i2;
  // the symmetrised variant is more expensive to calculate
  // double n = sqrt((r1 * r1 + i1 * i1) * (r2 * r2 + i2 * i2)) + beta;
  double n = (r1 * r1 + i1 * i1) + beta;
  rp = zr / n;
  ip = zi / n;
}

void Sdr::getPeak(ImageD& result)
{
  // create spectrum
  fftw_execute(spectrumPlan);

  for (int i = 0; i < bigXSize * bigYSize * 2; i++)
    peakdata[i] = 0;

  int halfXSize = xSize / 2;
  int halfYSize = ySize / 2;

  // extend by factor
  // and apply sdr formula

  for (int y = 0; y < halfYSize; y++)
    {
      int yr = (ySize + y) % ySize;
      int yrn = (ySize - y) % ySize;
      int yrbig = (bigYSize + y) % bigYSize;
      int yrnbig = (bigYSize - y) % bigYSize;
      for (int x = -halfXSize; x < halfXSize; x++)
        {
          int xr = (xSize + x) % xSize;
          int xrn = (xSize - x) % xSize;
          int xrbig = (bigXSize + x) % bigXSize;
          int xrnbig = (bigXSize - x) % bigXSize;

          int idx = (yr * xSize + xr) * 2;
          int nIdx = (yrn * xSize + xrn) * 2;

          int bigIdx = (yrbig * bigXSize + xrbig) * 2;
          int nbigIdx = (yrnbig * bigXSize + xrnbig) * 2;

          int xa = abs(x);
          int ya = abs(y);
          double f2 = xa * xa + ya * ya;
          if (f2 < fg2)
            {
              peakdata[bigIdx] = 0.0;
              peakdata[bigIdx + 1] = 0.0;
              peakdata[nbigIdx] = 0.0;
              peakdata[nbigIdx + 1] = 0.0;
            }
          else
            {
              double rp, ip;
              sdrFormulaPacked(data[idx], data[idx + 1],
                               data[nIdx], data[nIdx + 1],
                               beta2, rp, ip);

              peakdata[bigIdx] = rp;
              peakdata[bigIdx + 1] = ip;
              peakdata[nbigIdx] = rp;
              peakdata[nbigIdx + 1] = -ip;
            }
        }
    }

  fftw_execute(peakPlan);

  if ((result.xsize != bigXSize) || (result.ysize != bigYSize))
    result.create(bigXSize, bigYSize);
  int idx = 0;
  for (int y = 0; y < bigYSize; y++)
    for (int x = 0; x < bigXSize; x++)
      {
        result.setPixelUnchecked((x + bigXSize / 2) % bigXSize,
                                 (y + bigYSize / 2) % bigYSize,
                                 peakdata[idx]);
        idx += 2;
      }
}
