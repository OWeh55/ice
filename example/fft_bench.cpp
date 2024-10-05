// Tests zur FFT in ICE und FFTW
#include <fftw3.h>
#include <image.h>

#define TIMING 1
#include <visual/timing.h>

#define TIMES 1001
//#define SIZE 4096
#define SIZE 4093
//#define SIZE (1024*128)

double nTime;

void start(const string& msg)
{
  cout << msg << ": " ;
  cout.flush();
  nTime = TimeD();
}

void end(int times)
{
  double time = (TimeD() - nTime) / times;
  if (time < 1)
    cout << time * 1000 << " ms";
  else
    cout << time << " s";
  cout << endl;
}

void main(int argc, char** argv)
{
  int size = SIZE;
  int times = TIMES;
  bool loadWisdom = false;
  bool doNotWriteWisdom = false;
  bool iceFunctions = false;
  bool iceClass = false;
  bool fftw = false;
  int rc;

  while ((rc = getopt(argc, argv, "t:n:wWICF")) >= 0)
    {
      switch (rc)
        {
        case 't':
          times = atoi(optarg);
          break;
        case 'n':
          size = atoi(optarg);
          break;
        case 'w':
          loadWisdom = true;
          break;
        case 'W':
          doNotWriteWisdom = true;
          break;
        case 'I':
          iceFunctions = true;
          break;
        case 'C':
          iceClass = true;
          break;
        case 'F':
          fftw = true;
          break;
        }
    }

  if (!fftw && ! iceClass && ! iceFunctions)
    {
      iceFunctions = iceClass = fftw = true;
    }

  cout << "Zahl der Werte: " << size << endl;
  cout << "Zahl der Durchläufe: " << times << endl;

  if (loadWisdom)
    {
      cout << "loading wisdom.txt" << endl;
      fftw_import_wisdom_from_filename("wisdom.txt");
    }

  Vector vr(size);
  Vector vi(size);

  for (int i = 0; i < size; i++)
    {
      double fi = 2 * M_PI * 10.0 * i / size;
      vr[i] = cos(fi);
      vi[i] = sin(fi);
    }

  if (iceFunctions)
    {
      start("Fourier (Vector)");

      for (int i = 0; i < times; i++)
        Fourier(vr, vi);

      end(times);

      double* inr = new double[size];
      double* ini = new double[size];
      double* outr = new double[size];
      double* outi = new double[size];

      for (int i = 0; i < size; i++)
        {
          inr[i] = vr[i];
          ini[i] = vi[i];
        }

      start("FourierD (double *)");

      for (int i = 0; i < times; i++)
        FourierD(inr, ini,
                 size, NORMAL,
                 outr, outi);

      end(times);
    }

  // tests class FourierTrafo
  if (iceClass)
    {
      vector<double> fr(size);
      vector<double> fi(size);
      for (int i = 0; i < size; i++)
        {
          double phi = 2 * M_PI * 10.0 * i / size;
          fr[i] = cos(phi);
          fi[i] = sin(phi);
        }
      vector<double> gr(size);
      vector<double> gi(size);

      start("c'tor FourierTrafo");
      FourierTrafo ft(size);
      end(1);
      start("setInput/getResult");
      for (int i = 0; i < times; i++)
        {
          ft.setInput(fr, fi);
          ft.getResult(gr, gi);
        }
      end(times);
    }
  // tests with fftw
  // load wisdom if available

  if (fftw)
    {
      fftw_complex* in, * out;
      fftw_plan p;

#if 1
      in  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
      out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
#else
      in  = new fftw_complex[size];
      out = new fftw_complex[size];
#endif

      for (int i = 0; i < size; i++)
        {
          in[i][0] = vr[i];
          in[i][1] = vi[i];
        }

      start("Plan(estimate)");

      p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
      end(1);

      start("fftw");
      for (int i = 0; i < times; i++)
        fftw_execute(p);
      end(times);

      start("Plan(measure)");
      p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_MEASURE);
      end(1);

      start("fftw");

      for (int i = 0; i < times; i++)
        fftw_execute(p);
      end(times);

      fftw_destroy_plan(p);

      start("Plan(patient)");

      p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_PATIENT);
      end(1);

      start("fftw");

      for (int i = 0; i < times; i++)
        fftw_execute(p);
      end(times);

      fftw_destroy_plan(p);

#if 1
      fftw_free(in);
      fftw_free(out);
#else
      delete in;
      delete out;
#endif
    }
  if (loadWisdom && ! doNotWriteWisdom)
    {
      cout << "writing wisdom.txt" << endl;
      fftw_export_wisdom_to_filename("wisdom.txt");
    }
}
