#ifndef OPERATOREN_H
#define OPERATOREN_H

#include "arith.h"
#include "function.h"

template<class Tpara1, class Tpara2, class Tbase>
class binop: public Func2<Tpara1, Tpara2, Tbase>
{
public:
  binop(Tpara1 *v1, Tpara2 *v2): Func2<Tpara1, Tpara2, Tbase>(v1, v2) {}
  virtual string String(const string &oper) const
  {
    return Func2<Tpara1, Tpara2, Tbase>::para1->String() + oper
           + Func2<Tpara1, Tpara2, Tbase>::para2->String();
  }
};

typedef binop<number, number, number> numopNN;

class ProduktNN: public numopNN
{
public:
  ProduktNN(number *v1, number *v2): numopNN(v1, v2) {}
  virtual const float &Value() const
  {
    return temp = para1->Value() * para2->Value();
  }
  virtual string String() const
  {
    return numopNN::String("*");
  }
};

class QuotientNN: public numopNN
{
public:
  QuotientNN(number *v1, number *v2): numopNN(v1, v2) {}
  virtual const float &Value() const;
  virtual string String() const
  {
    return numopNN::String("/");
  }
};

class ModNN: public numopNN
{
public:
  ModNN(number *v1, number *v2): numopNN(v1, v2) {}
  virtual const float &Value() const;
  virtual string String() const
  {
    return numopNN::String(" MOD ");
  }
};

class SummeNN: public numopNN
{
public:
  SummeNN(number *v1, number *v2): numopNN(v1, v2) {}
  virtual const float &Value() const
  {
    return temp = para1->Value() + para2->Value();
  }
  virtual string String() const
  {
    return numopNN::String("+");
  }
};

class DifferenzNN: public numopNN
{
public:
  DifferenzNN(number *v1, number *v2): numopNN(v1, v2) {}
  virtual const float &Value() const
  {
    return temp = para1->Value() - para2->Value();
  }
  virtual string String() const
  {
    return numopNN::String("-");
  }
};

typedef binop<boolean, boolean, boolean> boolopBB;

class AndBB: public boolopBB
{
public:
  AndBB(boolean *v1, boolean *v2): boolopBB(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() && para2->Value();
  }
  virtual string String() const
  {
    return boolopBB::String("and");
  }
};

class OrBB: public boolopBB
{
public:
  OrBB(boolean *v1, boolean *v2): boolopBB(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() || para2->Value();
  }
  virtual string String() const
  {
    return boolopBB::String("or");
  }
};

typedef binop<number, number, boolean> boolopNN;

class EqualNN: public boolopNN
{
public:
  EqualNN(number *v1, number *v2): boolopNN(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() == para2->Value();
  }
  virtual string String() const
  {
    return boolopNN::String("==");
  }
};

class NotEqualNN: public boolopNN
{
public:
  NotEqualNN(number *v1, number *v2): boolopNN(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() != para2->Value();
  }
  virtual string String() const
  {
    return boolopNN::String("<>");
  }
};

class LessEqualNN: public boolopNN
{
public:
  LessEqualNN(number *v1, number *v2): boolopNN(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() <= para2->Value();
  }
  virtual string String() const
  {
    return boolopNN::String("<=");
  }
};

class GreaterEqualNN: public boolopNN
{
public:
  GreaterEqualNN(number *v1, number *v2): boolopNN(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() >= para2->Value();
  }
  virtual string String() const
  {
    return boolopNN::String(">=");
  }
};

class LessNN: public boolopNN
{
public:
  LessNN(number *v1, number *v2): boolopNN(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() < para2->Value();
  }
  virtual string String() const
  {
    return boolopNN::String("<");
  }
};

class GreaterNN: public boolopNN
{
public:
  GreaterNN(number *v1, number *v2): boolopNN(v1, v2) {}
  virtual const bool &Value() const
  {
    return temp = para1->Value() > para2->Value();
  }
  virtual string String() const
  {
    return boolopNN::String(">");
  }
};

typedef binop<vect, vect, vect> vecopVV;

class SummeVV: public vecopVV
{
public:
  SummeVV(vect *v1, vect *v2): vecopVV(v1, v2) {}
  virtual const vector3 &Value() const;
  virtual string String() const
  {
    return vecopVV::String("+");
  }
};

class DifferenzVV: public vecopVV
{
public:
  DifferenzVV(vect *v1, vect *v2): vecopVV(v1, v2) {}
  virtual const vector3 &Value() const;
  virtual string String() const
  {
    return vecopVV::String("-");
  }
};

typedef binop<number, vect, vect> vecopNV;

class ProduktNV: public vecopNV
{
public:
  ProduktNV(number *v1, vect *v2): vecopNV(v1, v2) {}
  virtual const vector3 &Value() const;
  virtual string String() const
  {
    return vecopNV::String("*");
  }
};

typedef binop<vect, vect, number> numopVV;

class ProduktVV: public numopVV
{
public:
  ProduktVV(vect *v1, vect *v2): numopVV(v1, v2) {}
  virtual const float &Value() const
  {
    return temp = para1->Value() * para2->Value();
  }
  virtual string String() const
  {
    return numopVV::String("*");
  }
};

typedef binop<trafo, trafo, trafo> tropTT;
class ProduktTT: public tropTT
{
public:
  ProduktTT(trafo *v1, trafo *v2): tropTT(v1, v2) {}
  virtual const Trafo &Value() const
  {
    temp = para2->Value();
    temp.Append(para1->Value());
    return temp;
  }
  virtual string String() const
  {
    return tropTT::String("*");
  }
};

typedef binop<trafo, vect, vect> vecopTV;
class ProduktTV: public vecopTV
{
public:
  ProduktTV(trafo *v1, vect *v2): vecopTV(v1, v2) {}
  virtual const vector3 &Value() const
  {
    return temp = para1->Value().Transform(para2->Value());
  }
  virtual string String() const
  {
    return vecopTV::String("*");
  }
};

class NegN: public NFuncN
{
public:
  NegN(number *par): NFuncN(par) {}
  virtual const float &Value() const
  {
    return temp = -para->Value();
  }
  virtual string String() const
  {
    return "-" + para->String();
  }
};

class NegV: public VFuncV
{
public:
  NegV(vect *par): VFuncV(par) {}
  virtual const vector3 &Value() const;
  virtual string String() const
  {
    return "-" + para->String();
  }
};

class CondNN: public Func3<boolean, number, number, number>
{
public:
  CondNN(boolean *v1, number *v2, number *v3): Func3<boolean, number, number, number>(v1, v2, v3) {}

  virtual const float &Value() const
  {
    temp = para1->Value() ? para2->Value() : para3->Value();
    // cout << temp << endl;
    return temp;
  }

  virtual string String() const
  {
    return para1->String() + " ? " + para2->String() + " : " + para3->String();
  }
};

class CondVV: public Func3<boolean, vect, vect, vect>
{
public:
  CondVV(boolean *v1, vect *v2, vect *v3): Func3<boolean, vect, vect, vect>(v1, v2, v3) {}

  virtual const vector3 &Value() const
  {
    if (para1->Value())
      temp = para2->Value();
    else
      temp = para3->Value();
    // cout << temp << endl;
    return temp;
  }

  virtual string String() const
  {
    return para1->String() + " ? " + para2->String() + " : " + para3->String();
  }
};

#endif
