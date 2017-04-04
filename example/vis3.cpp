/*
 *  vis3.cpp
 *  Testet User-Menüs
 *  testet bildspeicherverwaltung, visualisierung
 *  testet Alphanumerischen Bildschirm
 */

#include <string>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <image.h>
#include <visual/App.h>

using namespace std;
using namespace ice;

int maxcount = 127;

#define SFAK 1.01

// Startfenster
#define A0 -1.7
#define B0 -1.5
#define DA 3.0
#define DB 3.0

#ifndef MAXCOUNT
#define MAXCOUNT 1024
#endif

double a0, b0, da, db;

int sx, sy, maxval;

double Modifikator = 2.0;

typedef int iterfunc(double a, double b, int mc);

iterfunc iterapfel;
iterfunc iterapfel4;
iterfunc iterapfeli;

iterfunc* iter = &iterapfel;

int ColorMode;
int FileNr = 0;
string command;

void reset();
void resetgeo();
void fwert(int x, int y, double* fx, double* fy);
void frame_(int, int, int, int);
void itersel();
void depthsel();
void SelectColr();
void setcolor(int);
void SaveImage(Image ptr);
void PrintA(string);
void ParameterMessage(void);
int gc();

int Main(int argc, char* argv[])
{
  Image ptr;
  int x, y, z;
  int x1, y1, xe, ye;
  int xse = 0, yse = 0;   /* Bildschirmgroesse (Alpha) */
  int done;
  int valc;
  int klicks;

  int no_input;

  double a, b;

  double talt, tneu;
  double tp;

  OpenGraphic("Fraktal-Wunder");
  OpenAlpha("Fraktal-Wunder");
  valc = InfVis(TABLE_LEN);
  sx = InfVis(MAXX) - 2;
  sy = InfVis(MAXY) - 2;

  maxval = valc - 1;

  ptr = NewImg(sx, sy, maxval, "Fraktal");

  Visual v = Show(ON, ptr);
  v->AddMenuItem("File", "Reset\tALT-R", 1);
  v->AddMenuItem("File", "Quit\tALT-Q", 9);

  v->AddMenuItem("Fraktal", "Apfel\tALT-T", 41);
  v->AddMenuItem("Fraktal", "Apfel4\tALT-V", 42);
  v->AddMenuItem("Fraktal", "Apfel(Integer)\tALT-I", 43);

  v->AddMenuItem("Iterationstiefe", "128\tALT-1", 51);
  v->AddMenuItem("Iterationstiefe", "256\tALT-2", 52);
  v->AddMenuItem("Iterationstiefe", "512\tALT-3", 53);
  v->AddMenuItem("Iterationstiefe", "1024\tALT-4", 54);
  v->AddMenuItem("Iterationstiefe", "2048\tALT-5", 55);
  v->AddMenuItem("Iterationstiefe", "4096\tALT-6", 56);

  Display(ON);

  Alpha(ON);

  xse = AlphaSizeX() - 1;
  yse = AlphaSizeY() - 1;
  SetAttribute(C_YELLOW, C_RED, 0, 0);
  ClearAlpha();

  frame_(1, 1, xse - 1, yse - 1);

  /* Kein alphanumnerisches Menu mehr

     frame_(38, 13, xse - 3, yse - 2);

     SetAttribute(-1, -1, -1, 1);

     SetAlphaCursor(40, 13);
     Printf("Tasten-Funktionen:");

     SetAttribute(-1, -1, -1, 0);

     SetAlphaCursor(40, 14);
     PrintA("~Typ des Fraktals");
     SetAlphaCursor(40, 15);
     PrintA("~Iterationstiefe");
     SetAlphaCursor(40, 16);
     PrintA("~Farbtabellen");
     SetAlphaCursor(40, 17);
     PrintA("~Speichern");
     SetAlphaCursor(40, 18);
     PrintA("~Grösserer Ausschnitt");
     SetAlphaCursor(40, 19);
     PrintA("~Angleichen Ausdehnungen x/y");
     SetAlphaCursor(40, 20);
     PrintA("~Rücksetzen der Geometrie");
     SetAlphaCursor(40, 21);
     PrintA("~Quittieren");
  */
  reset();
  done = FALSE;

  while (!done)
    {
      SetAttribute(-1, -1, -1, 1);
      SetAlphaCursor(15, 5);
      Printf("Berechnung läuft.          ");
      SetAttribute(-1, -1, -1, 0);
      SetAlphaCursor(15, 8);
      Printf("X: %f ... %f     ", a0, a0 + da);
      SetAlphaCursor(15, 9);
      Printf("Y: %f ... %f     ", b0, b0 + db);
      SetAlphaCursor(15, 10);
      Printf("Max: %6d", maxcount);

      talt = TimeD(TM_PROCESS);
      //    talt=TimeD(TM_WORLD);
      //    talt=((double)clock())/CLOCKS_PER_SEC;
      for (int y = 0; y < ptr.ysize; y++)
        for (int x = 0; x < ptr.xsize; x++)
          {
            fwert(x, y, &a, &b);
            z = (*iter)(a, b, maxcount);
            PutValUnchecked(ptr, x, y, z % valc);
          }
      tneu = TimeD(TM_PROCESS);
      //    tneu=TimeD(TM_WORLD);
      //    tneu=((double)clock())/CLOCKS_PER_SEC;

      SetAlphaCursor(3, yse - 2);
      SetAlphaCursor(15, 5);
      Printf("Berechnung beendet.                  ");
      SetAlphaCursor(3, yse - 4);

      tp = (tneu - talt) * 1000000.0 / sx / sy;

      Printf("%g us/Pixel\n", tp);

      if ((a0 == A0) && (b0 == B0) && (da == DA) && (db == DB)
          && (maxcount == MAXCOUNT))
        {
          SetAlphaCursor(3, yse - 2);
          Printf("Leistungsindex: %3.2f", 50.0 / tp);
        }

      klicks = 1;

      x1 = sx;
      y1 = sy;
      xe = 0;
      ye = 0;

      no_input = TRUE;

      long ev = wxGetApp().WaitForEvent();

      switch (ev)
        {
        case 1:
          reset();
          break;
        case 9:
          done = true;
          break;
        case 41:
          iter = &iterapfel;
          break;
        case 42:
          iter = &iterapfel4;
          break;
        case 43:
          iter = &iterapfeli;
          break;
        case 51:
          maxcount = 128;
          break;
        case 52:
          maxcount = 256;
          break;
        case 53:
          maxcount = 512;
          break;
        case 54:
          maxcount = 1024;
          break;
        case 55:
          maxcount = 2048;
          break;
        case 56:
          maxcount = 4096;
          break;
        }

    }

  CloseAlpha();

  printf("\n");
  return OK;
}

