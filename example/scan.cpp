/**************************************************************/
/* Test Bildabtastung                                         */
/**************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <image.h>

#include <dc1394/control.h>

#ifndef WIN32
#include <getopt.h>
#endif

#define IMAGENR 20

int achannel = 0;
int snumber = 10;

void GrayScan(int xs, int ys, int maxval)
{
  Image p1;
  int i;
  double timea;
  p1 = NewImg(xs, ys, maxval);

  Display(ON);
  Show(ON, p1);
  Printf("Bild einstellen und <ENTER> geben, wenn zum Abtasten bereit\n");

  ScanImg(achannel, p1, TRUE);

  Printf("Zeitmessung\n");

  timea = TimeD();

  for (i = 0; i < snumber; i++)
    ScanImg(achannel, p1, FALSE);

  Printf("Benötigt: %7.2f\n", (TimeD() - timea) / i * 1000);

  do
    {
      Printf("<ENTER> to continue\n");
    }
  while (GetChar() != 13);

  Display(OFF);
  Show(OFF, p1);
  FreeImg(p1);
}

void RGBScan(int xs, int ys, int maxval)
{
  Image p1, p2, p3;
  int i;
  double timea;

  p1 = NewImg(xs, ys, maxval);
  p2 = NewImg(xs, ys, maxval);
  p3 = NewImg(xs, ys, maxval);
  Display(ON);
  Show(_RGB, p1, p2, p3);

  Printf("Bild einstellen und <ENTER> geben, wenn zum Abtasten bereit\n");
  ScanImg(achannel, p1, p2, p3, TRUE);

  Printf("Zeitmessung\n");

  timea = TimeD();

  for (i = 0; i < snumber; i++)
    ScanImg(achannel, p1, p2, p3, FALSE);

  Printf("Benötigt: %7.2f\n", (TimeD() - timea) / i * 1000);

  do
    {
      Printf("<ENTER> to continue\n");
    }
  while (GetChar() != 13);

  Show(OFF, p1);
  FreeImg(p1);
  FreeImg(p2);
  FreeImg(p3);
  Display(OFF);
}

void Parameter()
{
  int ready = false;
  int i;
  int ct;
  int c;
  int current = 0;
  int cfeature = 0;
  int val;
  PushAlpha();
  ClearAlpha();

  while (!ready)
    {
      SetAlphaCursor(0, 0);

      for (i = DC1394_FEATURE_MIN, ct = 0; i <= DC1394_FEATURE_MAX; i++)
        {
          int sc;
          string desc;

          if (ScanGetExtData(achannel, i, val, sc, desc) == OK)
            {
              int aval;
              ScanGetExtData(achannel, i | FEATURE_CURRENT, aval);
              int man;
              ScanGetExtData(achannel, i | FEATURE_CANSET, man);
              int autom;
              ScanGetExtData(achannel, i | FEATURE_HASAUTO, autom);
              int min;
              ScanGetExtData(achannel, i | FEATURE_GETMIN, min);
              int max;
              ScanGetExtData(achannel, i | FEATURE_GETMAX, max);

              while (desc.length() < 16) desc += " ";

              if (ct == current)
                {
                  SetAttribute(C_RED, -1, 0, 1);
                  cfeature = i;
                }
              else SetAttribute(C_BLACK, -1, 0, 1);

              Print(desc + " " + NumberString(aval, 4) + " ");

              if (man) Print("M");
              else Print(" ");

              if (autom)
                {
                  int isauto;
                  ScanGetExtData(achannel, i | FEATURE_ISAUTO, isauto);

                  if (isauto) Print("A");
                  else Print("a");
                }
              else Print(" ");

              Print(" ");
              Print(NumberString(min, 4) + ".." + NumberString(max, 4));
              Print("\n");
              ct++;
            }
        }

      c = GetChar();

      switch (c)
        {
        case '+':
          current++;

          if (current >= ct) current = 0;

          break;
        case '-':
          current--;

          if (current < 0) current = ct - 1;

          break;
        case 'a':
          ScanSetExtData(achannel, cfeature | FEATURE_AUTO, 0);
          break;
        case 'A':
          ScanSetExtData(achannel, cfeature | FEATURE_AUTO, 1);
          break;
        case 'i':
          val = Input("Wert: ");
          ScanSetExtData(achannel, cfeature | FEATURE_CURRENT, val);
          ClearAlpha();
          break;
        case 'x':
          ready = true;
          break;
        }
    }

  PopAlpha();
}

int usage(const string& pn)
{
  cout << pn << endl;
  cout << "Usage: " << endl;
  cout << "  " << pn << "<options>" << endl << endl;
  cout << "Options:" << endl;
  cout << "  -c <n>     Use scan Channel <n>" << endl;
  cout << "  -f <fn>    Scan to File <fn> - non-interactive mode" << endl;
  cout << "  -g         Grayvalue scan (default: color)" << endl;
  cout << "  -z <n>     Set Zoom to <n>" << endl;
  cout << "  -F <n>     Set Focus to <n>" << endl;
  cout << "  -i <n>     Set Iris to <n>" << endl;
  cout << "  -w         Wait for keypress before scanning" << endl;
  cout << "  -m <n>     Average <n> scans (default:1)" << endl;
  exit(1);
}

int main(int argc, char* argv[])
{
  int interactive = true;
  int i;
  int channels;
  int xs, ys;
  string desc;
  int flags;
  int maxval;

  int iris = -1;
  int zoom = -1;
  int focus = -1;

  bool wait = false;
  bool gray = false;

  string filename;
  int mittel = 1; // Zahl zu mittelnder Bilder

#ifndef WIN32
  int oret;

  while ((oret = getopt(argc, argv, "c:s:f:z:F:i:wm:gh")) >= 0)
    {
      switch (oret)
        {
        case 's':
          snumber = atol(optarg);
          break;
        case 'c':
          achannel = atol(optarg);
          break;
        case 'f':
          filename = optarg;
          interactive = false;
          break;
        case 'g':
          gray = true;
          break;
        case 'z':
          zoom = atol(optarg);
          break;
        case 'i':
          iris = atol(optarg);
          break;
        case 'F':
          focus = atol(optarg);
          break;
        case 'w':
          wait = true;
          break;
        case 'm':
          mittel = atol(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

#endif

  if (interactive)
    {
      Alpha(ON);
      ClearAlpha();

      Printf("gefundene Treiber:\n");
      channels = 1;
      i = 0;

      while (i < channels)
        {
          ScanInfo(i, xs, ys, maxval, channels, flags, desc);
          Printf("%d/%d: %s\n Bildgroesse %dx%d maximaler Wert:%d\n",
                 i, channels, desc.c_str(), xs, ys, maxval);
          i++;
        }

      Printf("Benutze Kanal %d\n", achannel);
      ScanInfo(achannel, xs, ys, maxval, channels, flags, desc);

      if (xs <= 0) xs = 767;

      if (ys <= 0) ys = 512;

      vector<string> men;
      men.push_back("~Gray");
      men.push_back("~Color");
      men.push_back("~Parameter");
      men.push_back("E~xit");
      i = 0;

      while (i != 3)
        {
          i = Menu(men, 55, 5, 79, 18, true);

          switch (i)
            {
            case 0:
              GrayScan(xs, ys, 255);
              break;
            case 1:
              RGBScan(xs, ys, 255);
              break;
            case 2:
              Parameter();
              break;
            }
        }
    }
  else // no interactive
    {
      if (iris >= 0)
        {
          cout << "Setze Iris (auto aus)" << endl;
          ScanSetExtData(achannel, DC1394_FEATURE_IRIS | FEATURE_AUTO, 0);
          usleep(200000);
          cout << "Setze Iris: " << iris << endl;
          ScanSetExtData(achannel, DC1394_FEATURE_IRIS, iris);
          Delay(2);
        }

      if (zoom >= 0)
        {
          cout << "Setze Zoom:" << zoom << endl;
          ScanSetExtData(achannel, DC1394_FEATURE_ZOOM, zoom);
          Delay(2);
        }

      if (focus >= 0)
        {
          cout << "Setze Fokus:" << focus << endl;
          ScanSetExtData(achannel, DC1394_FEATURE_FOCUS, focus);
          Delay(2);
        }

      ScanInfo(achannel, xs, ys, maxval, channels, flags, desc);

      if (gray)
        {
          // grayvalue only
          Image  p1 = NewImg(xs, ys, 255);
          ImageD p1d = NewImgD(xs, ys);

          ClearImgD(p1d);

          if (wait)
            {
              Alpha(ON);
              ClearAlpha();
              Display(ON);
              Show(ON, p1);
              Print("Press Enter to scan\n");
              ScanImg(achannel, p1, TRUE);
              Print("Scanning..\n");
              Show(OFF, p1);
            }

          ScanImg(achannel, p1, FALSE);

          WindowWalker ww(p1);

          for (i = 0; i < mittel; i++)
            {
              ScanImg(achannel, p1, FALSE);

              for (ww.init(); !ww.ready(); ww.next())
                {
                  PutValD(p1d, ww, GetValD(p1d, ww) + GetVal(p1, ww));
                }
            }

          for (ww.init(); !ww.ready(); ww.next())
            {
              PutVal(p1, ww, RoundInt(GetValD(p1d, ww) / mittel));
            }

          WriteImg(p1, filename);
        }
      else
        {
          // true color image

          Image p1 = NewImg(xs, ys, 255);
          Image p2 = NewImg(xs, ys, 255);
          Image p3 = NewImg(xs, ys, 255);

          ImageD p1d = NewImgD(xs, ys);
          ClearImgD(p1d);
          ImageD p2d = NewImgD(xs, ys);
          ClearImgD(p2d);
          ImageD p3d = NewImgD(xs, ys);
          ClearImgD(p3d);

          if (wait)
            {
              Alpha(ON);
              ClearAlpha();
              Display(ON);
              Show(_RGB, p1, p2, p3);
              Print("Press Enter to scan\n");
              ScanImg(achannel, p1, p2, p3, TRUE);
              Show(OFF, p1);
              Print("Scanning..\n");
            }

          ScanImg(achannel, p1, p2, p3, FALSE);
          WindowWalker ww(p1);

          for (i = 0; i < mittel; i++)
            {
              ScanImg(achannel, p1, p2, p3, FALSE);
              for (ww.init(); !ww.ready(); ww.next())
                {
                  PutValD(p1d, ww, GetValD(p1d, ww) + GetVal(p1, ww));
                  PutValD(p2d, ww, GetValD(p2d, ww) + GetVal(p2, ww));
                  PutValD(p3d, ww, GetValD(p3d, ww) + GetVal(p3, ww));
                }
            }


          for (ww.init(); !ww.ready(); ww.next())
            {
              PutVal(p1, ww, RoundInt(GetValD(p1d, ww) / mittel));
              PutVal(p2, ww, RoundInt(GetValD(p2d, ww) / mittel));
              PutVal(p3, ww, RoundInt(GetValD(p3d, ww) / mittel));
            }

          WriteImg(p1, p2, p3, filename);
        }
    }

  return OK;
}
