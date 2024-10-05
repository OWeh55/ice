#include <image_nonvis.h>
#include <region.h>

using namespace ice;

void sout(const Region& sl)
{
  int i, j;

  for (j = 0; j < 8; j++)
    {
      for (i = 0; i < 8; i++)
        if (sl.inside(i, j)) cout << "#" ;
        else cout << ".";

      cout << endl;
    }

  cout << sl << "area: " << sl.getArea() << endl;
}

int main(int argc, char* argv[])
{
  Region s;
  cout << "Test Region" << endl;
  s.add(2, 3, 5, 6);
  sout(s);

  s.add(3, 6, 7, 6);

  s.add(3, 3, 6, 6);
  sout(s);

  s.del(2, 3, 5, 6);
  sout(s);
  s.del(3, 6, 7, 6);
  sout(s);
  s.del(3, 3, 6, 6);
  sout(s);
  return OK;
}
