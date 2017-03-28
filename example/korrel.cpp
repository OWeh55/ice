#include <image.h>

#define XSIZE 1280
#define YSIZE 720
#define FPS 5

#define FRAMES 100

#define XS 10

#define YS 10

#define FNS (XS*YS)

int fn[FNS][FRAMES];

int main(int argc, char** argv)
{
  VideoFile nfo("noise.avi", ios_base::out);

  nfo.setPara(XSIZE, YSIZE, 255, 25);

  Image offs;
  offs.create(XSIZE, YSIZE, FRAMES);

  for (int y = 0; y < YSIZE; y++)
    for (int x = 0; x < XSIZE; x++)
      offs.setPixel(x, y, Random(FRAMES));

  ColorImage muster;
  muster.create(XSIZE, YSIZE, 255);

  for (int i = 0; i < FNS; i++)
    for (int j = 0; j < FRAMES; j++)
      {
        fn[i][j] = Random(255);
      }

  Show(ON, muster);

  for (int i = 0; i < FRAMES; i++)
    {
      //      cout << i << endl;
      for (int y = 0; y < YSIZE; y++)
        for (int x = 0; x < XSIZE; x++)
          {

            int t = i + GetVal(offs, x, y);

            int xi = x % XS;
            int yi = y % YS;

            int fnr = xi + yi * XS; // Nummer der Funktion

            int offsetr = x / XS; // zeitliche verschiebung der rotkomponente
            int offsetb = y / YS; // zeitliche verschiebung der blaukomponente

            muster.setPixel(x, y,
                            ColorValue(
                              fn[fnr][(t + offsetr) % FRAMES],
                              fn[fnr][ t            % FRAMES],
                              fn[fnr][(t + offsetb) % FRAMES]
                            )
                           );
          }
      /*
      SmearImg(muster.redImage());
      SmearImg(muster.greenImage());
      SmearImg(muster.blueImage());
      */
      nfo.write(muster);
    }

  return OK;
}
