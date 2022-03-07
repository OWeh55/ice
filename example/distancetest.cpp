#include <image.h>

int number = 900;

vector<Point> RandomList(int number, int maxval)
{
  vector<Point> res;

  for (int i = 0; i < number; i++)
    {
      res.push_back(Point(Random(maxval), Random(maxval)));
    }

  return res;
}

int DistanceMatrix(const vector<Point>& pl1,
                   const vector<Point>& pl2,
                   Matrix& distmatrix)
{
  Matrix mpl1(pl1.size(), 2);

  for (unsigned int i = 0; i < pl1.size(); i++)
    {
      mpl1[i][0] = pl1[i].x;
      mpl1[i][1] = pl1[i].y;
    }

  Matrix mpl2(pl1.size(), 2);

  for (unsigned int i = 0; i < pl2.size(); i++)
    {
      mpl2[i][0] = pl2[i].x;
      mpl2[i][1] = pl2[i].y;
    }

  return DistanceMatrix(mpl1, mpl2, distmatrix, D_EUCLID);
}

int main(int argc, char** argv)
{
  Image img;
  img.create(999 + 40, 999 + 40, 255 + 40);
  Show(ON, img);

  for (int i = 0; i < 200; i++)
    {
      cout << "Start" << endl;
      vector<Point> list1 = RandomList(number, 999);
      vector<Point> list2 = list1; // RandomList(number,999);

      cout << "Liste OK" << endl;

      for (unsigned int i = 0; i < list2.size(); i++)
        {
          list2[i].x *= 0.9;
          list2[i].y *= 0.9;
          list2[i].x += 55;
          list2[i].y += 77;
        }

      Matrix dist;
      DistanceMatrix(list1, list2, dist);
      cout << "Distanzmatrix OK" << endl;
      //  cout << dist << endl;
      IMatrix refpairs;
      Hungarian(dist, refpairs);
      cout << "Hungarian OK" << endl;

      clearImg(img);

      //  cout << Matrix(refpairs) << endl;
      for (int i = 0; i < refpairs.rows(); i++)
        {
          int i1 = refpairs[i][0];
          int i2 = refpairs[i][1];
          // cout << i1 << " " << i2 << endl;
          LineSeg l(list1[i1], list2[i2]);
          draw(l, img, 255);
          // Marker(1,list1[i].x,list1[i].y,127,5,img);
          // Marker(2,list2[i].x,list2[i].y,127,5,img);
        }

      GetChar();

    }

  return 0;
}
