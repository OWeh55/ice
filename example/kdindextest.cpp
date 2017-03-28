#include <image.h>
#include <ClassSample.h>
#include <KDIndexTree.h>

using namespace ice;
using namespace std;

int main(int argc, char** argv)
{
  ClassSample s;

  vector<ClassSample> sl;
  KDIndexTree<ClassSample> tr(sl);

  sl.push_back(ClassSample(1, vector<double> {1.1, 2, 3}));
  sl.push_back(ClassSample(1, vector<double> {2.2, 2, 3}));
  sl.push_back(ClassSample(1, vector<double> {3.1, 2, 3}));
  sl.push_back(ClassSample(2, vector<double> {1.2, 4, 3}));
  sl.push_back(ClassSample(2, vector<double> {2.1, 4, 3}));
  sl.push_back(ClassSample(2, vector<double> {3.2, 4, 0}));
  sl.push_back(ClassSample(0, vector<double> {2.1, 4, 4}));
  sl.push_back(ClassSample(0, vector<double> {2.2, 4, 2}));

  tr.create();

  tr.statistics();
  int p = tr.findNearest(vector<double> {2, 4, 3});
  cout << "Klasse: " << sl[p].classNr << endl;
  cout << "Merkmale ";
  for (int i = 0; i < sl[p].features.size(); ++i)
    cout << sl[p].features[i] << " ";

  cout << endl;

  cout << "--- 5 nearest " << endl;

  vector<int> res;
  vector<double> dist;
  tr.findKNearest(vector<double> {2.1, 4, 4}, 5,
                  res, dist);

  for (int k = 0; k < res.size(); ++k)
    {
      cout << sl[res[k]].classNr << " : ";
      for (int i = 0; i < sl[res[k]].size(); ++i)
        cout << sl[res[k]].features[i] << " ";
      cout << " " << dist[k];
      cout << endl;
    }


  vector<ClassSample> isl;
  KDIndexTree<ClassSample> itr(isl);

  isl.push_back(ClassSample(1, vector<double> {50, 50}));
  isl.push_back(ClassSample(2, vector<double> {850, 100}));
  isl.push_back(ClassSample(3, vector<double> {555, 666}));
  isl.push_back(ClassSample(4, vector<double> {550, 666}));
  isl.push_back(ClassSample(5, vector<double> {500, 666}));


  itr.create();
  itr.statistics();
  Image img;
  img.create(999, 777, 16);
  Visual v = Show(OVERLAY, img);
  for (int y = 0; y < 777; ++y)
    for (int x = 0; x < 999; ++x)
      {
        vector<double> f(2);
        f[0] = x;
        f[1] = y;
        int p = itr.findNearest(f);
        img.setPixel(x, y, isl[p].classNr);
      }

  GetChar();

  img.set(0);
  isl.clear();
  for (int y = 0; y < 777; ++y)
    for (int x = 0; x < 999; ++x)
      {
        double r = drand48();
        if (r < 0.01)
          {
            isl.push_back(ClassSample(1, vector<double> {double(x), double(y)}));
            img.setPixel(x, y, 2);
          }
      }

  itr.create();
  int x, y;

  vector<int> samples;
  while (Mouse(img, x, y) & (M_RIGHT_DOWN | M_LEFT_DOWN))
    {
      Print("Mouse-Button pressed\n");
    }
  Print("Press Mouse-Button to finish!\n");
  while (!(Mouse(img, x, y) & (M_RIGHT_DOWN | M_LEFT_DOWN)))
    {
      vector<double> v {double(x), double(y)};
      for (int i = 0; i < samples.size(); ++i)
        {
          img.setPixel(isl[samples[i]][0], isl[samples[i]][1], 2);
        }
      itr.findNeighbors(v, 30, samples);
      for (int i = 0; i < samples.size(); ++i)
        {
          img.setPixel(isl[samples[i]][0], isl[samples[i]][1], 1);
        }
      usleep(100000);
    }
  Print("Press <ENTER> to finish");
  GetChar();
}
