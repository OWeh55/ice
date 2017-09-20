/* Lexikalische Analyse für cviewer */

/* zustand zur erkennung von include-filenamen */
%x INCLUDEFILENAME
 /* zustand zur Erkennung von zeichenketten in ""*/
%x STRINGX

%{
#include <stdio.h>
#include <string.h>
#include "cviewer.h"
#include "parserb.hpp"
#include "parsdefs.h"

#define YY_USER_ACTION zeilenstring=yytext;
#define YY_FATAL_ERROR fatal_lex_error

  int zeilennr=0;  /* enthaelt die aktuelle, gelesene Zeile */
  string zeilenstring;
  int eofs=0;

  int IncludeStackPtr=0;
  YY_BUFFER_STATE IncludeStack[MAXINCLUDE];
  string infilenames[MAXINCLUDE];
  char zeilennrstack[MAXINCLUDE];
  string urstring, hstring;
 
  /* Dummy-Funktion um Warnung "defined but not used" zu unterdruecken */
  void lexdummy() { yy_fatal_error(""); }

  /* fuer die Include-Verwaltung */
  /* include */
  /*
    void Include1()
    {  
    if (IncludeStackPtr >= MAXINCLUDE) 
    cerror("Include-Stack-Overflow");
    BEGIN(INCLUDEFILENAME);
    }
  */
  /* Merken des Include-Namens */
  void Include2()
  {
    FILE *file;
    string h=yytext;
    h = Trim(h);
    //      cout << "-" << h << "-" << endl;
    infilenames[IncludeStackPtr+1] = h.c_str();
    /* umschalten auf include file */
    // Include-File oeffnen
    file = fopen(infilenames[IncludeStackPtr+1].c_str(),"r");
    if (file == NULL) 
      {
	cerror("Can't open file " + infilenames[IncludeStackPtr+1]);
      }
    yyin = file;
    /* alten Status sichern */
    IncludeStack[IncludeStackPtr] = YY_CURRENT_BUFFER;
    zeilennrstack[IncludeStackPtr] = zeilennr;
    zeilennr = 1;
    ++IncludeStackPtr;
    /* neues file anmelden */
    yy_switch_to_buffer((yy_create_buffer(yyin,YY_BUF_SIZE)));
    BEGIN(0);
  }

  int yywrap()
  {
    return 1;
  }

#define  YY_SKIP_YYWRAP 1
%}

WHITESPACE [ \t]
%%
 
include {
  if (IncludeStackPtr>=MAXINCLUDE) 
    cerror("Include-Stack-Overflow");
  BEGIN(INCLUDEFILENAME);
}

<INCLUDEFILENAME>[ \t]*      /* eat the whitespace */
<INCLUDEFILENAME>[^ \t\n]+   { /* got the include file name */
        Include2();
	return NEWLINE;
}

<<EOF>> { if (--IncludeStackPtr<0)
      {  
	IncludeStackPtr=0;
	if (eofs>0) yyterminate();
	eofs++;
	return NEWLINE;
      }
  else
    {  
      fclose(yyin);
      yy_switch_to_buffer(IncludeStack[IncludeStackPtr]);
      zeilennr=zeilennrstack[IncludeStackPtr];
      return NEWLINE;
    }
  }

\" { BEGIN(STRINGX); }

<STRINGX>[^\"]+ { return STRING; }

<STRINGX>\" { BEGIN(0); }

{WHITESPACE}+   {  }

;        { return(';'); }

\n { zeilennr++;return NEWLINE; }

("//")([^\n]*) { if (yytext[2]=='/') cout << yytext << endl; /* KOMMENTAR;*/ } 

^"#"([^\n]*) { /* KOMMENTAR; */ }

[\:\^\+\-\*\/=\<\>,\(\)\[\]\{\}@\$\\\.\#\~\|!\%\?]   {
    return((int)(yytext[0])); }

\>\= { return GREATEREQUAL; }
\<\= { return LESSEQUAL; }
\=\= { return EQUAL; }
\<\> { return NOTEQUAL; } 

and { return AND; }
or { return OR; }

\'  { return HKOMMA; }
´   { return HKOMMA; }

[0-9]+   { return ZAHL; }

[0-9]*"."?[0-9]* { return FZAHL; }
[0-9]*"."?[0-9]*[eE][0-9]+ { return FZAHL; }
[0-9]*"."?[0-9]*[eE]"+"[0-9]+ { return FZAHL; }
[0-9]*"."?[0-9]*[eE]"-"[0-9]+ { return FZAHL; }

:= { return ASSIGN; }

def { return DEF; }
enddef { return ENDDEF; }
global { return GLOBAL; }

begin { return BEGIN_; }
end { return END_; }

for { return FOR; }
endfor { return ENDFOR; }
to { return TO; }
step { return STEP; }

if { return IF; }
then { return THEN; }
else { return ELSE; }
endif { return ENDIF; }

point { return POINT; }

sphere { return SPHERE; }
cylinder { return CYLINDER; }
box { return BOX; }

color { return COLOR; }

light { return LIGHT;}

spline { return SPLINE; }
endspline { return ENDSPLINE; }

linear { return LINEAR; }
cubic { return CUBIC; }

polygon { return POLYGON; }
endpolygon { return ENDPOLYGON; }
triangle { return TRIANGLE; }
inverse { return INVERSE; }
heightfield { return HEIGHTFIELD; }

[A-Za-z_]([A-Za-z0-9_@])*  { return NAME; }

.       { return UNKNOWN; }

%%

extern char yychar;

void UnputChar(char c)
{
/*printf("Unput %c - yychar:%c = %d\n YYEMPTY:%d",c,yychar,yychar,YYEMPTY);*/
  unput(c);
}

