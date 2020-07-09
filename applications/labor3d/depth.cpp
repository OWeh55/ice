#include <image.h>
#include <image3d.h>
#include <image3dfunc.h>
#include "display3d.h"
#include "depth.h"

void DepthImage(const Image3d<Image>& src, int level, Image& depth, Image& depthvalid, int depthfactor)
{
  double xsd, ysd, zsd;
  src.getSizeD(xsd, ysd, zsd);

  int xs = RoundInt(xsd);
  int ys = RoundInt(ysd);
  int zs = RoundInt(zsd);

  // tiefenbild

  depth = NewImg(xs, zs, ys * depthfactor);
  depthvalid = NewImg(xs, zs, 4);

  clearImg(depth);
  clearImg(depthvalid);

  for (int z = 0; z < zs; z++)
    for (int x = 0; x < xs; x++)
      {
        int y;
        bool found = false;
        for (y = 0; y < ys && !found; y++)
          found = src.getValue(x, y, z) < level;
        if (found)
          PutVal(depth, x, z, y * depthfactor);
        else
          PutVal(depthvalid, x, z, 3);
      }
}

bool newPoint(Image3d<Image>& img, IVector3d test)
{
  if (img.inside(test) && img.getPixel(test) == 0)
    {
      img.setPixel(test, 10); // mark as seen
      return true;
    }
  else
    return false;
}

bool objectPoint(const Image3d<Image>& img, IVector3d test)
{
  return img.inside(test) &&
         img.getPixel(test) < 20;
}

void DepthImage(const Image3d<Image>& src, int level, IVector3d current,
                Image& depth, Image& depthvalid, char dir, int depthfactor)
{
  double xsd, ysd, zsd;
  src.getSizeD(xsd, ysd, zsd);

  int xs = RoundInt(xsd);
  int ys = RoundInt(ysd);
  int zs = RoundInt(zsd);

  // binarisierung und Groessenanpassung!
  Image3d<Image> binary;
  binary.create(xs, ys, zs, 255);
  Display3d displayb(binary);
  displayb.Cursor(false);

  for (int z = 0; z < zs; z++)
    {
      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          {
            //              cout << x << " "<< y << " "<< z <<endl;
            //              cout << xs << " "<< ys << " "<< zs <<endl;
            int val = src.getValue(x, y, z);
            if (val >= level)
              binary.setPixel(x, y, z, 255);
            else
              binary.setPixel(x, y, z, 0);
          }
    }

  displayb.Update();

  switch (dir)
    {
    case 'x':
      depth = NewImg(zs, ys, xs * depthfactor);
      depth.set(xs * depthfactor);
      depthvalid = NewImg(zs, ys, 4);
      break;
    case 'y':
      depth = NewImg(xs, zs, ys * depthfactor);
      depth.set(ys * depthfactor);
      depthvalid = NewImg(xs, zs, 4);
      break;
    case 'z':
      depth = NewImg(xs, ys, zs * depthfactor);
      depth.set(zs * depthfactor);
      depthvalid = NewImg(xs, ys, 4);
      break;
    case 'X':
      depth = NewImg(zs, ys, xs * depthfactor);
      depth.set(xs * depthfactor);
      depthvalid = NewImg(zs, ys, 4);
      break;
    case 'Y':
      depth = NewImg(xs, zs, ys * depthfactor);
      depth.set(ys * depthfactor);
      depthvalid = NewImg(xs, zs, 4);
      break;
    case 'Z':
      depth = NewImg(xs, ys, zs * depthfactor);
      depth.set(zs * depthfactor);
      depthvalid = NewImg(xs, ys, 4);
      break;
    default:
      throw "depth: wrong depth mode";
    }

  depthvalid.set(1);

  Show(OVERLAY, depth, depthvalid);

  vector<IVector3d> pp;

  pp.push_back(current);

  int ct = 0;
  IVector3d test;
  while (!pp.empty())
    {
      test = pp.back();
      pp.pop_back();

      // cout << xt << " " << yt << " " << zt <<endl;
      // cout << "--" << xs << " " << ys << " " << zs <<endl;
      // cout << "--" << binary.sizeX() << " " << binary.sizeY() << " " << binary.sizeZ() <<endl;
      if (objectPoint(binary, test))
        {
          int tx = 0, ty = 0, tval = 0;
          switch (dir)
            {
            case 'x':
              tx = test.z;
              ty = ys - 1 - test.y;
              tval = test.x * depthfactor;
              break;
            case 'y':
              tx = test.x;
              ty = test.z;
              tval = test.y * depthfactor;
              break;
            case 'z':
              tx = test.x;
              ty = ys - 1 - test.y;
              tval = test.z * depthfactor;
              break;
            case 'X':
              tx = zs - 1 - test.z;
              ty = ys - 1 - test.y;
              tval = (xs - 1 - test.x) * depthfactor;
              break;
            case 'Y':
              tx = xs - 1 - test.x;
              ty = test.z;
              tval = (ys - 1 - test.y) * depthfactor;
              break;
            case 'Z':
              tx = xs - 1 - test.x;
              ty = ys - 1 - test.y;
              tval = (zs - 1 - test.z) * depthfactor;
              break;
            }
          // cout << xt << " " << yt << " " << test.z <<endl;
          // cout << tx << " " << ty << " " << tval <<endl;

          PutVal(depthvalid, tx, ty, 0);

          if (tval < GetVal(depth, tx, ty))
            PutVal(depth, tx, ty, tval);

          binary.setPixel(test, 127); // Punkt ist bearbeitet

          // Nachbarn vormerken
          test.z++;
          if (newPoint(binary, test))
            pp.push_back(test);
          test.z--;
          test.z--;
          if (newPoint(binary, test))
            pp.push_back(test);
          test.z++;

          test.y++;
          if (newPoint(binary, test))
            pp.push_back(test);
          test.y--;
          test.y--;
          if (newPoint(binary, test))
            pp.push_back(test);
          test.y++;

          test.x++;
          if (newPoint(binary, test))
            pp.push_back(test);
          test.x--;

          test.x--;
          if (newPoint(binary, test))
            pp.push_back(test);
          test.x++;
        }
      ct++;
      if (ct % 100000 == 0)
        displayb.Update(current);
    }

  displayb.Update(current);
}

