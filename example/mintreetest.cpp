//
// test for MinTree function and Forest class
//

#include <cstdlib>
#include <image.h>

void usage(const string& pn)
{
  cout << pn << " - Test der Funktion Mintree" << endl << endl;
  cout << "Aufruf: " << endl;
  cout << "       " << pn << " [option]*" << endl;
  cout << "Optionen:" << endl;
  cout << " -h          Hilfe" << endl;
  cout << " -n <nr>     Verwende <nr> Punkte" << endl;
  cout << " -x <width>  Breite des Bildes" << endl;
  cout << " -y <height> Höhe des Bildes" << endl;
  cout << " -l <len>    Berechnung mit maximaler Länge <len>" << endl;
  cout << " -c <nr>     Berechnung mit Cluster-Anzahl <nr>" << endl;
  exit(1);
}

int main(int argc, char** argv)
{
  int rc;
  bool regular = false;
  double maxlen = 0.0;
  double minlen = 0.0;
  int punkte = 5000;
  int sleep = 10000;
  int xsize = 512;
  int ysize = 512;
  int cluster = 0;

  while ((rc = getopt(argc, argv, "hn:l:u:x:y:rm:c:")) >= 0)
    {
      switch (rc)
        {
        case 'l':
          maxlen = atof(optarg);
          break;
        case 'n':
          punkte = atol(optarg);
          break;
        case 'u':
          sleep  = atol(optarg);
          break;
        case 'x':
          xsize  = atol(optarg);
          break;
        case 'y':
          ysize  = atol(optarg);
          break;
        case 'r':
          regular = true;
          break;
        case 'm':
          minlen = atof(optarg);
          break;
        case 'c':
          cluster = atoi(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  Alpha(ON);

  Image pic;
  pic.create(xsize, ysize, 25500);
  ClearImg(pic);
  Show(OVERLAY, pic);

  int rand1 = 5;
  int gap = 100;

  vector<Point> Punkte;

  if (!regular)
    {
      double fsizex = (xsize - 2 * rand1 - gap) / 2;
      double sx = xsize - 2 * rand1 - fsizex;
      double fsizey = (ysize - 2 * rand1 - gap) / 2;
      double sy = ysize - 2 * rand1 - fsizey;

      //  Punkte.Append(Vector(1,1));
      for (int i = 1; i < punkte; i++)
        {
          int idx = Random(3);
          double x = RandomD() * fsizex + rand1 + (idx & 1) * sx;
          double y = RandomD() * fsizey + rand1 + (idx / 2) * sy;
          Punkte.push_back(Point(x, y));
        }
    }
  else
    {
      int dim = 1;

      while (dim * dim < punkte) dim++;

      int dx = (xsize - 20) / (dim - 1);
      int dy = (ysize - 20) / (dim - 1);

      for (int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
          {
            double x = i * dx + 10 /* +(rand()%100)*0.01*/;
            double y = j * dy + 10 /* +(rand()%100)*0.01*/;
            Punkte.push_back(Point(x, y));
          }
    }

  Forest f;
  double atime = TimeD();

  MinTree minTree(Punkte);
  Printf("Zeit: %5.2f\n", TimeD() - atime);

  double mintreelen = minTree.getTree(f);

  Printf("Länge der Kanten %5.2f\n", mintreelen);

  for (int i = 0; i < f.Size(); i++)
    {
      if (!f.isRoot(i))
        {
          int idx = f.Father(i);
          Line(Punkte[i], Punkte[idx], 1, pic);
        }

      //  cout << i << " " << f.father(i) << endl;
    }

  GetChar();

  unsigned int ui = -1;

  if (maxlen > 0.0)
    {
      atime = TimeD();
      mintreelen = minTree.getForest(f, maxlen);
      Printf("Zeit: %5.2f\n", TimeD() - atime);
      Printf("Länge der Kanten %5.2f\n", mintreelen);

      if (minlen > 0.0)
        cutShortBranches(f, Punkte, minlen);

      int color = 2;
      map<unsigned int, int> colormap;

      for (int i = 0; i < f.Size(); i++)
        {
          if (!f.isRoot(i))
            {
              int idx = f.Father(i);
              unsigned int root = f.Root(i);

              if (colormap.count(root) == 0)
                {
                  colormap[root] = color;
                  color++;
                }

              Line(Punkte[i], Punkte[idx], colormap[root], pic);
            }
        }

      GetChar();
    }

  if (cluster > 0)
    {
      atime = TimeD();
      mintreelen = minTree.getCluster(f, cluster);
      Printf("Zeit: %5.2f\n", TimeD() - atime);
      Printf("Länge der Kanten %5.2f\n", mintreelen);

      ClearImg(pic);
      int color = 2;
      map<unsigned int, int> colormap;

      for (int i = 0; i < f.Size(); i++)
        {
          if (!f.isRoot(i))
            {
              int idx = f.Father(i);
              unsigned int root = f.Root(i);

              if (colormap.count(root) == 0)
                {
                  colormap[root] = color;
                  color++;
                }

              Line(Punkte[i], Punkte[idx], colormap[root], pic);
            }
        }

    }
#if 0


  else
    {
      vector<vector<int> > pathlist = SplitToBranches(f);
      // ClearImg(pic);
      cout << pathlist.size() << " Äste " << endl;

      for (unsigned int i = 0; i < pathlist.size(); i++)
        {
          for (unsigned int j = 1; j < pathlist[i].size(); j++)
            {
              Line(Punkte[pathlist[i][j - 1]],
                   Punkte[pathlist[i][j]],
                   (i % 6) + 2, pic);
            }
        }
    }
#endif

  Print("Fertig!\nPress Enter");
  GetChar();
  return OK;
}
