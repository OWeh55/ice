// globale Variablen, Definitionen und Prototypen fuer den Parser
#ifndef PARSDEFS_H
#define PARSDEFS_H

#include <stdio.h>

#include "cviewer.h"

#ifdef WIN32
#include <io.h>
#define isatty _isatty
#endif

#define MAXPARAM 30      /* maximale Anzahl Parameter bei Prozeduraufrufen */
#define MAXINCLUDE 10    /* maximale Verschachtelung mit include */
#define MAXPROC 100      /* maximale Zahl von Prozedur-Definitionen im File */
#define MAXSBR 30
/*####################################################################*/

extern FILE* yyin;        /* Quell-File */
extern char* yytext;      /* zuletzt gelesene Zeichen */

extern FILE* OutFile;     /* Ausgabefile */
extern string infilenames[MAXINCLUDE];
extern int IncludeStackPtr;

extern string urstring;
extern string zeilenstring;
extern int zeilennr;
extern int yy_flex_debug;

int yytest(void);
int yyerror(const char*);
int yyparse(void);

void fatal_lex_error(const char msg[]);

vect*  Vect(value* p1, value* p2, value* p3);
value* Neg(value*);
value* Produkt(value*, value*);
value* Concat(value*, value*);
value* Quotient(value*, value*);
value* Mod(value*, value*);
value* Summe(value*, value*);
value* Differenz(value*, value*);

value* And(value*, value*);
value* Or(value*, value*);

value* LessThen(value*, value*);
value* LessEqual(value*, value*);
value* GreaterThen(value*, value*);
value* GreaterEqual(value*, value*);
value* Equal(value*, value*);
value* NotEqual(value*, value*);

value* Conditional(value*, value*, value*);

value* Funktion(const string& fn, value*);
value* Funktion(const string& fn, value*, value*);
value* Funktion(const string& fn, value*, value*, value*);

void Color(value*);

void Point(value*);
void Sphere(value*, value*);
void Cylinder(value*, value*, value*);
void Box(value*, value*);
void Triangle(value*, value*, value*);

void HeightField(const string& name, value*, value*, value*, value*);
void HeightField(const string& name, value*, value*, value*);
void HeightField(const string& name, value*);

void Spline(const string& name, vectlist* v, int mode);
value* SplineWert(const string& fn, value*);

void Light(value*, value*);
void Light(value*);

void UserObjekt(const string& name, value*, value*, value*);
void UserObjekt(const string& name, value*);

void For3(const string& name, value* from, value* to, value* step);
void For2(const string& name, value* from, value* to);
void For1(const string& name);

value* If1(value* cond);
objekt* If2(void);
objekt* If3(void);
void If(value* cond, objekt* o1, objekt* o2);

value* Punktliste(value* p1);
value* Punktliste(vectlist* liste, value* p);

void Polygon(vectlist* v, bool inverse);

void EnterLocal();
void LeaveLocal();

void BeginObjekt(const string& name);
void EndObjekt(const string& name, int global);

void PushWorld(void);
void PopWorld(void);
#endif
