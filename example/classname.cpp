// test zu Wrapper zu Klassifikatoren
#include <image.h>

int main(int argc, char* argv[])
{
  Display(ON);
  Alpha(ON);
  ClearAlpha();

  ClassifierWithNames<ClassifierBayes, int> named(3);

  named.addClass(5);
  named.addClass(3);
  named.addClass(8, 11);

  named.Train(5, Vector(5, 6, 7));
  named.Train(5, Vector(5, 5, 5));
  named.Train(3, Vector(3, 4, 5));
  named.Train(3, Vector(3, 3, 3));
  named.Train(8, Vector(8, 9, 10));
  named.Train(8, Vector(8, 8, 8));
  named.Train(9, Vector(9, 10, 11));
  named.Train(9, Vector(9, 9, 9));
  named.Train(10, Vector(10, 11, 12));
  named.Train(10, Vector(10, 10, 10));

  named.Finish();

  for (int i = 0; i < 12; i++)
    Printf("<%d,%d,%d> = %d\n", i, i + 1, i + 2, named.Classify(Vector(i, i + 1, i + 2)));

  GetChar();

  ClassifierWithNames<ClassifierBayes, string> names(3);

  vector<string> nl;
  nl.push_back("Drei");
  nl.push_back("Acht");
  nl.push_back("Fuenf");
  nl.push_back("Neun");
  nl.push_back("Zehn");

  names.addClass(nl);

  names.Train("Fuenf", Vector(5, 6, 7));
  names.Train("Fuenf", Vector(5, 5, 5));
  names.Train("Drei", Vector(3, 4, 5));
  names.Train("Drei", Vector(3, 3, 3));
  names.Train("Acht", Vector(8, 9, 10));
  names.Train("Acht", Vector(8, 8, 8));
  names.Train("Neun", Vector(9, 9, 9));
  names.Train("Neun", Vector(9, 10, 11));
  names.Train("Zehn", Vector(10, 11, 12));
  names.Train("Zehn", Vector(10, 10, 10));

  names.Finish();

  for (int i = 0; i < 12; i++)
    Printf("<%d,%d,%d> = %s\n",
           i, i + 1, i + 2, names.Classify(Vector(i, i + 1, i + 2)).c_str());

  //  cout << "Ausgabe " << names;

  GetChar();

  return 0;
}
