// test zu den Klassifikatoren
#include <image.h>

int classes = 3;
int samples = 5;

int main(int argc, char* argv[])
{
  Display(ON);
  Alpha(ON);
  ClearAlpha();
  //  Image p1 = ReadImg("test_rgb.jpg");
  Image p1 = NewImg(512, 512, 255);
  Image m1 = NewImg(p1);
  m1.set(0);
  Show(OVERLAY, p1, m1);

  vector<vector<double>> features;
  vector<int> classnr;

  // ClassifierBayes cl(classes, 2), cln(classes, 2);
  // ClassifierBayes2 cl(2,2,0.5,0.5,1,1), cln; classes=2;
  // ClassifierMinimumDistance cl(classes,2), cln(classes,2);
  ClassifierNearestNeighbor cl(classes, 2, 1, true), cln(classes, 2);
  //ClassifierNearestNeighbor cl(classes, 2, 3), cln(classes, 2); // == KNN
  // ClassifierLinear cl(classes, 2), cln;

  Print("Select points\n");

  for (int c = 0; c < classes; c++)
    {
      Printf("%d. von %d Klassen\n", c + 1, classes);
      for (int i = 0; i < samples; i++)
        {
          Printf(" %d. von %d Punkten.\n", i + 1, samples);
          IPoint p = SelPoint(DEFAULT, p1);
          vector<double> v {(double)p.x, (double)p.y};
          features.push_back(v);
          classnr.push_back(c);
          cl.Train(c, v);
          Marker(1, p.x, p.y, c + 3, 5, m1);
        }
      Print("==========================================\n");
    }

  int nSamples = features.size();

  Print("Finishing...");
  cl.Finish();
  Print("done.\n");

  for (int y = 0; y < m1.ysize; y++)
    for (int x = 0; x < m1.xsize; x++)
      {
        m1.setPixel(x, y, cl.Classify(vector<double> {(double)x, (double)y}) + 3);
      }
#if 1
  for (int s = 0; s < nSamples; s++)
    {
      int x = features[s][0];
      int y = features[s][1];
      int c = classnr[s];
      Marker(1, x, y, c + 1, 5, m1);
    }
#endif

  GetChar();

  Print("Writing to file...\n");
  cl.write("TestKlassifikator");
  Print("Reading from file...\n");
  cln.read("TestKlassifikator");

  int nWrongPixels = 0;
  for (int y = 0; y < m1->ysize; y++)
    for (int x = 0; x < m1->xsize; x++)
      {
        int classNr = cln.Classify(Vector((double)x, (double)y));
        int oldClass = m1.getPixel(x, y) - 3;
        if (classNr != oldClass)
          {
            m1.setPixel(x, y, 1);
            ++nWrongPixels;
          }
      }

  Printf("%d Pixels are misclassified after rereading classifier\n", nWrongPixels);
  GetChar();

  return 0;
}
