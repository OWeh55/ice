#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <vector>

#include "defs.h"

// darstellbare objekte
#include "objekte.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "triangle.h"
#include "polygon.h"
#include "hf.h"
#include "multiple.h"

#include "cviewer.h"
#include "parsdefs.h"
#include "arith.h"
#include "operatoren.h"
#include "variablen.h"
#include "function.h"

//value* Ausdruck(string &line);
//number *Number(string &line);
//vect *Vect(string &line);

//bool Objekt(optr &aworld,const string &name,string &line);
//bool UserDef(optr &aworld,const string &name,string &line);

// Fehlermeldungen
void cerror(const string &mesg)
{
  cerr << infilenames[IncludeStackPtr] << ":" << zeilennr << ":error:" << mesg << endl;
  exit(5);
}

int yyerror(const char *msg)
{
  cerror(msg);
  return 0;
}

void fatal_lex_error(const char msg[])
{
  cerror(msg);
}

// Funktionen zur Generierung numerischer und vectorieller Werte
vect *Vect(value *p1, value *p2, value *p3)
{
  if (!((p1->Type() == TypScalar) && (p2->Type() == TypScalar) && (p3->Type() == TypScalar)))
    {
      cerror("Numerischen Wert erwartet");
    }
  number *v1 = (number *) p1;
  number *v2 = (number *) p2;
  number *v3 = (number *) p3;
  if (v1->Constant() && v2->Constant() && v3->Constant())
    {
      vect *res = new vconst(vector3(v1->Value(), v2->Value(), v3->Value()));
      delete v1;
      delete v2;
      delete v3;
      return res;
    }
  else
    {
      return new vnum3(v1, v2, v3);
    }
}

value *Produkt(value *p1, value *p2)
{
  switch (T2(p1->Type(), p2->Type()))
    {
    case TSS:
      return new ProduktNN((number *) p1, (number *) p2);
      break;
    case TSV:
      return new ProduktNV((number *) p1, (vect *) p2);
      break;
    case TVS:
      return new ProduktNV((number *) p2, (vect *) p1);
      break;
    case TVV:
      return new ProduktVV((vect *) p1, (vect *) p2);
      break;
    case T2(TypTrafo, TypVector) :
      return new ProduktTV((trafo *) p1, (vect *) p2);
      break;
    case T2(TypTrafo, TypTrafo) :
      return new ProduktTT((trafo *) p1, (trafo *) p2);
      break;

    default:
      cerror("Inkompatible Typen bei *");
    }
  return NULL;
}

value *Concat(value *p1, value *p2)
{
  switch (T2(p1->Type(), p2->Type()))
    {
    case T2(TypTrafo, TypTrafo) :
      return new ProduktTT((trafo *) p2, (trafo *) p1);
      break;

    default:
      cerror("Inkompatible Typen bei :");
    }
  return NULL;
}

value *Quotient(value *p1, value *p2)
{
  switch (T2(p1->Type(), p2->Type()))
    {
    case TSS:
      return new QuotientNN((number *) p1, (number *) p2);
      break;
    default:
      cerror("Inkompatible Typen bei /");
    }
  return NULL;
}

value *Mod(value *p1, value *p2)
{
  switch (T2(p1->Type(), p2->Type()))
    {
    case TSS:
      return new ModNN((number *) p1, (number *) p2);
      break;
    default:
      cerror("Inkompatible Typen bei %");
    }
  return NULL;
}

value *Summe(value *p1, value *p2)
{
  switch (T2(p1->Type(), p2->Type()))
    {
    case TSS:
      return new SummeNN((number *) p1, (number *) p2);
      break;
    case TVV:
      return new SummeVV((vect *) p1, (vect *) p2);
      break;
    default:
      cerror("Inkompatible Typen bei +");
    }
  return NULL;
}

value *Differenz(value *p1, value *p2)
{
  switch (T2(p1->Type(), p2->Type()))
    {
    case TSS:
      return new DifferenzNN((number *) p1, (number *) p2);
      break;
    case TVV:
      return new DifferenzVV((vect *) p1, (vect *) p2);
      break;
    default:
      cerror("Inkompatible Typen bei -");
    }
  return NULL;
}

value *Neg(value *p1)
{
  switch (p1->Type())
    {
    case TypScalar:
      return new NegN((number *) p1);
      break;
    case TypVector:
      return new NegV((vect *) p1);
      break;
    default:
      cerror("Inkompatible Typen bei -");
    }
  return NULL;
}

