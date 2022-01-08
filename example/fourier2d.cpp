#include <iomanip>
#include <image.h>
#include <FourierTrafo2D.h>

#define COLS 16
#define ROWS 9

int main(int argc, char** argv)
{
  matrix<double> mir(COLS, ROWS, 0);
  matrix<double> mii(COLS, ROWS, 0);
  matrix<double> mor(COLS, ROWS, 0);
  matrix<double> moi(COLS, ROWS, 0);
  cout << setprecision(3) << fixed;
  for (int y = 0; y < ROWS; ++y)
    for (int x = 0; x < COLS; ++x)
      mir[x][y] = cos(3 * (x - COLS / 2) * M_PI * 2 / COLS + 2 * (y - ROWS / 2) * M_PI * 2 / ROWS) + 1;
  FourierTrafo2D tr(COLS, ROWS, true, true);
  tr.setInput(mir, mii);
  tr.getResult(mor, moi);
  cout << mir << mii << "Realteil Spektrum" << endl << setw(8) << mor << "Imaginaerteil Spektrum" << endl << setw(8) << moi << endl;
  return OK;
}
