// some experiments with lacunarity like features

#define GAUSS 2

#include <image.h>

vector<double> lacu(const Image& img, int maxsize)
{
  int maxnsize = maxsize * 2 + 1;
  int maxboxsize = maxnsize * maxnsize;

#ifdef GAUSS
  Image filtered = NewImg(img->xsize, img->ysize, img->maxval);
#else
  Image filtered = NewImg(img->xsize, img->ysize, img->maxval * maxboxsize);
#endif

  double l0 = 1.0;

  Show(ON, img, "Original");
  Show(ON, filtered, "Gefiltert");
  vector<double> res;

  for (int i = 0; i <= maxsize; i++)
    {
      int boxsize = (i * 2 + 1) * (i * 2 + 1);

      if (i > 0)
        {
#ifdef GAUSS
          double sigma = i / 1.5;
          //  cout << i << " " << sigma << endl;
          GaussImg(img, filtered, 1 + 2 * i, sigma);
#else
          BoxImg(img, filtered, 1 + 2 * i);
#endif
        }
      else
        {
          for (int y = 0; y < img->ysize; y++)
            for (int x = 0; x < img->xsize; x++)
              PutVal(filtered, x, y, GetVal(img, x, y));
        }

      Image validPixels = filtered(Window(i, i, img->xsize - 1 - i, img->ysize - 1 - i));
      Histogram h(validPixels);
      int n;
      double mean, std;
      h.getStatistics(n, mean, std);
#ifndef GAUSS
      mean /= boxsize;
      std /= boxsize;
#endif

      if (i == 0)
        l0 = std;

      cout << i << " " /*<< mean << " " */ << std / l0 << endl;
      res.push_back(std);
    }

  return res;
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  int maxsize = 50;
  int rc;

  if ((rc = getopt(argc, argv, "n:")) >= 0)
    {
      switch (rc)
        {
        case 'n':
          maxsize = atoi(optarg);
          break;
        }
    }

  if (argc > optind)
    fn = argv[optind];

  Image src = ReadImg(fn);
  vector<double> l = lacu(src, maxsize);
  //  for (int i=0;i<l.size();i++)
  //    cout << l[i] << " ";
}
