#include "VelocimetryUtil.h"
#include "macro.h"

using namespace std;

namespace ice
{
  double** mallocDoubleArray(int w, int h);
  void freeDoubleArray(double** a, int w);
  void doubleArrayToImg(double** a, Image img, int w, int h);

#define FNAME "directCrossCorrelation"
  int directCrossCorrelation(ImageD img1, ImageD img2, ImageD img3,
                             int minDisplacementX, int minDisplacementY,
                             int maxDisplacementX, int maxDisplacementY)
  {
    RETURN_ERROR_IF_FAILED(MatchImgD(img1, img2, img3));

    if ((minDisplacementX >= maxDisplacementX) ||
        (minDisplacementY >= maxDisplacementY))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    RETURN_ERROR_IF_FAILED(directCrossCorrelation(img1, img2, img3,
                           0, 0, img1.xsize, img1.ysize,
                           0, 0, minDisplacementX, minDisplacementY,
                           maxDisplacementX, maxDisplacementY));
    return OK;
  }

  int directCrossCorrelation(ImageD img1, ImageD img2, ImageD img3,
                             int x1, int y1, int w, int h,
                             int offsetX, int offsetY,
                             int minDisplacementX, int minDisplacementY,
                             int maxDisplacementX, int maxDisplacementY)
  {
    int x2 = x1 + offsetX;
    int y2 = y1 + offsetY;

    for (int xd = minDisplacementX; xd <= maxDisplacementX; xd++)
      for (int yd = minDisplacementY; yd <= maxDisplacementY; yd++)
        {
          double statXY = 0;
          double statXX = 0;
          double statYY = 0;
          double statX = 0;
          double statY = 0;
          int anz = 0;

          for (int y = max(y2 + yd, 0) - yd;
               y < min(y2 + yd + h, img1.ysize - 1) - yd; ++y)
            {
              int xmin = max(x2 + xd, 0) - xd;
              int xmax = min((x2 + w) + xd, img1.xsize) - xd;
              anz += xmax - xmin;

              for (int x = xmax - xmin; x > 0; --x)
                {
                  double val1 = GetValD(img1, x, y);
                  double val2 = GetValD(img2, x + xd, y + yd);
                  statX += val1;
                  statY += val2;
                  statXX += (val1 * val1);
                  statYY += (val2 * val2);
                  statXY += (val1 * val2);
                }
            }

          if (anz == 0)
            {
              PutValD(img3, xd - minDisplacementX, yd - minDisplacementY, 0);
            }
          else
            {
              statX = statX / anz;
              statY = statY / anz;
              statXX = statXX / anz;
              statYY = statYY / anz;
              statXY = statXY / anz;
              double norm = sqrt((statXX - statX * statX) * (statYY - statY * statY));
              if (norm != 0)
                {
                  PutValD(img3, xd - minDisplacementX, yd - minDisplacementY,
                          (statXY - statX * statY) / norm);
                }
              else
                {
                  PutValD(img3, xd - minDisplacementX, yd - minDisplacementY, 0);
                }
            }
        }

    return OK;
  }

  int directCrossCorrelation(Image img1, Image img2, Image img3,
                             int minDisplacementX, int minDisplacementY,
                             int maxDisplacementX, int maxDisplacementY)
  {
    RETURN_ERROR_IF_FAILED(MatchImg(img1, img2, img3));

    if ((minDisplacementX >= maxDisplacementX) || (minDisplacementY >= maxDisplacementY))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    RETURN_ERROR_IF_FAILED(directCrossCorrelation(img1, img2, img3,
                           0, 0, img1->xsize, img1->ysize,
                           0, 0,
                           minDisplacementX, minDisplacementY,
                           maxDisplacementX, maxDisplacementY));
    return OK;
  }

