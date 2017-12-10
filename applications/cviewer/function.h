#ifndef FUNCTION_H
#define FUNCTION_H

#include "numbase.h"
#include "arith.h"

template<class Tpara, class Tbase>
class Func1: public Tbase
{
public:
  Func1(Tpara* par): para(par) {}
  virtual ~Func1()
  {
    delete para;
  }
  virtual bool Constant() const
  {
    return para->Constant();
  }
  virtual string String(const string& fnc) const
  {
    return fnc + "(" + para->String() + ")";
  }
protected:
  Tpara* para;
};

//template<class T1,class T2,class TC,class TR>
template<class Tpara1, class Tpara2, class Tbase>
class Func2: public Tbase
{
public:
  Func2(Tpara1* par1, Tpara2* par2): para1(par1), para2(par2) { }
  virtual ~Func2()
  {
    delete para1, delete para2;
  }
  virtual bool Constant() const
  {
    return para1->Constant() && para2->Constant();
  }
  virtual string String(const string& fnc) const
  {
    return fnc + "(" + para1->String() + "," + para2->String() + ")";
  }
protected:
  Tpara1* para1;
  Tpara2* para2;
};

template<class Tpara1, class Tpara2, class Tpara3, class Tbase>
class Func3: public Tbase
{
public:
  Func3(Tpara1* par1, Tpara2* par2, Tpara3* par3):
    para1(par1), para2(par2), para3(par3) { }
  virtual ~Func3()
  {
    delete para1, delete para2;
    delete para3;
  }
  virtual bool Constant() const
  {
    return para1->Constant()
           && para2->Constant()
           && para3->Constant();
  }
  virtual string String(const string& fnc) const
  {
    return fnc + "(" + para1->String()
           + "," + para2->String()
           + "," + para3->String()
           + ")";
  }
protected:
  Tpara1* para1;
  Tpara2* para2;
  Tpara3* para3;
};


typedef Func1<number, number> NFuncN;
typedef Func2<number, number, number> NFuncNN;
typedef Func3<number, number, number, number> NFuncNNN;

typedef Func1<vect, number> NFuncV;

typedef Func1<vect, vect> VFuncV;
typedef Func2<vect, vect, vect> VFuncVV;
typedef Func3<vect, vect, number, vect> VFuncVVN;

typedef Func1<vect, trafo> TFuncV;
typedef Func2<vect, number, trafo> TFuncVN;
typedef Func3<vect, vect, number, trafo> TFuncVVN;

class InterpolV: public VFuncVVN
{
protected:
  mutable vector3 temp;
public:
  InterpolV(vect* par, vect* par2, number* par3):
    VFuncVVN(par, par2, par3) {} ;

  virtual const vector3& Value() const
  {
    double rel = para3->Value();
    return temp = para2->Value() * rel + para1->Value() * (1 - rel);
  }

  virtual string String() const
  {
    return VFuncVVN::String("Interpol");
  }
};

class DegreeN: public NFuncN
{
public:
  DegreeN(number* par): NFuncN(par) {}
  virtual const float& Value() const
  {
    return temp = para->Value() * 360 / M_PI;
  }
  virtual string String() const
  {
    return NFuncN::String("Degree");
  }
};

class RadianN: public NFuncN
{
public:
  RadianN(number* par): NFuncN(par) {}
  virtual const float& Value() const
  {
    return temp = para->Value() * 360 / M_PI;
  }
  virtual string String() const
  {
    return NFuncN::String("Radian");
  }
};

class SinN: public NFuncN
{
public:
  SinN(number* par): NFuncN(par) {}
  virtual const float& Value() const
  {
    return temp = Sin(para->Value());
  }
  virtual string String() const
  {
    return NFuncN::String("Sin");
  }
};

class CosN: public NFuncN
{
public:
  CosN(number* par): NFuncN(par) {}
  virtual const float& Value() const
  {
    return temp = Cos(para->Value());
  }
  virtual string String() const
  {
    return NFuncN::String("Cos");
  }
};

class MaxN: public NFuncNN
{
public:
  MaxN(number* par, number* par2): NFuncNN(par, par2) {}
  virtual const float& Value() const
  {
    return temp = Max(para1->Value(), para2->Value());
  }
  virtual string String() const
  {
    return NFuncNN::String("Max");
  }
};

class MinN: public NFuncNN
{
public:
  MinN(number* par, number* par2): NFuncNN(par, par2) {}
  virtual const float& Value() const
  {
    return temp = Min(para1->Value(), para2->Value());
  }
  virtual string String() const
  {
    return NFuncNN::String("Max");
  }
};

class InterpolN: public NFuncNNN
{
public:
  InterpolN(number* par, number* par2, number* par3): NFuncNNN(par, par2, par3) {};
  virtual const float& Value() const
  {
    double rel = para3->Value();
    return temp = rel * para2->Value() + (1 - rel) * para1->Value();
  }
  virtual string String() const
  {
    return NFuncNNN::String("Interpol");
  }
};

class NormalizeV: public VFuncV
{
public:
  NormalizeV(vect* par): VFuncV(par) {}
  virtual const vector3& Value() const;
  virtual string String() const
  {
    return VFuncV::String("Normalize");
  }
};

class Cross: public VFuncVV
{
public:
  Cross(vect* par1, vect* par2): VFuncVV(par1, par2) {}
  virtual const vector3& Value() const;
  virtual string String() const
  {
    return VFuncVV::String("Normalize");
  }
};

class Translation: public TFuncV
{
public:
  Translation(vect* par1): TFuncV(par1) {}
  virtual const Trafo& Value() const;
  virtual string String() const
  {
    return TFuncV::String("Translation");
  }
};

class Rotation: public TFuncVN
{
public:
  Rotation(vect* par1, number* par2): TFuncVN(par1, par2) {}
  virtual const Trafo& Value() const;
  virtual string String() const
  {
    return TFuncVN::String("Rotation");
  }
};

class Rotation3: public TFuncVVN
{
public:
  Rotation3(vect* par, vect* par2, number* par3):
    TFuncVVN(par, par2, par3) {} ;

  virtual const Trafo& Value() const
  {
    temp.clear();
    temp.Rotate(para1->Value(),
                para2->Value(),
                para3->Value());
    return temp;
  }

  virtual string String() const
  {
    return TFuncVVN::String("Rotation");
  }
};

class Length: public NFuncV
{
public:
  Length(vect* par): NFuncV(par) {}
  virtual const float& Value() const;
  virtual string String() const
  {
    return NFuncV::String("Length");
  }
};

#endif
