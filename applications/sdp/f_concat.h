#ifndef SDP_CONCAT_H
#define SDP_CONCAT_H

#include "f_filter.h"

class Concat: public Filter
{
public:
  Concat(): Filter("concat", 10, 1, "concatenate strings -"
                     "<string1> <string2> ... => <string>") {}

  FFUNC(Concat);
protected:
  virtual void get_data()
  {
    GString *tresult = new GString("");
    result[0] = tresult;
    int idx = 0;
    while (getInputType(idx).type == DType::string_type)
      {
        (*tresult)[0] += getInputString(idx);
        ++idx;
      }
  }

  virtual void graph_out(ostream &os, set<const Filter *> &fs) const
  {
    if (fs.count(this) == 0)
      {
        fs.insert(this);
        int lastinput = 0;
        for (int i = 0; i < (int)input.size(); ++i)
          {
            if (input[i].inp != NULL)
              {
                input[i].inp->graph_out(os, fs);
                lastinput = i;
              }
          }

        os << "F" << this << " [shape=record,label=\"{{";
        for (int i = 0; i <= lastinput; ++i)
          {
            if (input[i].inp != NULL)
              {
                if (i > 0)
                  os << "|";
                os << "<i" << i << ">";
                //    if (input[i].inp==NULL)
                //      os << "*";
              }
          }
        os << "}|" << getName() << "|{";
        for (int i = 0; i <= lastinput; ++i)
          {
            if (i > 0)
              os << "|";
            os << "<o" << i << ">";
          }
        os << "}}\"]" << endl;

        for (int i = 0; i < (int)input.size(); ++i)
          {
            if (input[i].inp != NULL)
              {
                os << "F" << input[i].inp << ":o" << input[i].idx << ":s";
                os << "->F" << this << ":i" << i << ":n" << endl;
              }
          }

        os << endl;
      }
  }


};
FF(Concat);
#endif