value *Funktion(const string &name, value *p1)
{
  int t1 = p1->Type();

  if ((name == "sin") && (t1 == TypScalar))
    {
      return new SinN((number *) p1);
    }

  if ((name == "cos") && (t1 == TypScalar))
    {
      return new CosN((number *) p1);
    }

  if ((name == "length") && (t1 == TypVector))
    {
      return new Length((vect *) p1);
    }

  if ((name == "normalize") && (t1 == TypVector))
    {
      return new NormalizeV((vect *) p1);
    }

  if ((name == "translation") && (t1 == TypVector))
    {
      return new Translation((vect *) p1);
    }

  cerror("Unbekannte Funktion " + name);
  return NULL;
}

value *Funktion(const string &name, value *p1, value *p2)
{
  int t2 = T2(p1->Type(), p2->Type());

  if ((name == "min") && (t2 == TSS))
    {
      return new MinN((number *) p1, (number *) p2);
    }

  if ((name == "max") && (t2 == TSS))
    {
      return new MaxN((number *) p1, (number *) p2);
    }

  if ((name == "cross") && (t2 == TVV))
    {
      return new Cross((vect *) p1, (vect *) p2);
    }

  cerror("Unbekannte Funktion " + name);
  return NULL;
}

value *Funktion(const string &name, value *p1, value *p2, value *p3)
{
  int t3 = T3(p1->Type(), p2->Type(), p3->Type());

  if ((name == "interpol") && (t3 == T3(TypScalar, TypScalar, TypScalar)))
    {
      return new InterpolN((number *) p1, (number *) p2, (number *) p3);
    }

  if ((name == "interpol") && (t3 == T3(TypVector, TypVector, TypScalar)))
    {
      return new InterpolV((vect *) p1, (vect *) p2, (number *) p3);
    }

  if ((name == "rotation") && (t3 == T3(TypVector, TypVector, TypScalar)))
    {
      return new Rotation3((vect *) p1, (vect *) p2, (number *) p3);
    }

  cerror("Unbekannte Funktion " + name);
  return NULL;
}

// Logische Operatoren
value *And(value *p1, value *p2)
{
  if ((p1->Type() == TypBoolean) && (p2->Type() == TypBoolean))
    return new AndBB((boolean *) p1, (boolean *) p2);
  else
    {
      cerror("Inkompatible Typen bei and");
      return NULL;
    }
}

value *Or(value *p1, value *p2)
{
  if ((p1->Type() == TypBoolean) && (p2->Type() == TypBoolean))
    return new OrBB((boolean *) p1, (boolean *) p2);
  else
    {
      cerror("Inkompatible Typen bei or");
      return NULL;
    }
}

value *Conditional(value *p1, value *p2, value *p3)
{
  if ((p1->Type() == TypBoolean) && (p2->Type() == TypScalar) && (p3->Type() == TypScalar))
    return new CondNN((boolean *) p1, (number *) p2, (number *) p3);
  else if ((p1->Type() == TypBoolean) && (p2->Type() == TypVector) && (p3->Type() == TypVector))
    return new CondVV((boolean *) p1, (vect *) p2, (vect *) p3);
  else
    {
      cerror("Inkompatible Typen bei ?:");
      return NULL;
    }
}

// Vergleiche
value *Equal(value *p1, value *p2)
{
  if ((p1->Type() == TypScalar) && (p2->Type() == TypScalar))
    return new EqualNN((number *) p1, (number *) p2);
  else
    {
      cerror("Inkompatible Typen bei ==");
      return NULL;
    }
}

value *NotEqual(value *p1, value *p2)
{
  if ((p1->Type() == TypScalar) && (p2->Type() == TypScalar))
    return new NotEqualNN((number *) p1, (number *) p2);
  else
    {
      cerror("Inkompatible Typen bei <>");
      return NULL;
    }
}

value *LessEqual(value *p1, value *p2)
{
  if ((p1->Type() == TypScalar) && (p2->Type() == TypScalar))
    return new LessEqualNN((number *) p1, (number *) p2);
  else
    {
      cerror("Inkompatible Typen bei <=");
      return NULL;
    }
}

value *GreaterEqual(value *p1, value *p2)
{
  if ((p1->Type() == TypScalar) && (p2->Type() == TypScalar))
    return new GreaterEqualNN((number *) p1, (number *) p2);
  else
    {
      cerror("Inkompatible Typen bei >=");
      return NULL;
    }
}

value *LessThen(value *p1, value *p2)
{
  if ((p1->Type() == TypScalar) && (p2->Type() == TypScalar))
    return new LessNN((number *) p1, (number *) p2);
  else
    {
      cerror("Inkompatible Typen bei <");
      return NULL;
    }
}

