#include <iostream>

#include <image.h>

using namespace std;

int main(int argc, char** argv)
{
  vector<double> v{5, 6, 4, 2, 3, 2, 4, 5, 4, 3, 2, 1, 2, 3, 4};
  for (int i = 0; i < v.size(); ++i)
    cout << setw(8) << i;
  cout << endl;
  for (int i = 0; i < v.size(); ++i)
    cout << setw(8) << v[i];
  cout << endl;

  vector<int> peaks = peak1d(v, false, 0.5);
  for (int i = 0; i < v.size(); ++i)
    {
      bool isPeak = false;
      for (int k = 0; k < peaks.size(); ++k)
        if (peaks[k] == i)
          isPeak = true;
      cout << setw(8) << (isPeak ? 'X' : '.');
    }
  cout << endl;
}
