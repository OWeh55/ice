/**************************************************************/
/* Test lokale Filter 2                                       */
/**************************************************************/

#include <stdio.h>
#include <image.h>
#include <string.h>

#define path "./"

#define TM_MODE TM_PROCESS

#define TIMES 30
#define KSIZE 11
#define MAXVALUE (256-1)
//#define KSIZE 3

int main(int argc, char* argv[])
{
  int rc;
  int maxval = MAXVALUE;
  int filter = -1;
  int ntimes = TIMES;

  while ((rc = getopt(argc, argv, "m:t:f:")) >= 0)
    {
      switch (rc)
        {
        case 'm':
          maxval = atol(optarg);
          break;
        case 'f':
          filter = atol(optarg);
          break;
        case 't':
          ntimes = atol(optarg);
          break;
        }
    }

  //  double fkernel[KSIZE*KSIZE];
  //  double *fkernel;
  Matrix matrix(KSIZE, KSIZE);
  IMatrix imatrix(KSIZE, KSIZE);
  string filename = "test_gray.jpg";
  double atime;
  OpenAlpha("Filtertest");

  for (int i = 0; i < KSIZE; ++i)
    for (int j = 0; j < KSIZE; j++)
      {
        int k2 = KSIZE / 2;
        double di = i - k2;
        double dj = j - k2;
        double r2 = di * di + dj * dj;
        int vali = 12;

        if (r2 > (k2 / 2) * (k2 / 2))
          {
            vali = -3;
          }

        if (r2 > k2 * k2)
          {
            vali = 0;
          }

        //  kernel[i*KSIZE+j]=vali;
        //  fkernel[i*KSIZE+j]=((double)vali)/((KSIZE)*(KSIZE));
        matrix[i][j] = vali;
        imatrix[i][j] = vali;
      }

  int mask[KSIZE * KSIZE]; // Maske für erode/dilate
  IMatrix maskm(KSIZE, KSIZE);

  for (int i = 0; i < KSIZE; ++i)
    for (int j = 0; j < KSIZE; j++)
      {
        mask[i * KSIZE + j] = 0;
        maskm[j][i] = 0;
      }

  mask[0] = 1;
  mask[KSIZE - 1] = 1;
  mask[(KSIZE - 1)*KSIZE] = 1;
  mask[KSIZE * KSIZE - 1] = 1;

  maskm[0][0] = 1;
  maskm[KSIZE - 1][0] = 1;
  maskm[0][KSIZE - 1] = 1;
  maskm[KSIZE - 1][KSIZE - 1] = 1;

  IMatrix m(3, 3);
  m[0][0] = -1;
  m[0][1] = 0;
  m[0][2] = 1;
  m[1][0] = -1;
  m[1][1] = 0;
  m[1][2] = 1;
  m[2][0] = -1;
  m[2][1] = 0;
  m[2][2] = 1;

  LSIFilter gradx(m, 6);
  LSIFilter grady(!m, 6);

  if (argc > optind)
    filename = argv[optind];

  Print(filename + "\n");

  // Originalbild einlesen
  Image p1a = ReadImg(filename);
  // Bild mit gewünschtem Maximalwert erzeugen
  Image p1 = NewImg(p1a->xsize, p1a->ysize, maxval);

  IPoint p;
  for (p.y = 0; p.y < p1->ysize; p.y++)
    for (p.x = 0; p.x < p1->xsize; p.x++)
      PutVal(p1, p, GetVal(p1a, p)*maxval / 255);

  // Zielbild anlegen
  Image p2 = NewImg(p1);
  // Hilfsbild anlegen
  Image ph = NewImg(p1);

  Show(ON, p1, "original");
  Show(ON, p2, "gefiltert");

  ClearAlpha();

  bool abort = false;

  while (!abort)
    {
      vector<string> men;
      IVector mid;

      men.push_back("~1 GradXImg");
      mid.Append(1);
      men.push_back("~1 GradXImg(LSI)");
      mid.Append(101);
      men.push_back("~2 GradYImg");
      mid.Append(2);
      men.push_back("~2 GradYImg(LSI)");
      mid.Append(102);
      men.push_back("~3 GradImg");
      mid.Append(3);
      men.push_back("~4 GradDirImg");
      mid.Append(4);
      men.push_back("~5 SmearImg(3*3)");
      mid.Append(5);
      men.push_back("~6 SmearImg(11*5)");
      mid.Append(6);
      men.push_back("~7 LSIImg(Image,IMatrix,int,Image)");
      mid.Append(7);
      men.push_back("~8 LSIImg(Image,Matrix,int,Image)");
      mid.Append(8);
      men.push_back("~a LaplaceXImg");
      mid.Append(9);
      men.push_back("~b LaplaceYImg");
      mid.Append(10);
      men.push_back("~c LaplaceImg");
      mid.Append(11);
      men.push_back("~d DilateImg mit Maske");
      mid.Append(12);
      men.push_back("~e ErodeImg mit Maske");
      mid.Append(13);
      men.push_back("~i DilateImg 5*5");
      mid.Append(14);
      men.push_back("~o ErodeImg 5*5");
      mid.Append(15);
      men.push_back("~X EXIT");
      mid.Append(17);

      int sel = Menu(men, mid, 33, 2, 77, 22);

      vector<double> times(ntimes);

      atime = TimeD(TM_MODE);
      double ctime = atime;

      for (int iii = 0; iii < ntimes; iii++)
        {
          switch (sel)
            {
            case 1:

              if (GradXImg(p1, p2) != OK) exit(255);

              break;
            case 101:
            {
              LSIImg(p1, p2, gradx, p2->maxval / 2);
              break;
            }
            case 2:

              if (GradYImg(p1, p2) != OK) exit(255);

              break;
            case 102:
            {
              LSIImg(p1, p2, grady, p2->maxval / 2);
              break;
            }
            case 3:

              if (GradImg(p1, 2, p2) != OK) exit(255);

              break;

            case 4:
            {
              if (GradDirImg(p1, p2) != OK) exit(255);

              WindowWalker p(p2);
              for (p.init(); !p.ready(); p.next())
                {
                  p2.setPixel(p, p2.getPixel(p)*p2->maxval / 8);
                }
            }
            break;

            case 5:

              if (SmearImg(p1, p2, 3) != OK)
                exit(255);

              break;
            case 6:

              if (SmearImg(p1, p2, 11, 5) != OK)
                exit(255);

              break;
            case 7:

              if (LSIImg(p1, imatrix, KSIZE * KSIZE,
                         p1->maxval / 2, p2) != OK)
                exit(255);

              break;
            case 8:

              if (LSIImg(p1, matrix * (1.0 / (KSIZE * KSIZE)),
                         p1->maxval / 2, p2) != OK)
                exit(255);

              break;

            case 9:

              if (LaplaceXImg(p1, 4, p2) != OK) exit(255);

              break;
            case 10:

              if (LaplaceYImg(p1, 4, p2) != OK) exit(255);

              break;
            case 11:

              if (LaplaceImg(p1, 4, p2) != OK) exit(255);

              break;

            case 12:

              //if (DilateImg(p1, KSIZE, mask, p2)!=OK) exit(255);
              if (DilateImg(p1, maskm, p2) != OK) exit(255);

              break;
            case 13:

              //if (ErodeImg(p1, KSIZE, mask, p2)!=OK) exit(255);
              if (ErodeImg(p1, maskm, p2) != OK) exit(255);

              break;

            case 14:

              if (DilateImg(p1, 5, 5, p2) != OK) exit(255);

              break;
            case 15:

              if (ErodeImg(p1, 5, 5, p2) != OK) exit(255);

              break;
            case 17:
              abort = true;
            }/* switch*/

          double jetzt = TimeD(TM_MODE);
          times[iii] = jetzt - ctime;
          //  cout << iii << " : " << (jetzt-ctime)*1000 << endl;
          ctime = jetzt;
        }

      for (int i = 0; i < ntimes; i++)
        cout << times[i] * 1000 << " " ;

      cout << endl;
      Printf("Zeit: %6.2f ms\n", (TimeD(TM_MODE) - atime) / ntimes * 1000.0);
    }/* while */

  return 0;
}