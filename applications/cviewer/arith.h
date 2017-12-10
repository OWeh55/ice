#ifndef ARITH_H
#define ARITH_H

#include <iostream>
#include <vector>

#include "vector3.h"
#include "stringtools.h"
#include "trafo.h"
#include "surface.h"

using namespace std;

#ifdef WIN32
#define M_PI (3.14159265)
#endif

#define TypScalar 1
#define TypVector 2
#define TypList 3
#define TypSpline 4
#define TypBoolean 5
#define TypTrafo 6
#define TypTexture 7

#define T2(a,b) ((a)*10+(b))

#define TSS T2(TypScalar,TypScalar)
#define TSV T2(TypScalar,TypVector)
#define TVS T2(TypVector,TypScalar)
#define TVV T2(TypVector,TypVector)

#define T3(a,b,c) ((a)*100+(b)*10+(c))

// Basis-Klasse für Werte aller Art
class value
{
public:
  virtual int Type() const = 0;
  virtual bool Constant() const = 0;
  virtual string String() const = 0;
  virtual ~value() {}
};

/*
 * (abstrakte) Basis-Klassen für spezielle Resultat-Typen
 * (simple Typen)
 */

template<class T, int TypId>
class basetype : public value
{
protected:
  mutable T temp;
public:
  virtual int Type() const
  {
    return TypId;
  }
  virtual const T& Value() const = 0;
  virtual ~basetype() {};
};

typedef basetype<float, TypScalar> number;
typedef number* numberp;

typedef basetype<bool, TypBoolean> boolean;
typedef boolean* booleanp;

typedef basetype<vector3, TypVector> vect;
typedef vect* vectp;

typedef basetype<Trafo, TypTrafo> trafo;
typedef trafo* trafop;

typedef basetype<Texture, TypTexture> texture;
typedef texture* texturep;

/*
 * Basis-Klassen für spezielle Resultat-Typen
 * (komplexe Typen)
 */

class vectlist: public value
{
public:
  vectlist() {} ;
  vectlist(const vectlist& vl): vlist(vl.vlist) {} ;
  virtual int Type() const
  {
    return TypList;
  }
  virtual bool Constant() const
  {
    vector<vectp>::const_iterator i;
    for (i = vlist.begin(); i < vlist.end(); i++)
      if (!(*i)->Constant()) return false;
    return true;
  };
  virtual void Add(vectp v)
  {
    vlist.push_back(v);
  }
  vector<vectp>::size_type size() const
  {
    return vlist.size();
  }
  const vector3 Value(int i) const
  {
    return vlist[i]->Value();
  }
  vectp& operator[](int i)
  {
    return vlist[i];
  }
  const vectp& operator[](int i) const
  {
    return vlist[i];
  }
  virtual ~vectlist() {} ;
  virtual string String() const
  {
    return "VektorListe";
  }
  virtual void Invert();
protected:
  vector<vectp> vlist;
};

/*
 * Klassen für konstante Werte (simple Typen)
 */

template<class base, class valuetype>
class constant: public base
{
public:
  constant(const valuetype& v)
  {
    value = v;
  }
  bool Constant() const
  {
    return true;
  }
  const valuetype& Value() const
  {
    return value;
  }
  virtual string String() const
  {
    return ToString(value);
  }
protected:
  valuetype value;
};

typedef constant<number, float> nconst;
typedef constant<vect, vector3> vconst;
typedef constant<trafo, Trafo> tconst;

/*
 * Klassen für "benannte Ausdrücke" == WertDefinition
 */

template<class base, class valuetype>
class namedExpression: public base
{
public:
  namedExpression(base* p)
  {
    val = p;
  }
  virtual ~namedExpression()
  {
    /* val nicht löschen!! */
  }
  virtual bool Constant() const
  {
    return val->Constant();
  }
  virtual const valuetype& Value() const
  {
    return val->Value();
  }
  virtual string String() const
  {
    return "(" + val->String() + ")";
  }
protected:
  base* val;
};

typedef namedExpression<number, float> namednumber;
typedef namedExpression<trafo, Trafo> namedtrafo;
typedef namedExpression<vect, vector3> namedvect;
typedef namedExpression<texture, Texture> namedtext;

//-----------------------------------------------------
extern float VirtualTime;

class timen: public number
{
public:
  virtual bool Constant() const
  {
    return false;
  }
  virtual const float& Value() const
  {
    return VirtualTime;
  }
  virtual string String() const
  {
    return "TIME";
  }
};

class nvar: public number
{
  // echte "Variable", Wert der sich durch Zuweisungen ändert
  // Verwendet in For-Schleife
  // hat hier keinen Namen sondern muß in
  // Variablenliste eingetragen sein
public:
  nvar(const string& namep, float v = 0.0): name(namep), value(v) {}
  bool Constant() const
  {
    return false;
  }
  const float& Value() const
  {
    return value;
  }
  void Set(float val)
  {
    value = val;
  }
  virtual string String() const
  {
    return "Variable " + name;
  }
protected:
  string name;
  float value;
};

class spline: public vectlist
{
public:
  virtual int Type() const
  {
    return TypSpline;
  }
  spline(const string& namep, int type): name(namep), typ(type) {};
  spline(const string& namep, const vectlist& vl, int type): vectlist(vl), name(namep), typ(type) {};
  const vector3& Value(float i) const;
  virtual string String() const
  {
    return name;
  }
protected:
  string name;
  int typ;
  mutable vector3 temp;
};

#if 0
class vlaccess: public vect
{
  // zugriff auf Vektorlisten-Element
public:
  vlvariable(vectlist* p, number* idx): val(p), index(idx) {}
  virtual ~vlvariable()
  {
    /* val nicht löschen!! */
  }
  virtual bool Constant() const
  {
    return (val->Constant() && index->Constant());
  }
  virtual const vector3& Value() const
  {
    return val->Value((int)index->Value());
  }
  virtual string String() const
  {
    return "(" + val->String() + ")";
  }
protected:
  vectlist* val;
  number* index;
};
#endif

class splineaccess: public vect
{
  // Zugriff auf Spline
public:
  splineaccess(spline* p, number* idx): val(p), index(idx) {}
  virtual ~splineaccess()
  {
    /* val und index nicht löschen!! */
  }
  virtual bool Constant() const
  {
    return (val->Constant() && index->Constant());
  }
  virtual const vector3& Value() const;
  virtual string String() const
  {
    return val->String() + "[" + index->String() + "]";
  }
protected:
  spline* val;
  number* index;
};

class vnum3: public vect
{
protected:
  numberp val1, val2, val3;
public:
  vnum3(numberp v1, numberp v2, numberp v3): val1(v1), val2(v2), val3(v3) {}
  virtual ~vnum3()
  {
    delete val1;
    delete val2;
    delete val3;
  }
  virtual bool Constant() const
  {
    return false;
  }
  virtual const vector3& Value() const;
  virtual string String() const
  {
    return "<" + val1->String() + ","
           + val2->String() + ","
           + val3->String() + ">";
  }
};
#endif