  int directCrossCorrelation(Image img1, Image img2, Image img3,
                             int x1, int y1, int w, int h,
                             int offsetX, int offsetY,
                             int minDisplacementX, int minDisplacementY,
                             int maxDisplacementX, int maxDisplacementY)
  {
    double** cor;
    int x, y;
    int xd, yd;
    int    anz;
    double norm;
    cor = mallocDoubleArray(maxDisplacementX - minDisplacementX + 1,
                            maxDisplacementY - minDisplacementY + 1);

    double statXY;
    double statXX, statYY;
    double statX, statY;
    double val1, val2;

    int x2, y2;
    int xmin;
    int xmax;

    x2 = x1 + offsetX;
    y2 = y1 + offsetY;

    for (xd = minDisplacementX; xd <= maxDisplacementX; xd++)
      {
        for (yd = minDisplacementY; yd <= maxDisplacementY; yd++)
          {
            statXY = 0;
            statXX = 0;
            statYY = 0;
            statX = 0;
            statY = 0;
            anz = 0;

            for (y = max(y2 + yd, 0) - yd; y < min(y2 + yd + h, img1->ysize - 1) - yd; y++)
              {
                xmin = max(x2 + xd, 0) - xd;
                xmax = min((x2 + w) + xd, img1->xsize) - xd;
                anz += xmax - xmin;

                for (x = xmax - xmin; x > 0; x--)
                  {
                    val1 = (double)GetVal(img1, x, y);
                    val2 = (double)GetVal(img2, x + xd, y + yd);
                    statX += val1;
                    statY += val2;
                    statXX += (val1 * val1);
                    statYY += (val2 * val2);
                    statXY += (val1 * val2);
                  }
              }

            if (anz == 0)
              {
                cor[xd - minDisplacementX][yd - minDisplacementY] = 0;
              }
            else
              {
                statX = statX / anz;
                statY = statY / anz;
                statXX = statXX / anz;
                statYY = statYY / anz;
                statXY = statXY / anz;
                norm = sqrt((statXX - statX * statX) * (statYY - statY * statY));

                if (norm != 0)
                  {
                    cor[xd - minDisplacementX][yd - minDisplacementY] = (statXY - statX * statY) / norm;
                  }
                else
                  {
                    cor[xd - minDisplacementX][yd - minDisplacementY] = 0;
                  }
              }
          }
      }

    doubleArrayToImg(cor, img3,
                     maxDisplacementX - minDisplacementX + 1,
                     maxDisplacementY - minDisplacementY + 1);
    freeDoubleArray(cor, maxDisplacementX - minDisplacementX + 1);
    return OK;
  }

  double directCrossCorrelation(Image img1, Image img2,
                                int x1, int y1, int w, int h,
                                int xd, int yd)
  {
    int x, y;
    int    anz;
    double norm;

    double statXY;
    double statXX, statYY;
    double statX, statY;
    double val1, val2;

    int xmin;
    int xmax;

    statXY = 0;
    statXX = 0;
    statYY = 0;
    statX = 0;
    statY = 0;
    anz = 0;

    for (y = max(y1 + yd, 0) - yd; y < min(y1 + yd + h, img1->ysize - 1) - yd; y++)
      {
        xmin = max(x1 + xd, 0) - xd;
        xmax = min((x1 + w) + xd, img1->xsize) - xd;
        anz += xmax - xmin;

        for (x = xmax - xmin; x > 0; x--)
          {
            val1 = GetVal(img1, x, y);
            val2 = GetVal(img2, x + xd, y + yd);
            statX += val1;
            statY += val2;
            statXX += (val1 * val1);
            statYY += (val2 * val2);
            statXY += (val1 * val2);
          }
      }

    statX = statX / anz;
    statY = statY / anz;
    statXX = statXX / anz;
    statYY = statYY / anz;
    statXY = statXY / anz;

    norm = sqrt((statXX - statX * statX) * (statYY - statY * statY));

    if (norm != 0)
      {
        return (statXY - statX * statY) / norm;
      }
    else
      {
        return 0;
      }

    return ERROR;
  }

#undef FNAME

#define FNAME "directSquaredError"
  int directSquaredError(ImageD img1, ImageD img2, ImageD img3,
                         int minDisplacementX, int minDisplacementY,
                         int maxDisplacementX, int maxDisplacementY)
  {
    RETURN_ERROR_IF_FAILED(MatchImgD(img1, img2, img3));

    if ((minDisplacementX >= maxDisplacementX) ||
        (minDisplacementY >= maxDisplacementY))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    RETURN_ERROR_IF_FAILED(directSquaredError(img1, img2, img3,
                           0, 0, img1.xsize, img1.ysize,
                           0, 0,
                           minDisplacementX, minDisplacementY,
                           maxDisplacementX, maxDisplacementY));
    return OK;
  }

