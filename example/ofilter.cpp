#include <image.h>

// testprogramm für Parameter für Orientierte Filter


int main(int argc, char** argv)
{
  string filename = "test_rgb.jpg";
  if (argc > 1)
    filename = argv[1];
  int xs, ys, mv, ch;
  InfImgFile(filename, xs, ys, mv, ch);

  ColorImage original;
  original.create(xs, ys, mv);
  original.read(filename);
  Show(ON, original);

  Image alpha;
  alpha.create(xs, ys, 359);

  ImageD lambda1 = NewImgD(xs, ys);
  ImageD lambda2 = NewImgD(xs, ys);
  ImageD lambdaRatio = NewImgD(xs, ys);
  ImageD lambdaValue = NewImgD(xs, ys);

  ColorImage filtered;
  filtered.create(xs, ys, mv);
  ColorImage combined;
  combined.create(xs, ys, mv);

  Show(ON, combined);

  Image regions;
  regions.create(xs, ys, 4);
  Show(OVERLAY, regions);

  bool ready = false;
  int ssize = 11; // Größe für Richtungsbestimmung
  bool needs_analyse = true;

  int fsize = 11;
  int fwidth = 2;
  bool needs_filtering = true;

  double ratioThreshold = 2.0;
  double valueThreshold = 0.1;
  bool needs_synthese = true;

  char mode = 'f';

  while (!ready)
    {
      if (needs_analyse)
        {
          CalcDirectionStructImg(original.greenImage(), alpha, ssize, lambda1, lambda2);
          for (int y = 0; y < ys; ++y)
            for (int x = 0; x < xs; ++x)
              {
                double l1 = GetValD(lambda1, x, y);
                double l2 = GetValD(lambda2, x, y);
                PutValD(lambdaRatio, x, y, l2 != 0 ? l1 / l2 : 0);
                PutValD(lambdaValue, x, y, fabs(l1) + fabs(l2));
              }
          needs_analyse = false;
          needs_filtering = true; // force new filtering
        }

      if (needs_filtering)
        {
          OrientedSmearImg(original, alpha, filtered, fsize, fsize, fwidth);
          needs_filtering = false;
          needs_synthese = true;
        }

      if (needs_synthese)
        {
          for (int y = 0; y < ys; ++y)
            for (int x = 0; x < xs; ++x)
              {
                ColorValue vo = original.getPixel(x, y);
                ColorValue vf = filtered.getPixel(x, y);
                if (GetValD(lambdaValue, x, y) > valueThreshold)
                  {
                    if (GetValD(lambdaRatio, x, y) > ratioThreshold)
                      {
                        combined.setPixel(x, y, vf);
                        regions.setPixel(x, y, 2);
                      }
                    else
                      {
                        combined.setPixel(x, y, vo);
                        regions.setPixel(x, y, 1);
                      }
                  }
                else
                  {
                    combined.setPixel(x, y, vo);
                    regions.setPixel(x, y, 3);
                  }
              }
        }
      ClearAlpha();
      Printf("\nOriented Filtering\n");
      if (mode == 's')
        Printf("* ");
      Printf("Strukturgröße s = %d\n", ssize);

      if (mode == 'f')
        Printf("* ");
      Printf("Filtergröße f = %d\n", fsize);
      if (mode == 'b')
        Printf("* ");
      Printf("Filterbreite b = %d\n", fwidth);

      if (mode == 'r')
        Printf("* ");
      Printf("Schwelle Verhältnis r = %f\n", ratioThreshold);
      if (mode == 'v')
        Printf("* ");
      Printf("Schwelle Betrag v = %f\n", valueThreshold);

      int select = GetChar();
      Printf("%c", select);
      switch (select)
        {
        case 's':
          mode = 's';
          break;
        case 'f':
          mode = 'f';
          break;
        case 'b':
          mode = 'b';
          break;
        case 'r':
          mode = 'r';
          break;
        case 'v':
          mode = 'v';
          break;
        case '+':
        {
          switch (mode)
            {
            case 's':
              ssize += 2;
              needs_analyse = true;
              break;
            case 'f':
              fsize += 2;
              needs_filtering = true;
              break;
            case 'b':
              fwidth += 1;
              needs_filtering = true;
              break;
            case 'r':
              ratioThreshold *= 1.1;
              needs_synthese = true;
              break;
            case 'v':
              valueThreshold *= 1.5;
              needs_synthese = true;
              break;
            }
        }
        break;
        case '-':
        {
          switch (mode)
            {
            case 's':
              if (ssize > 3) ssize -= 2;
              needs_analyse = true;
              break;
            case 'f':
              if (fsize > 1) fsize -= 2;
              needs_filtering = true;
              break;
            case 'b':
              if (fwidth > 1) fwidth -= 1;
              needs_filtering = true;
              break;
            case 'r':
              if (ratioThreshold > 1.1) ratioThreshold /= 1.1;
              needs_synthese = true;
              break;
            case 'v':
              if (valueThreshold > 0.1) valueThreshold /= 1.5;
              needs_synthese = true;
              break;
            }
        }
        break;
        case 'q':
          ready = Ask("Programm beenden?");
          break;
        }
    }
  return OK;
}


