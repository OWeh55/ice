#include <fouriertrafo.h>

#include <iostream>
#include <iomanip>

using namespace std;
using namespace ice;

// test der Klasse FourierTrafo

void print(const vector<double>& v)
{
  int dim = v.size();
  int fm = dim / 2;
  for (int i = -fm; i < dim - fm; ++i)
    cout << setw(6) << i << " ";
  cout << endl;
  for (int i = -fm; i < dim - fm; ++i)
    {
      int idx = i + fm;
      //      cout << setw(7) << idx << " " ;
      if (fabs(v[idx]) < 1e-10)
        cout << setw(6) << "-" << " ";
      else
        cout << setw(6) << setprecision(3) << fixed << v[idx] << " ";
    }
  cout << endl;
}

const int size = 16;

int main(int argc, char** argv)
{
  vector<double> v(size, 0.0);
  vector<double> rr(size);
  vector<double> ri(size);

  int deltai = size / 2;
  v[deltai] = 1;

  for (int i = 0; i < size; ++i)
    v[i] = cos((i - deltai) * M_PI * 2 / size);

  FourierTrafo ft(size, true, true);
  ft.setInput(v);
  cout << "Original-Funktion" << endl;
  print(v);
  ft.getResult(rr, ri);
  cout << "Spektrum (Real-Teil)" << endl;
  print(rr);
  cout << "Spektrum (Imaginär-Teil)" << endl;
  print(ri);

  FourierTrafo fti(size, false, true);
  ft.setInput(rr, ri);
  ft.getResult(rr, ri);
  cout << "Zurücktransformiert (Real-Teil)" << endl;
  print(rr);
  cout << "Zurücktransformiert (Imaginär-Teil)" << endl;
  print(ri);
}
