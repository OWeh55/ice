/* 
 * Syntaxdefinitionen
 */

/* Operatoren */

%left '?' ':'

%left OR
%left AND

%left LESSEQUAL
%left '<'
%left GREATEREQUAL
%left '>'
%left EQUAL
%left NOTEQUAL

%left '+' '-' 
%left '*' '/' '%' '#'
%left NEGATION

%token NEWLINE
%token ';'

%token '^' 
%token '='
%token ASSIGN
%token '<'
%token '>'
%token ','
%token '('
%token ')'
%token '['
%token ']'
%token '{'
%token '}'
%token '@'
%token '$'
%token '"'
%token '?'
%token ':'

%token HKOMMA

%token DEF
%token ENDDEF
%token GLOBAL

%token BEGIN_
%token END_

%token FOR
%token ENDFOR
%token TO
%token STEP

%token IF
%token THEN
%token ELSE
%token ENDIF

%token NAME
%token STRING
%token ZAHL
%token FZAHL

%token COLOR

%token SPLINE 
%token ENDSPLINE
%token LINEAR
%token CUBIC

%token POINT
%token SPHERE
%token CYLINDER
%token BOX
%token TRIANGLE

%token POLYGON
%token INVERSE
%token ENDPOLYGON

%token HEIGHTFIELD

%token LIGHT

%token UNKNOWN

%start Programm

%{ 
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "objekte.h"
#include "cviewer.h"
#include "parsdefs.h"
#include "variablen.h"
  
  typedef struct ytyp
  {
    int i;
    double d;
    value *w;
    objekt *o;
    string bez;
  } 
  yystype;
    
#define YYERROR_VERBOSE 1
    
#define YYDEBUG 1

#define YYSTYPE yystype
#define IFE(x) x; if (ErrorFlag>0) YYERROR;
  %}

%%

Programm: NewlinesOpt Anweisungen NewlinesOpt
	;

Anweisungen: Anweisung 
           | Anweisungen Newlines Anweisung ;

Anweisung: Definition |
           Zuweisung |
           Control |
           Light |
           Objekt |
	   For |
           If |
           BEGIN_ { EnterLocal(); } 
              Newlines Anweisungen Newlines
           END_ { LeaveLocal(); } 
;

Definition: WertDefinition |
            SplineDefinition |
            ObjectDefinition ;

Control:    COLOR Ausdruck { Color($2.w); }
;

WertDefinition: Name '=' Ausdruck { 
  new var<value>(varstack.back(),$1.bez,$3.w); 
  // cout << $1.bez << "=" << $3.w->String() << endl;
}
;

Zuweisung: Name ASSIGN Ausdruck { 
  cout << "Noch nicht implementiert: " << $1.bez << " := " << $3.w->String() << endl;
}
;

SplineDefinition: SPLINE Name SplineRumpf { Spline($2.bez,(vectlist*)$3.w,1); }
              |   SPLINE Name LINEAR SplineRumpf { Spline($2.bez,(vectlist*)$4.w,1); }
              |   SPLINE Name CUBIC SplineRumpf { Spline($2.bez,(vectlist*)$4.w,3); }
;

SplineRumpf: Newlines Punktliste Newlines ENDSPLINE { $$.w=$2.w; } ;

If:     IfHead IfThen IfElse { If($1.w,$2.o,$3.o);} 
;

IfHead:  IF Ausdruck THEN Newlines { $$.w=If1($2.w); } 
;

IfThen:   Anweisungen Newlines { $$.o=If2(); } 
; 

IfElse:  ENDIF { If3(); $$.o=NULL; } 
        | ELSE Newlines Anweisungen Newlines ENDIF { $$.o=If3(); }
;

For:    For1 | For2 ;

For1:	FOR Name '=' Ausdruck TO Ausdruck Newlines { For1($2.bez); }
	Anweisungen	
	Newlines
	ENDFOR { For2($2.bez,$4.w,$6.w);}
;

For2:	FOR Name '=' Ausdruck TO Ausdruck STEP Ausdruck Newlines { For1($2.bez); }
	Anweisungen
	Newlines
	ENDFOR { For3($2.bez,$4.w,$6.w,$8.w);}
;

ObjectDefinition: DEF Name Global Newlines { BeginObjekt($2.bez); }
		  Anweisungen Newlines
		  ENDDEF { EndObjekt($2.bez,$3.i); }

Global: /* nichts*/ { $$.i=0; }
        | GLOBAL { $$.i=1; }

