#include <image.h>
#include <fstream>


const int m_free = 0;
const int m_block = 250;
const int m_marker = 255;

bool realInside(const Image &lab, int x, int y)
{
  return x > 0 && y > 0 && x < lab->xsize - 1 && y < lab->ysize - 1;
}

int Neighbours4(const Image &lab, int x, int y, int val)
{
  int ct = 0;
  if (GetVal(lab, x - 1, y) == val) ct++;
  if (GetVal(lab, x + 1, y) == val) ct++;
  if (GetVal(lab, x, y - 1) == val) ct++;
  if (GetVal(lab, x, y + 1) == val) ct++;
  return ct;
}

int Neighbours8(const Image &lab, int x, int y, int val)
{
  int ct = Neighbours4(lab, x, y, val);
  if (GetVal(lab, x - 1, y - 1) == val) ct++;
  if (GetVal(lab, x + 1, y - 1) == val) ct++;
  if (GetVal(lab, x - 1, y + 1) == val) ct++;
  if (GetVal(lab, x + 1, y + 1) == val) ct++;
  return ct;
}

bool oneNeighbour(const Image &lab, int x, int y, int &dir)
{
  int ct = 0;
  dir = 0;

  if (GetVal(lab, x - 1, y) == m_marker)
    {
      ct++;
      dir = 4;
    }

  if (GetVal(lab, x + 1, y) == m_marker)
    {
      ct++;
      dir = 0;
    }

  if (ct < 2)
    {
      if (GetVal(lab, x, y - 1) == m_marker)
        {
          ct++;
          dir = 6;
        }

      if (ct < 2)
        if (GetVal(lab, x, y + 1) == m_marker)
          {
            ct++;
            dir = 2;
          }
    }
  return ct == 1;
}

bool canSet(const Image &lab, int x, int y)
{
  int dir;
  bool canset = oneNeighbour(lab, x, y, dir);
  if (canset)
    canset = Neighbours8(lab, x, y, m_block) == 0;
  if (canset)
    {
      switch (dir)
        {
        case 0:
          if (GetVal(lab, x - 1, y - 1) > 0 || GetVal(lab, x - 1, y + 1) > 0)
            canset = false;
          break;

        case 2:
          if (GetVal(lab, x - 1, y - 1) > 0 || GetVal(lab, x + 1, y - 1) > 0)
            canset = false;
          break;

        case 4:
          if (GetVal(lab, x + 1, y - 1) > 0 || GetVal(lab, x + 1, y + 1) > 0)
            canset = false;
          break;

        case 6:
          if (GetVal(lab, x - 1, y + 1) > 0 || GetVal(lab, x + 1, y + 1) > 0)
            canset = false;
          break;

        }
    }
  return canset;
}

void setMarkerA(Image &lab, int x, int y, int val)
{
  if (lab.inside(x - 1, y - 1) && lab.inside(x + 1, y + 1))
    {
      PutVal(lab, x - 1, y - 1, val);
      PutVal(lab, x  , y - 1, val);
      PutVal(lab, x + 1, y - 1, val);
      PutVal(lab, x - 1, y, val);
      PutVal(lab, x + 1, y, val);
      PutVal(lab, x - 1, y + 1, val);
      PutVal(lab, x  , y + 1, val);
      PutVal(lab, x + 1, y + 1, val);
    }
}

void setMarkerB(Image &lab, int x, int y, int val)
{
  if (lab.inside(x - 1, y - 1) && lab.inside(x, y))
    {
      PutVal(lab, x - 1, y - 1, val);
      PutVal(lab, x  , y - 1, val);
      PutVal(lab, x - 1, y, val);
      PutVal(lab, x , y, val);
    }
}

Polygon getPoly(const Contur &c)
{
  Polygon poly;
  LimitingPolygon(c, poly);
  Polygon rpoly = poly.ReducedToPrecision(0.5, 2);
  return rpoly;
}

