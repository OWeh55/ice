#include "operatoren.h"

#include <cmath>

const vector3 &ProduktNV::Value() const
{
  return temp = para2->Value() * para1->Value();
}

const vector3 &SummeVV::Value() const
{
  return temp = para1->Value() + para2->Value();
}

const vector3 &DifferenzVV::Value() const
{
  return temp = para1->Value() - para2->Value();
}

const float &QuotientNN::Value() const
{
  float divisor = para2->Value();
  if (divisor == 0.0)
    {
      return temp = 0.0;
    }
  return temp = para1->Value() / divisor;
}

const float &ModNN::Value() const
{
  float divisor = para2->Value();
  if (divisor == 0.0)
    {
      return temp = 0.0;
    }
  return temp = fmod(para1->Value(), divisor);
}

const vector3 &NegV::Value() const
{
  return temp = -para->Value();
}
