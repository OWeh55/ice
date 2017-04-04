#include <image.h>

int main(int argc, char* argv[])
{
  Image ir, ig, ib, id;
  int xs, ys, mv;
  string filename = argv[1];
  OpenAlpha("rgb2h");
  string::size_type pos = filename.rfind("."); // letzten Punkt im Dateinamen suchen

  if (pos == string::npos)
    {
      Print("Dateiname falsch (" + filename + ")");
      GetChar();
      return 1;
    }

  string basename = filename.substr(0, pos);
  string sext = filename.substr(pos + 1, 9999);
  Print(basename + " + " + sext + "\n");

  if (sext == "bmp")
    {
      InfImgFile(filename, xs, ys, mv);
      ir = NewImg(xs, ys, mv);
      ig = NewImg(ir);
      ib = NewImg(ig);
      id = NewImg(xs, ys, 256 * 256 - 1);
      Show(_RGB, ir, ig, ib);
      Show(ON, id);
      ReadImg(filename, ir, ig, ib);
  for (int y=0;y<id.ysize;y++)
    for (int x=0;x<id.xsize;x++)
      {
        PutVal(id, x, y, GetVal(ig, x, y) * 256 + GetVal(ir, x, y));
      }
      WriteImg(ir, ig, ib, basename + "_r.psd");
      WriteImg(id, basename + ".psd");
      GetChar();
      ClearImg(id);
      ClearImg(ir);
      ClearImg(ig);
      ClearImg(ib);
      GetChar();
      ReadImg(basename + ".psd", id);
      ReadImg(basename + "_r.psd", ir, ig, ib);
    }
  else
    {
//      int xm,ym,mv,nr;
//       InfImgFile(filename,xm,ym,mv,nr);
//       cout << xm << "*" << ym << " " << mv << " " << nr << endl;
      id = ReadImg(filename);
      Show(ON, id);
      ir = NewImg(id->xsize, id->ysize, 255);
      ig = NewImg(id->xsize, id->ysize, 255);
      ib = NewImg(id->xsize, id->ysize, 255);

  for (int y=0;y<id.ysize;y++)
    for (int x=0;x<id.xsize;x++)
  {
        int v = GetVal(id, x, y);
        PutVal(ir, x, y, v % 256);
        PutVal(ig, x, y, v / 256);
        PutVal(ib, x, y, 255);
      }
      WriteImg(ir, ig, ib, basename + ".bmp");
    }

  GetChar();
  return 0;
}
