#include <image.h>

const int isize = 250;
const int xSize = isize * 3;
const int ySize = isize * 2;

Image mkImg(const Image& all, const Image& mark,
            int i, int k)
{
  Window w(i * isize, k * isize, (i + 1)*isize - 1, (k + 1)*isize - 1);
  Image nImage = all(w);
  Image nMarker = mark(w);
  nImage.set(0);
  nMarker.set(0);
  for (int i = 0; i < isize; i++)
    {
      nMarker.setPixel(0, i, 3);
      nMarker.setPixel(i, 0, 3);
      nMarker.setPixel(isize - 1, i, 3);
      nMarker.setPixel(i, isize - 1, 3);
    }
  return nImage;
}

int main(int argc, char** argv)
{
  Image all;
  all.create(xSize, ySize, 255);
  Image mark;
  mark.create(xSize, ySize, 255);
  mark.set(0);

  Show(OVERLAY, all, mark, "source - dilate - erode || opening - closing - top hat");
  Zoom(all, 2);
  Image source = mkImg(all, mark, 0, 0);

  for (int y = 0; y < source.ysize; y++)
    for (int x = 0; x < source.xsize; x++)
      {
        if (x % 16 > 0 || y > isize * 2 / 3 || (x > isize / 2 - 10 && x < isize / 2 + 10))
          source.setPixel(x, y, (1 + sin(x * 5 * M_PI * 2 / source.xsize)) * 30);
        else
          {
            source.setPixel(x, y, 255);
          }
      }

  Circle c1(isize / 2, isize / 2 - 9, 8);
  draw(c1, source, 255, 255);
  Circle c2(isize / 2, isize / 2 + 8, 8);
  draw(c2, source, 255, 255);

  Circle c3(isize / 3, isize * 3 / 4, 16);
  draw(c3, source, 255, 255);

  Circle c4(2 * isize / 3, isize * 3 / 4, 16);
  draw(c4, source, 255, 255);

  for (int y = isize * 2 / 3 - 10; y < isize * 2 / 3 + 10; y++)
    for (int x = -3; x <= 3; x++)
      source.setPixel(x + isize / 2, y, 255);

  int fSize = isize / 4;
  source(Window(0, isize - fSize, fSize - 1, isize - 1)).set(0);
  source.setPixel(fSize / 2, isize - fSize + fSize / 2, 255);

  source(Window(isize - fSize, isize - fSize, isize - 1, isize - 1)).set(255);
  source.setPixel(isize - fSize + fSize / 2, isize - fSize + fSize / 2, 0);

  const int msize = 7;
// erode
  Image erode = mkImg(all, mark, 1, 0);
  erodeImg(source, erode, msize, msize);

// dilate
  Image dilate = mkImg(all, mark, 2, 0);
  dilateImg(source, dilate, msize, msize);

// opening
  Image opening = mkImg(all, mark, 0, 1);
  openingImg(source, opening, msize, msize);

// closing
  Image closing = mkImg(all, mark, 1, 1);
  closingImg(source, closing, msize, msize);

// topHatTransform
  Image topHat = mkImg(all, mark, 2, 1);
  topHatTransform(source, topHat, msize + 2);

  GetChar();
  // now with special structuring element
  vector<vector<int>> se
  {
    {1, 0, 1, 0, 1},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1}
  };

  IMatrix im(se.size(), se[0].size());
  for (int i = 0; i < se.size(); i++)
    for (int k = 0; k < se[i].size(); k++)
      im[i][k] = se[i][k];

  erodeImg(source, erode, im);
  dilateImg(source, dilate, im);
  openingImg(source, opening, im);
  closingImg(source, closing, im);
  topHatTransform(source, topHat, im);
  GetChar();
}
