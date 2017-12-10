#include "function.h"

const vector3& NormalizeV::Value() const
{
  return temp = Normalize(para->Value());
}

const Trafo& Translation::Value() const
{
  temp.clear();
  temp.Shift(para->Value());
  return temp;
}

const Trafo& Rotation::Value() const
{
  temp.clear();
  temp.Rotate(para1->Value(), para2->Value());
  return temp;
}

const float& Length::Value() const
{
  return temp = para->Value().Length();
}

const vector3& Cross::Value() const
{
  return temp = para1->Value().cross(para2->Value());
}