void fwert(int x, int y, double* fx, double* fy)
{
  *fx = a0 + da * ((double) x / (double) sx);
  *fy = b0 + db * ((double) y / (double) sy);
}

int iterapfel(double ap, double bp, int maxcount)
{
  int z;
  double fd;
  double fx, fy;
  double x2, y2;
  double a, b;
  a = ap - 0.5;
  b = bp;
  fx = a;
  fy = b;
  z = 0;
  fd = 0;

  while ((z < maxcount) && (fd < 16))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * Modifikator + b;
      fx = x2 - y2 + a;
      fd = x2 + y2;
      z++;
    }

  return z;
}

int iterapfeli(double ap, double bp, int maxcount)
{
  int z;
  int fd;
  int fx, fy;
  int x2, y2;
  int a, b;
  int modi = int (Modifikator * 1024);
  a = int ((-ap - 0.5) * 1024);
  b = int (bp * 1024);
  fx = a;
  fy = b;
  z = 0;
  fd = 0;

  while ((z < maxcount) && (fd < 16 * 1024))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy / 1024 * modi / 1024 + b;
      fx = (x2 - y2) / 1024 + a;
      fd = (x2 + y2) / 1024;
      z++;
    }

  return z;
}

int iterapfel4(double ap, double bp, int maxcount)
{
  int z;
  double a, b;
  double fd;
  double fx, fy;
  double x2, y2;
  a = ap;
  b = bp;
  fx = a;
  fy = b;
  z = 0;
  fd = 0;

  while ((z < maxcount) && (fd < 40))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * Modifikator;
      fx = x2 - y2;
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * Modifikator + b;
      fx = x2 - y2 + a;
      fd = x2 + y2;
      z++;
    }

  return z;
}

void frame_(int x1, int y1, int x2, int y2)
{
  int x, y;

  for (x = x1; x <= x2; x++)
    {
      SetAlphaCursor(x, y1);
      PutChar(R_OU);
      SetAlphaCursor(x, y2);
      PutChar(R_OU);
    }

  for (y = y1; y <= y2; y++)
    {
      SetAlphaCursor(x1, y);
      PutChar(R_LR);
      SetAlphaCursor(x2, y);
      PutChar(R_LR);
    }

  SetAlphaCursor(x2, y2);
  PutChar(R_RU);
  SetAlphaCursor(x1, y2);
  PutChar(R_LU);
  SetAlphaCursor(x2, y1);
  PutChar(R_RO);
  SetAlphaCursor(x1, y1);
  PutChar(R_LO);
}

