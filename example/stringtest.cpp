#include <image.h>
#include <strtool.h>
#include <Vector.h>

int main(int argc, char** argv)
{
  string s1, s2, s3;
  ClearAlpha();
  s1 = "Aller Anfang ist schwer";

  for (unsigned int l = 0; l < 10; l++)
    {
      Print("Herbert sagt " + NumberString(125, l) + " mal:" + s1 + '\n');
      Print("Entropie ist " + NumberString(2 / 3.0, l, 9) + string("\n"));
    }

  GetChar();
  return OK;
}
