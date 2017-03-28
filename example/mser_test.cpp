#include <image.h>

inline bool InRange(int max1, int val)
{
  return val >= 0 && val < max1;
}

void usage(const string& pn)
{
  cout << "Usage: " << pn << " <options> [<filename>] " << endl;
  cout << "options:" << endl;
  cout << " -d <n> Darstellungen (Bit-Werte)" << endl;
  cout << "   -d 1 Originalbild" << endl;
  cout << "   -d 2 Regionen gleichen Wertes" << endl;
  cout << "   -d 4 Komponenten-Baum" << endl;
  cout << " -D <d> Delta für mser (int, default:5)" << endl;
  cout << " -m <n> Maskierung aller Grauwerte - AND n" << endl;
  cout << " -v     Ausgaben" << endl;
  cout << " -s <n> Smear n*n vor Analyse" << endl;
  cout << " -n <n> Rauschen der Stärke n" << endl;
  cout << " -a     Minimale Fläche von smer (default:1)" << endl;
  cout << " -h     help" << endl;
  exit(1);
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  int display = 2;
  int mask = -1;
  int print = 0;
  int smear = 0;
  int noise = 0;
  int minarea = 1;
  int verbose = false;
  int delta = 5;

  int rc;

  while ((rc = getopt(argc, argv, "hd:m:s:n:p:vD:a:")) >= 0)
    {
      switch (rc)
        {
        case 'd':
          display = atol(optarg);
          break;
        case 'D':
          delta = atol(optarg);
          break;
        case 'a':
          minarea = atol(optarg);
          break;
        case 'm':
          mask = atol(optarg);
          break;
        case 'p':
          print = atol(optarg);
          break;
        case 's':
          smear = atol(optarg);
          break;
        case 'n':
          noise = atol(optarg);
          break;
        case 'v':
          verbose = true;
          break;
        case 'h': // no break !!
        default:
          usage(argv[0]);
          break;
        }
    }

  if (optind < argc)
    fn = argv[optind];

  int xs, ys, mv, ch;

  InfImgFile(fn, xs, ys, mv, ch);

  Image img1 = NewImg(xs, ys, mv);
  ReadImg(fn, img1);
  Image mrk = NewImg(img1);
  ClearImg(mrk);

  if (display & 1)
    Show(ON, img1, "Ausgangsbild");

  if (smear > 0)
    SmearImg(img1, img1, 5, 5);

  if (noise > 0)
    {
      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          {
            int val = GetVal(img1, x, y) + Random(noise);

            if (val > 255) val = 255;

            PutVal(img1, x, y, val);
          }
    }

  if (mask > 0)
    {
      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          PutVal(img1, x, y, GetVal(img1, x, y) & mask);
    }

  ComponentTreeNode* root = ComponentTree(img1);

  vector<int> area;

  Print("Berechne Stabilität\n");
  root->Eval(delta, area);
  Print("Berechne Stabilität .. done\n");

  if (print > 0)
    root->print(print, 0);

  if (display & 2)
    {
      Show(OVERLAY, img1, mrk, "Componenten");
      //    ClearImg(img1);
      ClearImg(mrk);
      Print("Berechne Totale Regionen\n");

      root->MakeTotalReg();

      Print("Berechne Totale Regionen .. done\n");

      // Printf("Baum selbst regs:%d\n" ,root->Draw(mrk));
      // root->print(9999);
      // GetChar();

      ClearImg(mrk);

      //Printf("mser area:%d  regs:%d\n" ,minarea,root->Drawm(mrk,1e10,minarea));

      GetChar();

      ClearImg(mrk);

      if (minarea > 1)
        root->CutSmallAreas(minarea);

      root->Shorten();

      Printf("Reduce regs:%d\n" , root->Draw(mrk));

#if 0

      for (int idx = 0; idx < ss; idx++)
        {
          M[idx] = Node.Find(idx);
          //      if (nodes[M[idx]]->area > 100 )
          PutVal(mrk, indx(idx), indy(idx), M[idx] % 255 + 1);
        }

#endif
      GetChar();
    }

  return 1;
}