  int directSquaredError(ImageD img1, ImageD img2, ImageD img3,
                         int x1, int y1, int w, int h,
                         int offsetX, int offsetY,
                         int minDisplacementX, int minDisplacementY,
                         int maxDisplacementX, int maxDisplacementY)
  {
    int x, y;
    int xd, yd;
    double sum;
    int    anz;
    double s;

    int x2, y2;
    int xmin;
    int xmax;

    x2 = x1 + offsetX;
    y2 = y1 + offsetY;

    for (xd = minDisplacementX; xd <= maxDisplacementX; xd++)
      {
        for (yd = minDisplacementY; yd <= maxDisplacementY; yd++)
          {
            s = 0;
            sum = 0;
            anz = 0;

            for (y = max(y2 + yd, 0) - yd; y < min(y2 + yd + h, img1.ysize - 1) - yd; y++)
              {
                xmin = max(x2 + xd, 0) - xd;
                xmax = min((x2 + w) + xd, img1.xsize) - xd;
                anz += xmax - xmin;

                for (x = xmax - xmin; x > 0; x--)
                  {
                    s = (((double)GetValD(img1, x, y)) - ((double)GetValD(img2, x + xd, y + yd)));
                    sum += (s * s);
                  }
              }

            PutValD(img3, xd - minDisplacementX, yd - minDisplacementY, sum / (double)anz);
          }
      }

    return OK;
  }

  int directSquaredError(Image img1, Image img2, Image img3,
                         int minDisplacementX, int minDisplacementY,
                         int maxDisplacementX, int maxDisplacementY)
  {
    RETURN_ERROR_IF_FAILED(MatchImg(img1, img2, img3));

    if ((minDisplacementX >= maxDisplacementX) || (minDisplacementY >= maxDisplacementY))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    RETURN_ERROR_IF_FAILED(directSquaredError(img1, img2, img3,
                           0, 0, img1->xsize, img1->ysize,
                           0, 0, minDisplacementX, minDisplacementY,
                           maxDisplacementX, maxDisplacementY));
    return OK;
  }

