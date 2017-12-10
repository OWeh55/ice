#include <iostream>

#include "defs.h"

#include "objekte.h"
#include "numbase.h"

using namespace std;

void color::myDraw() const
{
  vector3 apos = pos->Value();
  glColor3f(apos.X(), apos.Y(), apos.Z());
}

ocall::ocall(objekt* pob, vect* pshift, number* ang, vect* prot):
  objekt(pshift)
{
  rot = NULL;
  angle = NULL;
  setv(rot, prot);
  setn(angle, ang);
  ob = pob;
}

void ocall::myDraw() const
{
  float ang = angle->Value();
  vector3 apos = pos->Value();
  vector3 arot = rot->Value();
  glPushMatrix();
  glTranslatef(apos.X(), apos.Y(), apos.Z());
  glRotatef(ang, arot.X(), arot.Y(), arot.Z());

  if (ob != NULL) ob->Draw();

  glPopMatrix();
}

ocalltrafo::ocalltrafo(objekt* pob, trafo* ptrafo): objekt()
{
  tr = NULL;
  settr(tr, ptrafo);
  ob = pob;
}

void ocalltrafo::myDraw() const
{
  glPushMatrix();

  Trafo trc;
  trc.glGet();

  //  cout << trc.toString() << endl << "A-------------" << endl;

  Trafo trn = tr->Value();
  // cout << trn.toString() << endl << "M-------------" << endl;

  trn.Append(trc);
  trn.glSet();

  // cout << trn.toString() << endl << "E--------------" << endl;

  // trn.glGet();
  // cout << trn.toString() << endl << "reread---------" << endl;

  if (ob != NULL) ob->Draw();

  glPopMatrix();
}

void CondObject::myDraw() const
{
  if (condition->Value())
    {
      if (thenobject != NULL)
        thenobject->Draw();
    }
  else
    {
      if (elseobject != NULL)
        elseobject->Draw();
    }
}

void Append(objekt * (&ptr), objekt* ob)
{
  if (ptr == NULL)
    {
      ptr = ob;
      ob->next = ob;
      ob->prev = ob;
    }
  else
    {
      objekt* last = ptr->prev;
      last->next = ob;
      ob->prev = last;
      ob->next = ptr;
      ptr->prev = ob;
    }
}
//========================================================================

void Optimize(objekt*& ob)
{
  if (ob != NULL)
    {
      objekt* lasto = ob->prev;
      objekt* aob = ob;
      if (aob == lasto)
        aob->MyOptimize();
      else
        {
          while (lasto != aob)
            {
              aob->MyOptimize();
              aob = aob->next;
            }
          lasto->MyOptimize();
        }
    }
}
