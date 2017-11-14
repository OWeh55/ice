#include "cviewer.h"
#include "variablen.h"

var<value> *vari = NULL;
var<objekt> *ovar = NULL;

vector<var<value> *> varstack;
vector<var<objekt> *> ostack;

void NewNamespace()
{
  ostack.push_back(NULL);
  varstack.push_back(NULL);
}

void LeaveNamespace()
{
  ostack.pop_back();
  varstack.pop_back();
}
