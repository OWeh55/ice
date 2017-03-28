#include <image.h>

const int ss = 2;

void Update(ImageD d, Image& i)
{
  UpdateLimitImgD(d);
  ConvImgDImg(d, i);
}

int main(int argc, char** argv)
{
  Image img;
  img = ReadImg("test_gray.jpg");
  //img=ReadImg("test.jpg");
  //  InvertImg(img,img);
  Show(ON, img);
#if 0
  ClearImg(img);
  Point center = Point(img->xsize / 2, img->ysize / 2);
  //  cout << sizeof(center) << endl;
  double rad = img->ysize / 3;
  Circle(center, rad).Draw(img, 255);
  Point dx(rad * 2 / 3, 0);
  Point dy(0, rad * 2 / 3);
  LineSeg(center - dx, center + dx).Draw(img, 222);
  LineSeg(center - dy, center + dy).Draw(img, 222);

  Point dxy = 0.5 * (dx + dy);
  Circle(center - dxy, rad / 5).Draw(img, 222, 222);

  //  for (int y=0;y<img->ysize;y++)
  //    for (int x=0;x<img->xsize/2;x++)
  //      PutVal(img,x,y,GetVal(img,x,y)/2);


  DilateImg(img, 3, 3, img);
  SmearImg(img);
#endif
  ImageD ddst = NewImgD(img);
  Image dst = NewImg(img);
  Show(ON, dst);
  IPoint dd(ss, ss);
  int ct = 0;

  IPoint p;
  for (p.y = 0; p.y < img->ysize; p.y++)
    for (p.x = 0; p.x < img->xsize; p.x++)
      {
        if (Inside(img, p - dd) && Inside(img, p + dd))
          {
            Polynom2d2o f;
            f.FitInit();

            for (int dy = -ss; dy <= ss; dy++)
              for (int dx = -ss; dx <= ss; dx++)
                {
                  IPoint h = p + IPoint(dx, dy);
                  int g = GetVal(img, h);
                  f.FitVal(dx, dy, g);
                }

            double a00, a10, a01, a20, a11, a02;
            f.getCoefficient(a00, a10, a01, a20, a11, a02);
            //  Vector v=f.Coeff();
            double h11 = a20;
            double h22 = a02;
            double h12 = a11 * 0.5;

            double pp = -(h11 + h22);
            double qq = h11 * h22 - h12 * h12;

            double delta = pp * pp / 4 - qq;

            double grad1 = sqrt(a01 * a01 + a10 * a10);

            PutValD(ddst, p, a11);
            // PutValD(ddst,p,delta);
            //      PutValD(ddst,p,delta/(grad1+10));

            //    PutValD(ddst,p,limited((a20+a02)*10,img));
            //    PutValD(ddst,p,limited(a00,img));
            //    PutValD(ddst,p,limited(a10+128,img));
            //    PutValD(ddst,p,limited(a01+128,img));
            //    PutValD(ddst,p,limited(a20+128,img));
            //      PutValD(ddst,p,limited(a02+128,img));
            //    PutValD(ddst,p,limited(a11+128,img));
            //    PutValD(ddst,p,limited((a01*a01+a10*a10)/10,img));
            //    PutValD(ddst,p,grad1);
          }

        ct++;

        if (ct % 3333 == 0)
          Update(ddst, dst);
      }

  Update(ddst, dst);
  GetChar();
  return OK;
}