value *GreaterThen(value *p1, value *p2)
{
  if ((p1->Type() == TypScalar) && (p2->Type() == TypScalar))
    return new GreaterNN((number *) p1, (number *) p2);
  else
    {
      cerror("Inkompatible Typen bei >");
      return NULL;
    }
}

value *Punktliste(value *p1)
{
  // first point of a pointlist
  if (p1->Type() != TypVector)
    cerror("Element in Punktliste ist kein Vektor");

  vectlist *vl = new vectlist();
  vl->Add((vect *) p1);
  return vl;
}

value *Punktliste(vectlist *liste, value *p)
{
  // next point of a pointlist
  if (p->Type() != TypVector)
    cerror("Element in Punktliste ist kein Vektor");
  liste->Add((vect *) p);
  return liste;
}

value *SplineWert(const string &name, value *p)
{
  value *rv = NULL;
  value *v = find(varstack, name);
  if (v == NULL) cerror(string("Spline-Variable existiert nicht: ") + name);
  if (p->Type() != TypScalar) cerror("Falscher Indextyp");
  if (v->Type() == TypSpline) rv = new splineaccess((spline *) v, (number *) p);
  else cerror(name + " ist kein Spline");
  return rv;
}

//---------------------
void BeginObjekt(const string &name)
{
  PushWorld();
  if (find(ostack, name) != NULL)
    cerror("Objekt existiert schon: " + name);
}

void EndObjekt(const string &name, int global)
{
  if (!global)
    new var<objekt> (ostack.back(), name, world);
  else
    {
      if (ostack.size() < 2) cerror("global in globale namespace invalid");
      // eine Ebene höher eintragen
      new var<objekt> (ostack[ostack.size() - 2], name, world);
    }
  PopWorld();
};

//--------------------------------------------------
vector<optr> worldstack;
void PushWorld()
{
  worldstack.push_back(world);
  world = NULL;
}

void PopWorld()
{
  world = worldstack.back();
  worldstack.pop_back();
}
//------------------------------------------
void Sphere(value *center, value *rad)
{
  if ((center->Type() != TypVector) || (rad->Type() != TypScalar))
    {
      cerror("sphere - Parameterfehler");
    }
  Append(world, new sphere((vect *) center, (number *) rad));
}
//------------------------------------------
void Cylinder(value *p1, value *p2, value *rad)
{
  if ((p1->Type() != TypVector) || (p2->Type() != TypVector) || (rad->Type() != TypScalar))
    {
      cerror("cylinder - Parameterfehler");
    }
  Append(world, new cylinder((vect *) p1, (vect *) p2, (number *) rad));
}
//------------------------------------------
void Box(value *p1, value *p2)
{
  if ((p1->Type() != TypVector) || (p2->Type() != TypVector))
    {
      cerror("box - Parameterfehler");
    }
  Append(world, new box((vect *) p1, (vect *) p2));
}
//--------------------------------------------
void Triangle(value *p1, value *p2, value *p3)
{
  if ((p1->Type() != TypVector) || (p2->Type() != TypVector) || (p3->Type() != TypVector))
    {
      cerror("triangle - Parameterfehler");
    }
  Append(world, new triangle((vect *) p1, (vect *) p2, (vect *) p3));
}
//--------------------------------------------
void HeightField(const string &name, value *p1, value *p2, value *p3, value *p4)
{
  if ((p1->Type() != TypVector) ||
      (p2->Type() != TypVector) ||
      (p3->Type() != TypVector) ||
      (p4->Type() != TypVector)
     )
    {
      cerror("heightfield - Parameterfehler");
    }
  Append(world, new heightfield(name, (vect *) p1, (vect *) p2, (vect *) p3, (vect *) p4));
}

void HeightField(const string &name, value *p1, value *p2, value *p3)
{
  if ((p1->Type() != TypVector) ||
      (p2->Type() != TypVector) ||
      (p3->Type() != TypVector)
     )
    {
      cerror("heightfield - Parameterfehler");
    }
  Append(world, new heightfield(name, (vect *) p1, (vect *) p2, (vect *) p3));
}