Objekt: SPHERE Ausdruck ',' Ausdruck { Sphere($2.w,$4.w); }
        | CYLINDER Ausdruck ',' Ausdruck ',' Ausdruck {
	  Cylinder($2.w,$4.w,$6.w); }
        | BOX Ausdruck ',' Ausdruck { Box($2.w,$4.w); }
        | TRIANGLE Ausdruck ',' Ausdruck ',' Ausdruck { Triangle($2.w,$4.w,$6.w); }
        | HEIGHTFIELD String ',' Ausdruck ',' Ausdruck ',' Ausdruck ',' Ausdruck { HeightField($2.bez,$4.w,$6.w,$8.w,$10.w); }
        | HEIGHTFIELD String ',' Ausdruck ',' Ausdruck ',' Ausdruck { HeightField($2.bez,$4.w,$6.w,$8.w); }
        | HEIGHTFIELD String ',' Ausdruck { HeightField($2.bez,$4.w); }
        | POINT Ausdruck { Point($2.w); }
	| Polygon
	| Name Ausdruck ',' Ausdruck ',' Ausdruck { 
	  UserObjekt($1.bez,$2.w,$4.w,$6.w); }
	| Name Ausdruck { 
	  UserObjekt($1.bez,$2.w); }
;

Light:   LIGHT Ausdruck ',' Ausdruck { Light($2.w,$4.w); }
        | LIGHT Ausdruck { Light($2.w); }
;

Polygon: POLYGON InverseOpt Newlines Punktliste
	 Newlines ENDPOLYGON { Polygon((vectlist*)$4.w,$2.i); }
;

InverseOpt: INVERSE { $$.i=1; }
         | { $$.i=0; } /* nothing */
;

Punktliste: Punktliste Trenner Ausdruck { $$.w=Punktliste((vectlist*)$1.w,$3.w); } 
          | Ausdruck { $$.w=Punktliste($1.w); }
;

Trenner:   Newlines
          |  ','
;

Newlines: NEWLINE
        | Newlines NEWLINE
;

NewlinesOpt: /*nothing*/ 
        | Newlines 
;

Ausdruck: Zahl { $$.w=$1.w; }
           | Variable { $$.w=$1.w; }
	   | Funktion { $$.w=$1.w; }
           | Ausdruck '#' Ausdruck { $$.w=Concat($1.w,$3.w); }
           | Ausdruck '?' Ausdruck ':' Ausdruck { $$.w=Conditional($1.w,$3.w,$5.w); }
	   | SplineWert { $$.w=$1.w; }
	   | '(' Ausdruck ')' { $$.w=$2.w;}
	   | '-' Ausdruck { $$.w=Neg($2.w); }
	   | Ausdruck '*' Ausdruck { $$.w=Produkt($1.w,$3.w); }
	   | Ausdruck '/' Ausdruck { $$.w=Quotient($1.w,$3.w); }
	   | Ausdruck '%' Ausdruck { $$.w=Mod($1.w,$3.w); }
	   | Ausdruck '+' Ausdruck { $$.w=Summe($1.w,$3.w); }
	   | Ausdruck '-' Ausdruck { $$.w=Differenz($1.w,$3.w); }
           | Ausdruck '<' Ausdruck { $$.w=LessThen($1.w,$3.w); }
           | Ausdruck LESSEQUAL Ausdruck { $$.w=LessEqual($1.w,$3.w); }
           | Ausdruck '>' Ausdruck { $$.w=GreaterThen($1.w,$3.w); }
           | Ausdruck GREATEREQUAL Ausdruck { $$.w=GreaterEqual($1.w,$3.w); }
           | Ausdruck EQUAL Ausdruck { $$.w=Equal($1.w,$3.w); }
           | Ausdruck NOTEQUAL Ausdruck { $$.w=NotEqual($1.w,$3.w); }
	   | Ausdruck AND Ausdruck { $$.w=And($1.w,$3.w); }
	   | Ausdruck OR Ausdruck { $$.w=Or($1.w,$3.w); }
           | '<' Ausdruck ',' Ausdruck ',' Ausdruck '>' { 
	     $$.w=Vect($2.w,$4.w,$6.w); }
;

Variable: Name {
  value *v=find(varstack,$1.bez);
  if (v==NULL) cerror(string("Variable existiert nicht: ")+$1.bez);
  switch (v->Type())
    {
    case TypScalar: $$.w=new namednumber((number*)v); break;
    case TypVector: $$.w=new namedvect((vect*)v); break;
    case TypTrafo: $$.w=new namedtrafo((trafo*)v); break;
    default: cerror(string("Falscher Variablentyp: ")+$1.bez);break;
    }
  }
;

SplineWert: Name '[' Ausdruck ']' { $$.w=SplineWert($1.bez,$3.w); }
;

Funktion: Name '(' Ausdruck ')' { $$.w=Funktion($1.bez,$3.w); }
	  |  Name '(' Ausdruck ',' Ausdruck ')' { 
	    $$.w=Funktion($1.bez,$3.w,$5.w);
	  }
	  |  Name '(' Ausdruck ',' Ausdruck ',' Ausdruck ')' 
	    {  $$.w=Funktion($1.bez,$3.w,$5.w,$7.w);  }
;

Zahl: FZAHL  { $$.w=new nconst(atof(yytext)); }
      | ZAHL { $$.w=new nconst(atol(yytext)); }
;

Name:	NAME { $$.bez=yytext; }

String: STRING { $$.bez=yytext; }

%%
