#include <image_nonvis.h>

int main(int argc, char* argv[])
{
  GaussFit g(3);
  Matrix p(0, 3);
  Vector v(0);
  g.Init();

  for (int i = 0; i < 3000; i++)
    {
      double fw = i / 1000;
      Vector z(fw * fw, fw, 1);
      double f = 9 * fw * fw + 8 * fw + 7  + 3 * (RandomD() - 0.5);
      g.Value(z, f, 1);
      p.append(z);
      v.Append(f);
    }

  g.Finish();

  vector<double> res;

  g.getResult(res);
  for (unsigned int i = 0; i < res.size(); i++)
    cout << res[i] << endl ;

  cout << g.Variance() << endl;

  g.Fit(p, v);

  g.getResult(res);
  for (unsigned int i = 0; i < res.size(); i++)
    cout << res[i] << endl ;

  return 0;
}
