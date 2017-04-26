/**************************************************************/
/* Test Zeit Fouriertransformation in Bildern                 */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <image.h>

#include <fftw3.h>

class DViewer
{
  Image img;
  ImageD imgd;
  string title;
public:
  DViewer(ImageD id = ImageD(), string tp = ""): imgd(id), title(tp)
  {
    if (imgd.isValid())
      {
        img = NewImg(imgd.xsize, imgd.ysize, 255);
        Update();
        Show(ON, img, title);
      }
  }

  DViewer(string tp): imgd(), title(tp) {}

  ~DViewer()
  {
    if (imgd.isValid())
      Show(OFF, img);
  }

  void Update(ImageD id = ImageD())
  {
    if (id.isValid())
      {
        imgd = id;
        img = NewImg(imgd.xsize, imgd.ysize, 255);
        Show(ON, img, title);
      }

    if (imgd.isValid())
      {
        UpdateLimitImgD(imgd);
        ConvImgDImg(imgd, img);
      }
  }
};

void FourierImgDH(ImageD sr, ImageD si, int mode, ImageD dr, ImageD di)
{
  fftw_iodim dim[2];
  dim[0].n = sr.xsize;
  dim[0].is = 1;
  dim[0].os = 1;
  dim[1].n = sr.ysize;
  dim[1].is = sr.xsize;
  dim[1].os = sr.ysize;

  fftw_plan p;

  if (mode == NORMAL)
    p = fftw_plan_guru_split_dft(2, dim,
                                 0, NULL,
                                 sr.getMatrix().getData(),
                                 si.getMatrix().getData(),
                                 dr.getMatrix().getData(),
                                 di.getMatrix().getData(),
                                 FFTW_ESTIMATE);
  //             FFTW_MEASURE);
  else
    p = fftw_plan_guru_split_dft(2, dim,
                                 0, NULL,
                                 si.getMatrix().getData(),
                                 sr.getMatrix().getData(),
                                 di.getMatrix().getData(),
                                 dr.getMatrix().getData(),
                                 FFTW_ESTIMATE);

  //                           FFTW_MEASURE);
  fftw_execute(p);
  fftw_destroy_plan(p);
}

void HartleyImgDH(ImageD src, ImageD dst)
{
  fftw_iodim dim[2];
  dim[0].n = src.xsize;
  dim[0].is = 1;
  dim[0].os = 1;
  dim[1].n = src.ysize;
  dim[1].is = src.xsize;
  dim[1].os = src.ysize;

  fftw_r2r_kind kind[2] = {FFTW_DHT, FFTW_DHT};

  fftw_plan p;

  p = fftw_plan_guru_r2r(2, dim,
                         0, NULL,
                         src.getMatrix().getData(),
                         dst.getMatrix().getData(),
                         kind,
                         FFTW_ESTIMATE);
  fftw_execute(p);
  fftw_destroy_plan(p);
}

int main(int argc, char* argv[])
{
  int mode = 1;
  int rc;
  bool display = false;
  bool wait = false;

  int times = 1;
  int xsize = 512;
  int ysize = 512;

  while ((rc = getopt(argc, argv, "m:dwx:y:t:")) >= 0)
    {
      switch (rc)
        {
        case 'm':
          mode = atoi(optarg);
          break;
        case 'd':
          display = true;
          break;
        case 'w':
          wait = true;
          break;
        case 'x':
          xsize = atoi(optarg);
          break;
        case 'y':
          ysize = atoi(optarg);
          break;
        case 't':
          times = atoi(optarg);
          break;
        }
    }

  if (wait)
    display = true;

  Image tests = NewImg(xsize, ysize, 255);
  clearImg(tests);

  if (display) Show(ON, tests, "Source");

  Image testd = NewImg(xsize, ysize, 255);
  clearImg(testd);

  if (display) Show(ON, testd, "Destination");

  // double complex images
  // src
  ImageD srcr = NewImgD(xsize, ysize);
  clearImgD(srcr);
  //  DViewer srcrd("source real");
  //  if (display) srcrd.Update(srcr);

  ImageD srci = NewImgD(xsize, ysize);
  clearImgD(srci);
  //  DViewer srcid("source imag");
  //  if (display) srcid.Update(srci);

  // dst
  ImageD dstr = NewImgD(xsize, ysize);
  clearImgD(dstr);
  DViewer dstrd("dest real");

  if (display) dstrd.Update(dstr);

  ImageD dsti = NewImgD(xsize, ysize);
  clearImgD(dsti);
  DViewer dstid("dest imag");

  if (display) dstid.Update(dsti);

  for (int y = 0; y < tests.ysize; y++)
    for (int x = 0; x < tests.xsize; x++)
      {
        PutVal(tests, x, y, Random(255));
      }

  smearImg(tests);

  switch (mode)
    {

    case 1:
      ConvImgImgD(tests, srcr);

      for (int i = 0; i < times; i++)
        {
          FourierImgD(srcr, srci, NORMAL, dstr, dsti);
          PutValD(dstr, xsize / 2, ysize / 2, 0);
          PutValD(dsti, xsize / 2, ysize / 2, 0);

          if (display)
            {
              dstrd.Update();
              dstid.Update();
            }

          if (wait) GetChar();

          FourierImgD(dstr, dsti, INVERS, dstr, dsti);
        }

      ConvImgDImg(dstr, testd);
      break;

    case 2:
      ConvImgImgD(tests, srcr);

      for (int i = 0; i < times; i++)
        {
          FourierImgDH(srcr, srci, NORMAL, dstr, dsti);
          PutValD(dstr, 0, 0, 0);
          PutValD(dsti, 0, 0, 0);

          if (display)
            {
              dstrd.Update();
              dstid.Update();
            }

          if (wait) GetChar();

          FourierImgDH(dstr, dsti, INVERS, dstr, dsti);
        }

      ConvImgDImg(dstr, testd);
      break;

    case 3:
      ConvImgImgD(tests, srcr);

      for (int i = 0; i < times; i++)
        {
          HartleyImgD(srcr, dstr);
          PutValD(dstr, xsize / 2, ysize / 2, 0);

          if (display)
            dstrd.Update();

          if (wait)
            GetChar();

          HartleyImgD(dstr, dstr);
        }

      ConvImgDImg(dstr, testd);
      break;

    case 4:
      ConvImgImgD(tests, srcr);

      for (int i = 0; i < times; i++)
        {
          HartleyImgDH(srcr, dstr);
          PutValD(dstr, 0, 0, 0);

          if (display)
            dstrd.Update();

          if (wait)
            GetChar();

          HartleyImgDH(dstr, dstr);
        }

      ConvImgDImg(dstr, testd);
      break;


    }

  if (wait) GetChar();

  return 0;
}
