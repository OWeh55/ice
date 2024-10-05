#include <image.h>
#include <ClassSample.h>

using namespace ice;
using namespace std;

int main(int argc, char** argv)
{
  //  ClassSample s;

  vector<ClassSample> sl;
  sl.push_back(ClassSample(1, vector<double> {1.1, 2, 3}));
  sl.push_back(ClassSample(1, vector<double> {2.2, 2, 3}));
  sl.push_back(ClassSample(1, vector<double> {3.1, 2, 3}));
  sl.push_back(ClassSample(2, vector<double> {1.2, 4, 3}));
  sl.push_back(ClassSample(2, vector<double> {2.1, 4, 3}));
  sl.push_back(ClassSample(2, vector<double> {3.2, 4, 0}));
  sl.push_back(ClassSample(0, vector<double> {2.1, 4, 4}));
  sl.push_back(ClassSample(0, vector<double> {2.2, 4, 2}));

  KDTree<ClassSample> tr;
  tr.create(sl);

  tr.statistics();
  const ClassSample* p = tr.findNearest(vector<double> {2, 4, 3});
  cout << "Klasse: " << p->classNr << endl;
  cout << "Merkmale ";
  for (unsigned int i = 0; i < p->features.size(); ++i)
    cout << p->features[i] << " ";

  cout << endl;
  cout << "--- 3 nearest " << endl;

  vector<const ClassSample*> res;
  vector<double> dist;
  tr.findKNearest(vector<double> {2.1, 4, 4}, 5,
                  res, dist);

  for (unsigned int k = 0; k < res.size(); ++k)
    {
      cout << res[k]->classNr << " : ";
      for (int i = 0; i < res[k]->size(); ++i)
        cout << res[k]->features[i] << " ";
      cout << " " << dist[k];
      cout << endl;
    }

  KDTree<ClassSample> itr;
  vector<ClassSample> isl;
  isl.push_back(ClassSample(1, vector<double> {50, 50}));
  isl.push_back(ClassSample(2, vector<double> {850, 100}));
  isl.push_back(ClassSample(3, vector<double> {555, 666}));
  isl.push_back(ClassSample(4, vector<double> {550, 666}));
  isl.push_back(ClassSample(5, vector<double> {500, 666}));

  itr.create(isl);
  itr.statistics();
  Image img;
  img.create(999, 777, 16);
  Show(OVERLAY, img);
  for (int y = 0; y < 777; ++y)
    for (int x = 0; x < 999; ++x)
      {
        vector<double> f(2);
        f[0] = x;
        f[1] = y;
        const ClassSample* p = itr.findNearest(f);
        img.setPixel(x, y, p->classNr);
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

  itr.create(isl);

  vector<const ClassSample*> samples;
  int x, y;
  while (Mouse(img, x, y) & (M_RIGHT_DOWN | M_LEFT_DOWN))
    {
      Print("Mouse-Button pressed\n");
    }
  Print("Press Mouse-Button to finish!\n");
  while (!(Mouse(img, x, y) & (M_RIGHT_DOWN | M_LEFT_DOWN)))
    {

      for (unsigned int i = 0; i < samples.size(); ++i)
        {
          img.setPixel((*samples[i])[0], (*samples[i])[1], 2);
        }

      itr.findNeighbors(ClassSample(1, vector<double> {double(x), double(y)}),
                        30, samples);

      for (unsigned int i = 0; i < samples.size(); ++i)
        {
          img.setPixel((*samples[i])[0], (*samples[i])[1], 1);
        }
      usleep(100000);
    }
  Print("Press <ENTER> to finish");
  GetChar();
  return 0;
}