void HeightField(const string &name, value *p1)
{
  if (p1->Type() != TypVector)
    {
      cerror("heightfield - Parameterfehler");
    }
  Append(world, new heightfield(name, (vect *) p1));
}
//--------------------------------------------
void Color(value *col)
{
  if (col->Type() != TypVector)
    cerror("color - Parameterfehler");
  Append(world, new color((vect *) col));
}
//--------------------------------------------
void UserObjekt(const string &name, value *p1, value *p2, value *p3)
{
  optr myobject = find(ostack, name);
  if (myobject == NULL)
    cerror(name + " ist undefiniert");
  if ((p1->Type() != TypVector) ||
      (p2->Type() != TypScalar) ||
      (p3->Type() != TypVector))
    cerror(name + " - Parameterfehler");
  Append(world, new ocall(myobject, (vect *) p1, (number *) p2, (vect *) p3));
}
//---------------------------------------------
void UserObjekt(const string &name, value *p1)
{
  optr myobject = find(ostack, name);
  if (myobject == NULL)
    cerror(name + " ist undefiniert");
  if ((p1->Type() != TypTrafo))
    cerror(name + " - Parameterfehler");
  Append(world, new ocalltrafo(myobject, (trafo *) p1));
}
//---------------------------------------------
void For1(const string &name)
{
  // Lauf-Variable
  NewNamespace();
  nvar *lvp = new nvar(name);
  new var<value> (varstack.back(), name, lvp);
  PushWorld();
}

void For2(const string &name, value *from, value *to)
{
  if ((from->Type() != TypScalar) || (to->Type() != TypScalar))
    cerror("for " + name + " - Parameterfehler");

  nvar *lvp = (nvar *) find(varstack, name);
  MultipleObject *mop = new MultipleObject((number *) from, (number *) to, new nconst(1), lvp, world);
  LeaveNamespace();
  PopWorld();
  Append(world, mop);
}

void For3(const string &name, value *from, value *to, value *step)
{
  if ((from->Type() != TypScalar) || (to->Type() != TypScalar) || (step->Type() != TypScalar))
    cerror("for " + name + " - Parameterfehler");

  nvar *lvp = (nvar *) find(varstack, name);
  MultipleObject *mop = new MultipleObject(
    (number *) from, (number *) to, (number *) step, lvp, world);
  varstack.front()->Delete(name);
  PopWorld();
  LeaveNamespace();
  Append(world, mop);
}

value *If1(value *cond)
{
  if (cond->Type() != TypBoolean)
    {
      cerror("if - Erwarte Boolschen Wert");
    }
  PushWorld();
  return cond;
}

objekt *If2(void)
{
  objekt *ret = world;
  PopWorld();
  PushWorld();
  return ret;
}

objekt *If3(void)
{
  objekt *ret = world;
  PopWorld();
  return ret;
}

void If(value *cond, objekt *o1, objekt *o2)
{
  Append(world, new CondObject((boolean *) cond, o1, o2));
}

//---------------------------------------
void Polygon(vectlist *vl, bool inverse)
{
  Append(world, new polygon(vl, inverse));
}

//---------------------------------------
void Light(value *col, value *pos)
{
  int i;
  if (col->Type() != TypVector) cerror("Farbe der Lichtquelle muß Vektor sein");
  if (!col->Constant()) cerror("Farbe der Lichtquelle muß konstant sein");
  if (pos->Type() != TypVector) cerror("Position der Lichtquelle muß Vektor sein");
  if (!pos->Constant()) cerror("Position der Lichtquelle muß konstant sein");
  if (!(light_number < 2)) cerror("Zu viele Lichtquellen");

  const vector3 &vpos = ((vect *) pos)->Value();
  const vector3 &vcol = ((vect *) col)->Value();

  if (light_number == 0)
    {
      for (i = 0; i < 3; i++) pos0[i] = vpos[i];
      pos0[3] = 1.0;
      for (i = 0; i < 3; i++) dif0[i] = vcol[i];
    }
  else
    {
      for (i = 0; i < 3; i++) pos1[i] = vpos[i];
      pos1[3] = 1.0;
      for (i = 0; i < 3; i++) dif1[i] = vcol[i];
    }
  light_number++;
}

void Light(value *col)
{
  int i;
  if (col->Type() != TypVector) cerror("Farbe der Lichtquelle muß Vektor sein");
  if (!col->Constant()) cerror("Farbe der Lichtquelle muß konstant sein");
  vector3 vcol = ((vect *) col)->Value();
  for (i = 0; i < 3; i++) globalambient[i] = vcol[i];
}

void Spline(const string &name, vectlist *vl, int mode)
{
  spline *sp = new spline(name, *vl, mode);
  new var<value> (varstack.back(), name, sp);
}

void Point(value *ort)
{
  if (ort->Type() != TypVector)
    cerror("point - Parametertypfehler");
  Append(world, new point((vect *) ort));
}

void EnterLocal()
{
  NewNamespace();
}

void LeaveLocal()
{
  LeaveNamespace();
}

