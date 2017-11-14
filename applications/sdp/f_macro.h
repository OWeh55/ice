#ifndef IM_MACRO_H
#define IM_MACRO_H

#include "f_filter.h"
#include "filterfactory.h"

class MacroCapsule: public Filter
{
  // filter that shall pass arguments to macros and
  // pass the outputs of the macro to the following filters
  // works like nop but with additional "knowledge" about macro
  //
public:
  MacroCapsule(int inputs, int outputs):
    Filter("macro", inputs + outputs, inputs + outputs, "encapsulation of macro expansions, only usable in compiler - "
           "<input1> <input2> ... => <output1> <output2> ..."), nInput(inputs), nOutput(outputs) {}

  virtual ~MacroCapsule()
  {
    // d'tor of Filter must not free *result[i], because the pointers are only
    // copies of the input pointers here
    for (int i = 0; i < (int)result.size(); ++i)
      result[i] = NULL;
  }

  CLONE(MacroCapsule);

  virtual void reset()
  {
    for (int i = 0; i < (int)result.size(); ++i)
      result[i] = NULL;
    Filter::reset();
  }

protected:
  int nInput, nOutput;

  virtual void get_data()
  {
    // first handle inputs
    for (int i = nOutput; i < nOutput + nInput; ++i)
      {
        if (input[i].inp != NULL)
          {
            // cout << name << " get " << i << endl;
            const GData *inp = getInputData(i);
            result[i] = const_cast<GData *>(inp);
          }
      }
    for (int i = 0; i < nOutput; ++i)
      {
        if (input[i].inp != NULL)
          {
            //  cout << name << " get " << i << endl;
            const GData *inp = getInputData(i);
            result[i] = const_cast<GData *>(inp);
          }
      }
  }

  virtual void graph_out(ostream &os, set<const Filter *> &fs) const
  {
    if (fs.count(this) == 0)
      {
        fs.insert(this);

        for (int i = 0; i < nInput; ++i)
          {
            if (input[i + nOutput].inp != NULL)
              {
                input[i + nOutput].inp->graph_out(os, fs);
              }
          }

        os << "F" << this << " [shape=record,label=\"{{";
        for (int i = 0; i < nInput; ++i)
          {
            if (input[i + nOutput].inp != NULL)
              {
                if (i > 0)
                  os << "|";
                os << "<i" << i << ">";
                //    if (input[i].inp==NULL)
                //      os << "*";
              }
          }
        os << "}|macro: " << getName() << "|{";
        for (int i = 0; i < nOutput; ++i)
          {
            if (i > 0)
              os << "|";
            os << "<o" << i << ">";
          }
        os << "}}\"]" << endl;

        for (int i = 0; i < nInput; ++i)
          {
            if (input[i + nOutput].inp != NULL)
              {
                os << "F" << input[i + nOutput].inp << ":o" << input[i + nOutput].idx << ":s";
                os << "->F" << this << ":i" << i << ":n" << endl;
              }
          }

        os << endl;
      }
  }
};
// FF(Macro)
#endif
