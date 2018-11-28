#ifndef IM_NOP_H
#define IM_NOP_H

#include "f_filter.h"
#include "filterfactory.h"

class Nop: public Filter
{
  static const int nChannels = 10;
  // filter that does nothing
  // all input streams are passed to the output
  //
public:
  Nop(): Filter("nop", nChannels, nChannels, "do nothing = copy input to output - "
                  " <input1> <input2> ... => <output1> <output2> ...") {}
  virtual ~Nop()
  {
    // d'tor of Filter must not free *result[i], because the pointers are only
    // copies of the input pointers here
    for (int i = 0; i < (int)result.size(); ++i)
      result[i] = NULL;
  }

  virtual void reset()
  {
    for (int i = 0; i < (int)result.size(); ++i)
      result[i] = NULL;
    Filter::reset();
  }

  CLONE(Nop);
protected:
  virtual void get_data()
  {
    const GData* inp;
    for (int i = 0; i < nChannels; ++i)
      {
        if (input[i].inp != NULL)
          {
            inp = getInputData(i);
            result[i] = const_cast<GData*>(inp);
          }
      }
  }

  virtual void graph_out(ostream& os, set<const Filter*>& fs) const
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

FF(Nop)
#endif
