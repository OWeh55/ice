#include <image.h>

const int sx = 1500;
const int maxval = 255;

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  Image img;
  bool wait = true;
  if (argc > 1)
    wait = false;
  
#if 0
  img.read(fn);
#else
  img.create(sx, sx, maxval);
  WindowWalker ww(img);
  for (ww.init(); !ww.ready(); ww.next())
    {
      double fix = M_PI * 2 * ww.x / sx;
      double fiy = M_PI * 2 * ww.y / sx;
      double sc = sin(fix) * sin(fiy);
      img.setPixelLimited(ww, 0.5 * maxval * (1 + sc));
    }
#endif
  
  Image reg;
  reg.create(img);
  Show(OVERLAY, img, reg);
  for (int i = 0; i < 2; ++i)
    {
      reg.set(0);
      bool mode = i > 0;
      double atime = TimeD();
      ComponentTreeNode* tree = makeComponentTree(img, mode);

      cout << TimeD() - atime << " s to create CT" << endl;

      cout << tree->getNComponents() << " Komponenten" << endl;
      cout << "Tiefe: " << tree->getDepth() << endl;
      cout << "Höhe: " << tree->getHeight() << endl;

      atime = TimeD();
      tree->reduceSmallLeaves(25);
      cout << TimeD() - atime << " s to reduce" << endl;

      cout << tree->getNComponents() << " Komponenten" << endl;
      cout << "Tiefe: " << tree->getDepth() << endl;
      cout << "Höhe: " << tree->getHeight() << endl;

      tree->draw(reg);
      if (wait)
        GetChar();

      reg.set(0);
      vector<Region> regions;
      vector<Region> bregions;
      //  cout << tree->getBigRegions(regions, 1) << " big regions" << endl;
      tree->getLeafs(regions, bregions);
      cout << bregions.size() << " Einzugsgebiete" << endl;
      for (unsigned int i = 0; i < bregions.size(); ++i)
        {
          bregions[i].draw(reg, 3);
        }
      cout << regions.size() << " Blätter" << endl;
      for (unsigned int i = 0; i < regions.size(); ++i)
        {
          regions[i].draw(reg, 2);
        }

      for (unsigned int i = 0; i < regions.size(); ++i)
        {
          Point p = regions[i].getCenter();
          Marker(1, p, 4, 7, reg);
        }
      //      tree->print(5);
      if (wait)
        GetChar();
    }
  return OK;
}