void ObjImage(const Image3d<Image>& src, int level,
              IVector3d startpoint,
              Image3d<Image>& obj)
{
  int xs = src.xsize;
  int ys = src.ysize;
  int zs = src.zsize;

  obj.copy(src);
  binImg(obj, level);

  vector<IVector3d> pp;

  pp.push_back(startpoint);

  IVector3d test;
  while (!pp.empty())
    {
      test = pp.back();
      pp.pop_back();

      // cout << xt << " " << yt << " " << zt <<endl;
      // cout << "--" << xs << " " << ys << " " << zs <<endl;
      // cout << "--" << obj.sizeX() << " " << obj.sizeY() << " " << obj.sizeZ() <<endl;
      if (objectPoint(obj, test))
        {
          obj.setPixel(test, 127); // Punkt ist Objektpunkt und ist bearbeitet

          // Nachbarn vormerken
          test.z++;
          if (newPoint(obj, test))
            pp.push_back(test);
          test.z--;
          test.z--;
          if (newPoint(obj, test))
            pp.push_back(test);
          test.z++;

          test.y++;
          if (newPoint(obj, test))
            pp.push_back(test);
          test.y--;
          test.y--;
          if (newPoint(obj, test))
            pp.push_back(test);
          test.y++;

          test.x++;
          if (newPoint(obj, test))
            pp.push_back(test);
          test.x--;

          test.x--;
          if (newPoint(obj, test))
            pp.push_back(test);
          test.x++;
        }
    }

  for (int z = 0; z < zs; z++)
    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          int val = obj.getPixel(x, y, z);
          if (val == 127)
            obj.setPixel(x, y, z, 0);
          else
            obj.setPixel(x, y, z, 255);
        }
}