void WritePoly(ofstream &cf, const Polygon &rpoly, char id)
{
  cf << "const int corners" << id << "=" << rpoly.size() << ";" << endl;
  cf << "double pattern_x_" << id << "[corners" << id << "]={" << endl << rpoly[0].x;
  for (int i = 1; i < rpoly.size(); i++)
    cf << "," << rpoly[i].x;
  cf << "};" << endl;
  cf << "double pattern_y_" << id << "[corners" << id << "]={" << endl << rpoly[0].y;
  for (int i = 1; i < rpoly.size(); i++)
    cf << "," << rpoly[i].y;
  cf << "};" << endl;
}

int main(int argc, char **argv)
{
  int xs = 90;
  int ys = 64;
  int markers = 0;
  int minmarkers = 5;
  int mtyp = 0;
  int mode = 1;
  int objects = 1;
  int factor = 10;
  int rc;

  Alpha(ON);

  while ((rc = getopt(argc, argv, "hx:y:m:K:f:k:")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          xs = atoi(optarg);
          break;
        case 'y':
          ys = atoi(optarg);
          break;
        case 'm':
          mode = atoi(optarg);
          break;
        case 'f':
          factor = atoi(optarg);
          break;
        case 'K':
          mtyp = 1;
          minmarkers = 6; // no break !!
        case 'k':
          markers = atoi(optarg);
          break;
        }
    }

  if (markers > 0 && markers < minmarkers)
    markers = minmarkers;

  Alpha(ON);
  Image lab;
  lab.create(xs, ys, 255);
  Show(ON, lab);
  SetImg(lab, m_free);

  if (markers > 0)
    {
      for (int y = 0; y < lab->ysize - 5; y += markers)
        for (int x = 0; x < lab->xsize - 5; x += markers)
          {
            if (mtyp == 0)
              setMarkerB(lab, x + 3, y + 3, m_block);
            else
              setMarkerA(lab, x + 3, y + 3, m_block);
          }
    }

  switch (mode)
    {
    case 1:
      if (markers == 0)
        PutVal(lab, xs / 2, ys / 2, m_marker);
      else
        {
          int x = xs / markers / 2 * markers;
          int y = ys / markers / 2 * markers;
          PutVal(lab, x, y, m_marker);
        }
      objects = 1;
      break;

    case 2:
      PutVal(lab, xs / markers / 4 * markers, ys / markers / 2 * markers, m_marker);
      PutVal(lab, xs / markers * 3 / 4 * markers, ys / markers / 2 * markers, m_marker);
      objects = 2;
      break;
    default:
      cerr << "Falscher Modus (-m)" << endl;
      exit(1);
    }
  bool newset = false;
  do
    {
      newset = false;
      for (int i = 0; i < 1000000; i++)
        {
          int x = Random(xs - 3) + 1;
          int y = Random(ys - 3) + 1;
          if (GetVal(lab, x, y) == m_free &&
              canSet(lab, x, y))
            {
              PutVal(lab, x, y, m_marker);
              newset = true;
            }
        }
    }
  while (newset);

  Image big;
  big.create(xs * factor, ys * factor, 255);
  Image mark;
  mark.create(xs * factor, ys * factor, 255);
  Show(OVERLAY, big, mark);

  for (int y = 0; y < big->ysize; y++)
    {
      int yo = y / factor;
      for (int x = 0; x < big->xsize; x++)
        {
          int xo = x / factor;
          PutVal(big, x, y, GetVal(lab, xo, yo));
        }
    }

  ClearImg(mark);
  IPoint start(0, 0);

  vector<Contur> c(objects);
  vector<Polygon> p(objects);
  for (int i = 0; i < objects; i++)
    {
      if (SearchStart(big, mark, NULL, 251, 1, start) == OK)
        c[i] = CalcContur(big, mark, NULL, 251, start);

      MarkContur(c[i], 1 + i, mark);
      p[i] = getPoly(c[i]);
      draw(p[i], mark, 1 + i);
    }

  GetChar();
  ClearImg(mark);
  GetChar();

  WriteImg(big, "labyrinth.jpg");

  ofstream cf("labyrinth.inc");
  for (int i = 0; i < objects; i++)
    {
      WritePoly(cf, p[i], '1' + i);
    }
  return OK;
}
