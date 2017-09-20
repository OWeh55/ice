#ifndef VARIABLEN_H
#define VARIABLEN_H

#include <vector>
#include "arith.h"

template <class T>
class var
{
public:
  var(var<T> *&first, const string &pname, T *pval):
    name(pname), hidden(false), val(pval), next(first)
  {
    var<T> *varp = first;
    while (varp != NULL)
      {
        if (varp->name == pname)
          cerror("Name " + pname +  " existiert schon");
        varp = varp->next;
      }
    first = this;
  }

  T *find(const string &pname)
  {
    if ((name == pname) && (!hidden)) return val;
    if (next == NULL) return NULL;
    return next->find(pname);
  }

  string find(T *v)
  {
    if (val == v) return name;
    if (next == NULL) return "?";
    return next->find(v);
  }

  void Delete(void)
  {
    hidden = true;
  }

  void Delete(const string &pname)
  {
    if (name == pname)
      {
        hidden = true;
        return;
      }
    if (next == NULL)
      return;
    next->Delete(pname);
  }

  void Hide(var<T> *bis)
  {
    if (next != bis)
      {
        next->Hide(bis);
      }
    Delete();
  }

protected:
  var() {};
  var(const var &) {};
  string name;
  bool hidden;
  T *val;
  var *next;
};

template < class T >
T *find(const vector<var<T>* > stack, const string &pname)
{
  T *res = NULL;
  int i = stack.size() - 1;
  while ((res == NULL) && (i >= 0))
    {
      if (stack[i] != NULL)
        res = stack[i]->find(pname);
      i--;
    }
  return res;
}

extern vector<var<value>* > varstack;
extern vector<var<objekt>* > ostack;

void NewNamespace();
void LeaveNamespace();

#endif
