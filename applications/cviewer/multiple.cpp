#include "multiple.h"

//void MultipleObject::myDraw(int id) const
void MultipleObject::myDraw() const
{
  float ito = to->Value();
  float istep = step->Value();
  float i = from->Value();
  if (step > 0)
    {
      while (i <= ito)
        {
          lvar->Set(i);
          ob->Draw();
          i += istep;
        }
    }
  else if (step < 0)
    {
      while (i >= ito)
        {
          lvar->Set(i);
          ob->Draw();
          i += istep;
        }
    }
}

void MultipleObject::MyOptimize()
{
  DEBUGOUT("opt. multi ");
  objekt::MyOptimize();
  optnumber(from);
  optnumber(to);
  optnumber(step);
#if 0
// unfertig, könnte mal loop unrolling werden
  if (from->Constant() && to->Constant() && step->Constant())
    {
      int number = (to->Value() - from->Value()) / step->Value();
      cout << "from " << from->Value();
      cout << " to " << to->Value();
      cout << " step " << step->Value() << endl;
      cout << "Number: " << number << endl;
    }
#endif
  Optimize(ob);
  DEBUGOUT("\nend multi \n");
}