  int directSquaredError(Image img1, Image img2, Image img3,
                         int x1, int y1, int w, int h,
                         int offsetX, int offsetY,
                         int minDisplacementX, int minDisplacementY,
                         int maxDisplacementX, int maxDisplacementY)
  {
    double** cor;
    int x, y;
    int xd, yd;
    double sum;
    int    anz;
    int s;
    cor = mallocDoubleArray(maxDisplacementX - minDisplacementX + 1, maxDisplacementY - minDisplacementY + 1);

    int x2, y2;
    int xmin;
    int xmax;

    x2 = x1 + offsetX;
    y2 = y1 + offsetY;

    for (xd = minDisplacementX; xd <= maxDisplacementX; xd++)
      {
        for (yd = minDisplacementY; yd <= maxDisplacementY; yd++)
          {
            s = 0;
            sum = 0;
            anz = 0;

            for (y = max(y2 + yd, 0) - yd; y < min(y2 + yd + h, img1->ysize - 1) - yd; y++)
              {
                xmin = max(x2 + xd, 0) - xd;
                xmax = min((x2 + w) + xd, img1->xsize) - xd;
                anz += xmax - xmin;

                for (x = xmax - xmin; x > 0; x--)
                  {
                    s = GetVal(img1, x, y) - GetVal(img2, x + xd, y + yd);
                    sum += (double)(s * s);
                  }
              }

            cor[xd - minDisplacementX][yd - minDisplacementY] = sum / anz;
          }
      }

    doubleArrayToImg(cor, img3, maxDisplacementX - minDisplacementX + 1, maxDisplacementY - minDisplacementY + 1);
    invertImg(img3, img3);
    freeDoubleArray(cor, maxDisplacementX - minDisplacementX + 1);
    return OK;
  }
#undef FNAME

#define FNAME "crossCorrelationImgD"
// IS2 = ID (*) IS1
  ImageD crossCorrelationImgD(ImageD is1, ImageD is2, ImageD id, int mode)
  {
    try
      {
        int x, y;
        int xq, yq;
        int xs, ys, x0, y0;

        ImageD rc;
        double rr, rq, ir, iq;
        double r1, r2, im1, im2;
        double r3 = 0, im3 = 0;
        //    double efactor;

        MatchImgD(is1, is2, xs, ys);

        x0 = (xs + 1) / 2;
        y0 = (ys + 1) / 2;

        if (id.isValid())
          {
            MatchImgD(is1, id);
            rc = id;
          }
        else
          {
            rc = NewImgD(xs, ys, 0, 1);
          }

        //    efactor=sqrt((double)(xs*ys));

        ImageD ds1 = NewImgD(xs, ys, 0, 1);
        ImageD ds2 = NewImgD(xs, ys, 0, 1);

        FourierImgD(is1, is2, NORMAL, ds1, ds2);

        for (y = 0; y < ys; y++)
          {
            yq = negf(y, ys);

            for (x = 0; x < xs; x++)
              {
                xq = negf(x, xs);
                rr = GetValD(ds1, x, y);
                ir = GetValD(ds2, x, y);
                rq = GetValD(ds1, xq, yq);
                iq = GetValD(ds2, xq, yq);
                r1 = (rr + rq) / 2;
                im1 = (ir - iq) / 2;
                r2 = (ir + iq) / 2;
                im2 = (rq - rr) / 2;

                r3 = (r2 * r1 + im2 * im1);
                im3 = (r1 * im2 - r2 * im1);
                PutValD(rc, x, y, r3 - im3);
              }
          }

        if ((mode & MD_BIAS) == MD_IGNORE_BIAS)
          {
            PutValD(rc, x0, y0, 0.0);
          }

        HartleyImgD(rc, rc);

        return rc;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "phaseCorrelation"
// IS2 = ID (*) IS1
  ImageD phaseCorrelationD(ImageD is1, ImageD is2, ImageD id,
                           double noise, int mode)
  {
    try
      {
        int xq, yq;
        int xs, ys, x0, y0;

        ImageD rc;
        double rr, rq, ir, iq;
        double r1, r2, im1, im2;
        double r3 = 0, im3 = 0;
        double b1;
        double efactor;

        if (noise < 0)
          throw IceException(FNAME, M_WRONG_PARAM);

        double noise2 = noise * noise;

        MatchImgD(is1, is2, xs, ys);

        x0 = (xs + 1) / 2;
        y0 = (ys + 1) / 2;

        if (id.isValid())
          {
            rc = id;
          }
        else
          {
            rc = NewImgD(xs, ys, 0, 1);
          }

        efactor = sqrt((double)(xs * ys));

        ImageD ds1 = NewImgD(xs, ys, 0, 1);
        ImageD ds2 = NewImgD(xs, ys, 0, 1);
        //  ImageD ddi=NewImgD(xs,ys,0,1);

        FourierImgD(is1, is2, NORMAL, ds1, ds2);

        for (int y = 0; y < ys; y++)
          {
            yq = negf(y, ys);

            for (int x = 0; x < xs; x++)
              {
                xq = negf(x, xs);
                rr = GetValD(ds1, x, y);
                ir = GetValD(ds2, x, y);
                rq = GetValD(ds1, xq, yq);
                iq = GetValD(ds2, xq, yq);
                r1 = (rr + rq) / 2;
                im1 = (ir - iq) / 2;
                r2 = (ir + iq) / 2;
                im2 = (rq - rr) / 2;

                b1 = sqrt((r1 * r1 + im1 * im1) * (r2 * r2 + im2 * im2));

                if (noise == 0)
                  {
                    if (b1 == 0)
                      {
                        r3 = im3 = 0;
                      }
                    else
                      {
                        r3 = (r2 * r1 + im2 * im1) / b1 * efactor;
                        im3 = (r1 * im2 - r2 * im1) / b1 * efactor;
                      }
                  }
                else
                  {
                    b1 += noise2;
                    r3 = (r2 * r1 + im2 * im1) / b1 * efactor;
                    im3 = (r1 * im2 - r2 * im1) / b1 * efactor;
                  }

                PutValD(rc, x, y, r3 - im3);
                //    PutValD(ddi,x,y,im3);
              }
          }

        if ((mode & MD_BIAS) == MD_IGNORE_BIAS)
          {
            PutValD(rc, x0, y0, 0.0);
          }

        HartleyImgD(rc, rc);

        return rc;
      }
    RETHROW;
  }

  Image phaseCorrelation(Image img1, Image img2, Image img3, double beta)
  {
    try
      {
        int xs, ys;
        MatchImg(img1, img2, img3, xs, ys);

        ImageD ds1 = NewImgD(xs, ys);
        ImageD ds2 = NewImgD(xs, ys);
        ImageD dd = NewImgD(xs, ys);
        ConvImgImgD(img1, ds1, NORMALIZED, SIGNED);
        ConvImgImgD(img2, ds2, NORMALIZED, SIGNED);

        phaseCorrelationD(ds1, ds2, dd, beta);
        ConvImgDImg(dd, img3, ADAPTIVE, UNSIGNED);

        return img3;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "horn"
  int horn(Image img1, Image img2, ImageD xDelta, ImageD yDelta, double lambda, int iterationNumber)
  {
    try
      {
        int kWidth;
        int kHeight;

        MatchImg(img1, img2, kWidth, kHeight);
        int kWidthd;
        int kHeightd;

        MatchImgD(xDelta, xDelta, kWidthd, kHeightd);

        if (iterationNumber <= 0 || lambda <= 0)
          throw IceException(FNAME, M_WRONG_PARAM);

        int MASK_DIM = 2;

        double** f_x = new double *[kHeight];
        double** f_y = new double *[kHeight];
        double** f_t = new double *[kHeight];
        double** u = new double *[kHeight];
        double** u_avg = new double *[kHeight];
        double** v = new double *[kHeight];
        double** v_avg = new double *[kHeight];

        for (int i = 0; i < kHeight; i++)
          {
            f_x[i] = new double[kWidth];
            f_y[i] = new double[kWidth];
            f_t[i] = new double[kWidth];
            u[i] = new double[kWidth];
            u_avg[i] = new double[kWidth];
            v[i] = new double[kWidth];
            v_avg[i] = new double[kWidth];
          }

        //derivative masks
        int f_x_mask[][2] = {{ -1, 1}, { -1, 1}};
        int f_y_mask[][2] = {{ -1, -1}, {1, 1}};
        int f_t_mask_pre[][2] = {{ -1, -1}, { -1, -1}};
        int f_t_mask_post[][2] = {{1, 1}, {1, 1}};

        for (int i = 0; i < kHeight; i++)
          {
            for (int j = 0; j < kWidth; j++)
              {
                u_avg[i][j] = 0.0F;
                v_avg[i][j] = 0.0F;
                u[i][j] = 0.0F;
                v[i][j] = 0.0F;
                f_x[i][j] = 0.0F;
                f_y[i][j] = 0.0F;
                f_t[i][j] = 0.0F;
              }
          }

        //calc_derivative_masks();

        int mr = (int)(MASK_DIM * 0.5); //radius of mask
        int sum_x, sum_y, sum_t;
        //~ int max_dim = kWidth - mr;

        //perform convolution operation using derivative masks
        for (int i = mr; i < kHeight; i++)
          {
            for (int j = mr; j < kWidth; j++)
              {

                sum_x = 0;
                sum_y = 0;
                sum_t = 0;

                //apply derivative masks for fx, fy, and ft
                for (int y = 0; y < MASK_DIM; y++)
                  {
                    for (int x = 0; x < MASK_DIM; x++)
                      {
                        sum_x += f_x_mask[y][x] *
                                 (GetVal(img1, j + x - 1, i + y - 1) + GetVal(img2, j + x - 1, i + y - 1));
                        sum_y += f_y_mask[y][x] *
                                 (GetVal(img1, j + x - 1, i + y - 1) + GetVal(img2, j + x - 1, i + y - 1));
                        sum_t += f_t_mask_pre[y][x] * GetVal(img1, j + x - 1, i + y - 1) +
                                 f_t_mask_post[y][x] * GetVal(img2, j + x - 1, i + y - 1);
                      }
                  }

                f_x[i][j] = (double)(sum_x) * 0.25; //* by 0.25 to average
                f_y[i][j] = (double)(sum_y) * 0.25;
                f_t[i][j] = (double)(sum_t) * 0.25;
                //~ f_t[i][j] = 0.25F;
              }
          }

        for (int it = 0; it < iterationNumber; it++)
          {

            printf("Iteration %d\n", it);

            //calc_uv_averages();

            float u_sum, v_sum;

            for (int i = 1; i < kHeight - 1; i++)
              {
                for (int j = 1; j < kWidth - 1; j++)
                  {
                    u_sum = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) * 0.25;
                    v_sum = (v[i - 1][j] + v[i + 1][j] + v[i][j - 1] + v[i][j + 1]) * 0.25;
                    u_avg[i][j] = u_sum;
                    v_avg[i][j] = v_sum;
                  }
              }

            //calc_uv();

            float p_over_d = 0; // P/D

            for (int y = 0; y < kHeight; y++)
              {
                for (int x = 0; x < kWidth; x++)
                  {
                    p_over_d = lambda * ((f_x[y][x] * u_avg[y][x] +
                                          f_y[y][x] * v_avg[y][x] + f_t[y][x]) /
                                         (1 + lambda * (f_x[y][x] * f_x[y][x]) + (f_y[y][x] * f_y[y][x])));
                    u[y][x] = u_avg[y][x] - f_x[y][x] * p_over_d;
                    v[y][x] = v_avg[y][x] - f_y[y][x] * p_over_d;
                    //~ u[y][x] = f_t[y][x];
                    //~ v[y][x] = f_t[y][x];
                  }
              }

          }

        for (int i = 0; i < kHeight; i++)
          {
            for (int j = 0; j < kWidth; j++)
              {
                PutValD(xDelta, j, i, u[i][j]);
                PutValD(yDelta, j, i, v[i][j]);
              }

          }

        for (int i = 0 ; i < kHeight ; i++)
          {
            delete [] f_x[i];
            delete [] f_y[i];
            delete [] f_t[i];
            delete [] u[i];
            delete [] u_avg[i];
            delete [] v[i];
            delete [] v_avg[i];
          }

        delete [] f_x;
        delete [] f_y;
        delete [] f_t;
        delete [] u;
        delete [] u_avg;
        delete [] v;
        delete [] v_avg;

        return OK;
      }
    RETHROW;
  }

#undef FNAME

#define FNAME "horn"
  int showOpticalFlow(Image img, ImageD xDelta, ImageD yDelta, int subSampleSize, double vectorScale, int color)
  {
    if (!(IsImgD(xDelta) && IsImgD(yDelta)))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if (!(IsImg(img)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int sizeX = xDelta.xsize;
    int sizeY = xDelta.ysize;

    if (!(subSampleSize > 0 && subSampleSize <= sizeX && subSampleSize <= sizeY && vectorScale != 0))
      throw IceException(FNAME, M_WRONG_PARAM);

    int x, y;
    double x1, y1;
    double x2, y2;
    int gridSizeX = sizeX / subSampleSize;
    int gridSizeY = sizeY / subSampleSize;
    double sumx, sumy;
    double xd, yd;

    for (int i = 0; i < gridSizeX; i++)
      {
        for (int j = 0; j < gridSizeY; j++)
          {
            sumx = 0;
            sumy = 0;

            for (x = i * subSampleSize; x < (i + 1)*subSampleSize; x++)
              {
                for (y = j * subSampleSize; y < (j + 1)*subSampleSize; y++)
                  {
                    sumx += GetValD(xDelta, x, y);
                    sumy += GetValD(yDelta, x, y);
                  }
              }

            xd = sumx / (subSampleSize * subSampleSize);
            yd = sumy / (subSampleSize * subSampleSize);
            x1 = i * subSampleSize + (subSampleSize / 2);
            y1 = j * subSampleSize + (subSampleSize / 2);
            x2 = x1 + xd * vectorScale;
            y2 = y1 + yd * vectorScale;
            drawArrow((int)x1, (int)y1, (int)(x2), (int)y2, color, 5, img);
          }
      }

    return OK;
  }
#undef FNAME

  void FlipX(Image& img)
  {
    int w = img->xsize;
    int h = img->ysize;
    int w2 = w / 2;

    for (int y = 0; y < h; y++)
      {
        for (int x = 0; x < w2; x++)
          {
            int xi = w - x - 1;
            int val = GetVal(img, x, y);
            PutVal(img, x, y, GetVal(img, xi, y));
            PutVal(img, xi, y, val);
          }
      }
  }

  void FlipY(Image& img)
  {
    int w = img->xsize;
    int h = img->ysize;
    int h2 = h / 2;

    for (int y = 0; y < h2; y++)
      {
        for (int x = 0; x < w; x++)
          {
            int yi = h - 1 - y;
            int val = GetVal(img, x, y);
            PutVal(img, x, y, GetVal(img, x, yi));
            PutVal(img, x, yi, val);
          }
      }
  }

  void Flip(Image& img)
  {
    FlipX(img);
    FlipY(img);
  }

  void drawArrow(int x1, int y1, int x2, int y2, int val, int size, Image img)
  {
    double  phi;
    double xd = x2 - x1;
    double yd = y2 - y1;
    double circleData[5];

    if (xd == 0)
      {
        phi = yd > 0 ? M_PI * 1.5 : M_PI / 2;
      }
    else
      {
        phi = atan(yd / xd);

        if (xd > 0)
          {
            phi = phi + M_PI;
          }
      }

    //~ printf("phi=%f %f\n",phi,PI);
    Line(x1, y1, x2, y2, val, DEFAULT, img);
    circleData[0] = x2;
    circleData[1] = y2;
    circleData[2] = size;
    circleData[3] = phi - 0.5;
    circleData[4] = phi + 0.5;
    drawCircleSegment(circleData, val, val, DEFAULT, img);
  }

#define  FNAME "parseParaFile"
  int parseParaFile(const std::string& filename, map<string, string>& paraMap)
  {
    ifstream inFile(filename.c_str(), ios_base::in);
    string  lineString;
    string paraName;
    string paraValue;

    if (!inFile)
      throw IceException(FNAME, M_FILE_OPEN);

    while (getline(inFile, lineString))
      {
        size_t substringStart = lineString.find_first_not_of(" ");

        if (substringStart != string::npos)
          {

            size_t equalPos = lineString.find("=", substringStart);

            if (equalPos != string::npos)
              {

                size_t substringEnd = lineString.find_last_not_of(" ", equalPos - 1);

                paraName = lineString.substr(substringStart, substringEnd - substringStart + 1);

                substringStart = lineString.find_first_not_of("\" ", equalPos + 1);

                if (substringStart != string::npos)
                  {

                    substringEnd = lineString.find_last_not_of("\" ", lineString.length());
                    paraValue = lineString.substr(substringStart, substringEnd - substringStart + 1);
                    paraMap[paraName] = paraValue;
                  }
              }
          }
      }

    return OK;
  }
#undef FNAME

  Image pivRenormImg(Image img1, Image img2)
  {
    int g;
    int ming = 255;
    int maxg = 0;
    int scale;
    for (int y = 0; y < img1.ysize; y++)
      for (int x = 0; x < img1.xsize; x++)
        {
          g = GetVal(img1, x, y);
          ming = min(ming, g);
          maxg = max(maxg, g);
        }
    printf("%d %d\n", ming, maxg);
    scale = maxg - ming;

    if (scale > 0)
      {
        for (int y = 0; y < img1.ysize; y++)
          for (int x = 0; x < img1.xsize; x++)
            {
              g = GetVal(img1, x, y);
              g = (g - ming) * 255 / scale;
              PutVal(img2, x, y, g);
            }
      }

    return img2;
  }

  Image pivMexicanImg(Image img1,
                      int size, double sigma1, double sigma2,
                      Image img2)
  {
    Image puffer1 = NewImg(img1->xsize, img1->ysize, 255);
    Image puffer2 = NewImg(img1->xsize, img1->ysize, 255);
    GaussImg(img1, puffer1, size, sigma1);
    GaussImg(img1, puffer2, size, sigma2);
    subImg(puffer1, puffer2, 1, img2);
    pivRenormImg(img2, img2);
    return img2;
  }

  double** mallocDoubleArray(int w, int h)
  {
    double** a;
    int i;
    a = (double**)malloc(w * sizeof(double*));

    for (i = 0; i < w; i++)
      {
        a[i] = (double*)malloc(h * sizeof(double));
      }

    return a;
  }

  void freeDoubleArray(double** a, int w)
  {
    int i;

    for (i = 0; i < w; i++)
      {
        free(a[i]);
      }

    free(a);
  }

  void doubleArrayToImg(double** a, Image img, int w, int h)
  {
    int x, y;
    double maximum = DBL_MIN;
    double minimum = DBL_MAX;

    for (x = 0; x < w; x++)   // Maximum finden
      {
        for (y = 0; y < h; y++)
          {
            maximum = max(maximum, a[x][y]);
            minimum = min(minimum, a[x][y]);
          }
      }

    //~ printf("min %e max %e \n",minimum,maximum);
    maximum = maximum - minimum;

    for (x = 0; x < w; x++)   // nach img kopieren
      {
        for (y = 0; y < h; y++)
          {
            PutVal(img, x, y, max(min((int)((a[x][y] - minimum) * 255.0 / maximum), 255), 0));
          }
      }
  }

  void makePositivImgD(ImageD img)
  {
    double minD = DBL_MAX;;
    for (int y = 0; y < img.ysize; ++y)
      for (int x = 0; x < img.xsize; ++x)
        {
          minD = min(GetValD(img, x, y), minD);
        }

    for (int y = 0; y < img.ysize; ++y)
      for (int x = 0; x < img.xsize; ++x)
        {
          PutValD(img, x, y, GetValD(img, x, y) - minD);
        }
  }
}
