// Tests zur FFT in ICE und FFTW
#include <fftw3.h>
#include <image.h>

#define TIMING 1
#include <visual/timing.h>

#define TIMES 1001
//#define SIZE 4096
#define SIZE 4093
//#define SIZE (1024*128)

void main(int argc, char** argv)
{
  int size = SIZE;
  int times = TIMES;
  int rc;

  while ((rc = getopt(argc, argv, "t:n:")) >= 0)
    {
      switch (rc)
        {
        case 't':
          times = atoi(optarg);
          break;
        case 'n':
          size = atoi(optarg);
          break;
        }
    }

  Alpha(ON);
  Printf("Zahl der Werte: %d\n", size);
  Printf("Zahl der Durchläufe: %d\n", times);

  Vector vr(size);
  Vector vi(size);

  Printf("Vectoren initialisieren\n");

  for (int i = 0; i < size; i++)
    {
      double fi = 2 * M_PI * 10.0 * i / size;
      vr[i] = cos(fi);
      vi[i] = sin(fi);
    }

  vr[0] = 1.0;
#if 0
  BEGIN(0, "Fourier (Vector)");

  for (int i = 0; i < times; i++)
    Fourier(vr, vi);

  END(0, "Fourier");
#endif
  //  for (int i=0;i<20;i++)
  //    cout << i << ": " << vr[i] << " " << vi[i] << endl;
  //  for (int i=1;i<20;i++)
  //    cout << -i << ": " << vr[size-i] << " " << vi[size-i] << endl;

  FILE* wisdom = fopen("wisdom.txt", "r");

  if (wisdom)
    {
      fftw_import_wisdom_from_file(wisdom);
      fclose(wisdom);
    }

  double* inr = new double[size];
  double* ini = new double[size];
  double* outr = new double[size];
  double* outi = new double[size];

  for (int i = 0; i < size; i++)
    {
      inr[i] = vr[i];
      ini[i] = vi[i];
    }

  BEGIN(0, "FourierD (double *)");

  for (int i = 0; i < times; i++)
    FourierD(inr, ini,
             size, NORMAL,
             outr, outi);

  END(0, "FourierD");

  // tests with fftw
  // load wisdom if available

  fftw_complex* in, *out;
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

  BEGIN(1, "Plan(estimate)");
  p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  END(1, "Plan");


  BEGIN(1, "fftw");

  for (int i = 0; i < times; i++)
    fftw_execute(p);

  END(1, "fftw");

  BEGIN(1, "Plan(measure)");
  p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_MEASURE);
  END(1, "Plan");


  BEGIN(1, "fftw");

  for (int i = 0; i < times; i++)
    fftw_execute(p);

  END(1, "fftw");

  fftw_destroy_plan(p);

  BEGIN(1, "Plan(patient)");
  p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_PATIENT);
  END(1, "Plan");


  BEGIN(1, "fftw");

  for (int i = 0; i < times; i++)
    fftw_execute(p);

  END(1, "fftw");

  fftw_destroy_plan(p);

#if 1
  fftw_free(in);
  fftw_free(out);
#else
  delete in;
  delete out;
#endif

  wisdom = fopen("wisdom.txt", "w");
  fftw_export_wisdom_to_file(wisdom);
  fclose(wisdom);

  GetChar();
}
