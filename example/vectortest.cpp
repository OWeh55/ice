#include <image_nonvis.h>

using namespace ice;

int main(int argc, char* argv[])
{
  int i, j;

  Vector v5;

  cout << "Grossen Vektor durch suksessives Anhaengen erzeugen" << endl;

  for (i = 0; i < 200; i++)
    {
      v5.Append(i);
    }

  cout << v5 << endl << endl;

  v5.Resize(30);
  cout << "Resize(30)        " << v5 << endl;

  v5.Exchange(5, 8);
  cout << "v5.Exchange(5,8); " << v5 << endl;

  v5.Delete(15, 18);
  cout << "v5.Delete(15,18); " << v5 << endl;

  v5.Delete(5);
  cout << "v5.Delete(5);     " << v5 << endl;

  v5.Sort();
  cout << "v5.Sort();        " << v5 << endl;

  v5.Sort(1);
  cout << "v5.Sort(1);       " << v5 << endl;

  cout << endl << "Vektor mit Clear() loeschen " ;
  v5.Clear();
  cout << v5 << endl << endl;

  Matrix m(4, 4);

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      m[i][j] = (i + 1) * 10 + j;

  cout << "Vektoren und Matrix angelegen" << endl;

  cout << "Die Vektoren:" << endl;
  Vector v1(1, 3, 2);
  cout << v1 << "   Vector v1(1,3,2)" << endl;

  Vector v2(0, 1, 5);
  cout << v2 << "   Vector v2(0,1,5)" << endl;

  double fd[] = {3, -1, 1};
  Vector v3(3, fd);
  cout << v3 << "   double fd[]={3,-1,1}; Vector v3(3,fd);" << endl;

  Vector v4;
  v4.Append(10);
  v4.Append(-3);
  v4.Append(22);
  cout << v4 << "   Vector v4; v4.Append(10); v4.Append(-3); v4.Append(22);" << endl << endl;

  cout << "swap(v3,v4):" << endl;
  swap(v3, v4);

  cout << "v3: " << endl << v3 << endl;
  cout << "v4: " << endl << v4 << endl;

  cout << endl << "Die Matrix:" <<  endl;
  cout << m << endl;

  cout << "Die Verkettung von Vektoren mit && zu einer Matrix" << endl;
  cout << (v1 && v2 && v3 && v4) << endl;

  cout << "Die Verkettung von Vektoren mit Append zu einem großen Vektor" << endl;
  Vector v6(v1);
  v6.Append(v2);
  v6.Append(v3);
  v6.Append(v4);
  cout << v6 << endl << endl;

  cout << v1 << " + " << v2 << " = " << (v1 + v2) << endl;
  cout << v1 << " - " << v2 << " = " << (v1 - v2) << endl;
  cout << " - " << v2 << " = " << (-v2) << endl;
  cout << v1 << " * " << v2 << " = " << (v1 * v2) << endl << endl;

  cout << "Cross(v1,v2)     " << Cross(v1, v2) << endl;

  cout << "Distance(v1,v2)  " << Distance(v1, v2) << endl;
  cout << "v1.Distance(v2)  " << v1.Distance(v2) << endl;
  cout << "(v1-v2).Length() " << (v1 - v2).Length() << endl;

  cout << endl << "Gleichungssystem loesen" << endl;
  Matrix A = v1 && v2 && v3 && v4;
  cout << "Matrix A: " << endl << A << endl;

  Vector s(3, 2, 1);
  Vector ih = A * s;
  cout << "Inhomogenitaet: A * " << s << " = " << ih << endl;

  Vector r = SolveLinEqu(v1 && v2 && v3 && v4 , ih);

  cout << "Resultat: " << r << " =?= " << s << " " << boolalpha << (r.Distance(s) < 1e-10) << endl;

  return OK;
}