void initvars()
{
  varstack.clear();
  ostack.clear();

  NewNamespace();

// Zeit
  new var<value> (varstack.front(), "TIME", new timen());

// Grundrichtungen
  new var<value> (varstack.front(), "X", new vconst(vector3(1, 0, 0)));
  new var<value> (varstack.front(), "Y", new vconst(vector3(0, 1, 0)));
  new var<value> (varstack.front(), "Z", new vconst(vector3(0, 0, 1)));

// Ursprung
  new var<value> (varstack.front(), "NULL", new vconst(vector3(0, 0, 0)));

// Farben zweisprachig
  new var<value> (varstack.front(), "ROT", new vconst(vector3(1, 0, 0)));
  new var<value> (varstack.front(), "RED", new vconst(vector3(1, 0, 0)));

  new var<value> (varstack.front(), "GRUEN", new vconst(vector3(0, 1, 0)));
  new var<value> (varstack.front(), "GREEN", new vconst(vector3(0, 1, 0)));

  new var<value> (varstack.front(), "BLAU", new vconst(vector3(0, 0, 1)));
  new var<value> (varstack.front(), "BLUE", new vconst(vector3(0, 0, 1)));

  new var<value> (varstack.front(), "GELB", new vconst(vector3(1, 1, 0)));
  new var<value> (varstack.front(), "YELLOW", new vconst(vector3(1, 1, 0)));

  new var<value> (varstack.front(), "MAGENTA", new vconst(vector3(1, 0, 1)));
  new var<value> (varstack.front(), "PURPUR", new vconst(vector3(0.5, 0, 0.5)));

  new var<value> (varstack.front(), "CYAN", new vconst(vector3(0, 1, 1)));

  new var<value> (varstack.front(), "WHITE", new vconst(vector3(1, 1, 1)));
  new var<value> (varstack.front(), "WEISS", new vconst(vector3(1, 1, 1)));

  new var<value> (varstack.front(), "SCHWARZ", new vconst(vector3(0, 0, 0)));
  new var<value> (varstack.front(), "BLACK", new vconst(vector3(0, 0, 0)));

  new var<value> (varstack.front(), "GRAU", new vconst(vector3(0.5, 0.5, 0.5)));
  new var<value> (varstack.front(), "GRAY", new vconst(vector3(0.5, 0.5, 0.5)));

// Webfarben
//  new var<value>(varstack.front(),"BLACK",new vconst(vector3(0,0,0)));
  new var<value> (varstack.front(), "MAROON", new vconst(vector3(0.5, 0, 0)));
//  new var<value>(varstack.front(),"GREEN",new vconst(vector3(0,0.5,0))); // Konflikt
  new var<value> (varstack.front(), "OLIVE", new vconst(vector3(0.5, 0.5, 0)));
  new var<value> (varstack.front(), "NAVY", new vconst(vector3(0, 0, 0.5)));
  new var<value> (varstack.front(), "PURPLE", new vconst(vector3(0.5, 0, 0.5)));
  new var<value> (varstack.front(), "TEAL", new vconst(vector3(0, 0.5, 0.5)));
  new var<value> (varstack.front(), "SILVER", new vconst(vector3(0.75, 0.75, 0.75)));
//  new var<value>(varstack.front(),"GRAY",new vconst(vector3(0.5,0.5,0.5)));
//  new var<value>(varstack.front(),"RED",new vconst(vector3(1,0,0)));
  new var<value> (varstack.front(), "LIME", new vconst(vector3(0, 1, 0)));
//  new var<value>(varstack.front(),"YELLOW",new vconst(vector3(1,1,0)));
//  new var<value>(varstack.front(),"BLUE",new vconst(vector3(0,0,1)));
  new var<value> (varstack.front(), "FUCHSIA", new vconst(vector3(1, 0, 1)));
  new var<value> (varstack.front(), "AQUA", new vconst(vector3(0, 1, 1)));
//  new var<value>(varstack.front(),"WHITE",new vconst(vector3(1,1,1)));

// Unity-Transformation
  new var<value> (varstack.front(), "I", new tconst(Trafo()));

}

void ReadWorld(const string &fname)
{
  if (fname != "-")
    {
      yyin = fopen(fname.c_str(), "r");
    }
  else
    yyin = stdin;

  if (yyin == NULL) cerror("Kann Datei " + fname + " nicht öffnen");

  infilenames[IncludeStackPtr] = fname;

  initvars();

  light_number = 0;

  zeilennr = 1;
  if (cdebug & 1)
    yy_flex_debug = 1;
  else
    yy_flex_debug = 0;

  if (cdebug & 2)
    yydebug = 1;
  else
    yydebug = 0;
  yyparse();
}