void itersel()
{
  vector<string> men;
  int select;
  men.push_back("~Apfel");
  men.push_back("Apfel~4");
  men.push_back("Apfel-~Integer");
  PushAlpha();

  if (command != "")
    {
      select = gc() - '0';
    }
  else
    {
      select = Menu(men, 5, 5, 24, 18);
    }

  if (select >= 0)
    {
      switch (select)
        {
        case 0:
          iter = &iterapfel;
          break;
        case 1:
          iter = &iterapfel4;
          break;
        case 2:
          iter = &iterapfeli;
          break;
        }
    }

  PopAlpha();
}

void SelectColr()
{
  vector<string> men;
  int select;
  men.push_back("Grau");
  men.push_back("Blau-Gelb");
  men.push_back("Schwarz/Weiß");
  PushAlpha();
  select = Menu(men, 5, 5, 24, 18);

  if (select >= 0)
    setcolor(select);

  PopAlpha();
}

void depthsel()
{
  vector<string> men;
  int select;
  char s[40];
  int i;

  for (i = 6; i < 15; i++)
    {
      sprintf(s, "%d", 1 << i);
      men.push_back(s);
    }

  PushAlpha();
  select = Menu(men, 7, 3, 24, 18);

  if (select >= 0)
    {
      maxcount = 1 << (select + 6);
    }

  PopAlpha();
}

void resetgeo()
{
  a0 = A0;
  da = DA;
  b0 = B0;
  db = DB;
}

void reset()
{
  maxcount = MAXCOUNT;
  iter = &iterapfel;
  resetgeo();
  Modifikator = 2.0;
  setcolor(1);
}

void setcolor(int select)
{
  int z, i1, i2, i3;
  double fx;
  ColorMode = select;

  switch (select)
    {
    case 0:

      for (z = 0; z <= maxval; z++)
        {
          i1 = i2 = i3 = (maxval - z) * 255 / maxval;

          if ((z & 1) != 0)
            SetGreyColor(z, i1, i2, i3);
          else
            SetGreyColor(z, i1 * 90 / 100, i2 * 90 / 100, i3 * 90 / 100);
        }

      break;

    case 1:

      for (z = 0; z <= maxval; z++)
        {
          fx = z * 3.14159265 * 2 / (maxval + 1);
          i1 = (int)((sin(fx) + 1) * 255 / 2);
          i2 = (int)((cos(fx) + 1) * 255 / 2);

          if ((z & 1) != 0)
            SetGreyColor(z, i1, i1, i2);
          else
            SetGreyColor(z, i1 * 90 / 100, i1 * 90 / 100, i2 * 90 / 100);
        }

      break;

    case 2:

      for (z = 0; z <= maxval; z++)
        {
          if ((z & 1) == 0)
            SetGreyColor(z, 255, 255, 255);
          else
            SetGreyColor(z, 0, 0, 0);
        }

      break;
    }
}

void SaveImage(Image ptr)
{
  char fn[80];
  int x, y;

  sprintf(fn, "frac%04d.jpg", FileNr++);

  PushAlpha();
  SetAttribute(-1, -1, -1, 1);
  SetAlphaCursor(15, 5);
  Printf("Bild wird gespeichert. ");
  SetAttribute(-1, -1, -1, 0);

  ColorImage rgb;
  rgb.create(ptr->xsize, ptr->ysize, 255);

  for (int y = 0; y < ptr.ysize; y++)
    for (int x = 0; x < ptr.xsize; x++)
      {
        int gw = GetVal(ptr, x, y);
        unsigned char r, g, b;
        GetGreyColor(ptr, gw, r, g, b);
        PutVal(rgb.redImage(), x, y, r ^ 255);
        PutVal(rgb.greenImage(), x, y, g ^ 255);
        PutVal(rgb.blueImage(), x, y, b ^ 255);
      }
  rgb.write(fn);
  PopAlpha();
}

int gc()
{
  int c;

  if (command == "")
    c = GetKey();
  else
    {
      c = command[0];
      command = command.substr(1);
    }

//  if (c!=0)  cout << c << " " << (char)c << endl;
  return c;
}

void ParameterMessage()
{
  printf("\nvis2 (c)OWeh\n\n");
  printf("vis [-b] commandstring\n");
}

void PrintA(string s)
/* Ausschriften mit Attribut */
{
  bool high;
  unsigned int ct;
  high = false;
  ct = 0;

  while (ct < s.length())
    {
      if (s[ct] == '~')
        {
          SetAttribute(-1, -1, -1, 1);
          high = true;
        }
      else
        {
          PutChar(s[ct]);

          if (high)
            SetAttribute(-1, -1, -1, 0);

          high = false;
        }

      ct++;
    }

  if (high)
    SetAttribute(-1, -1, -1, 0);
}
