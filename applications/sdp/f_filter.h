#ifndef IM_FILTER_H
#define IM_FILTER_H

#include <string>
#include <vector>
#include <set>

#include "exceptions.h"
#include "gdata.h"
#include "gnumber.h"
#include "genimage.h"
#include "gstring.h"

extern int fdebug;

class Filter;

class FilterOutlet
{
public:
  FilterOutlet(): inp(NULL), idx(0) {}
  FilterOutlet(Filter* src, int idx = 0): inp(src), idx(idx) {}
  Filter* inp;
  int idx;
};

/**
 * abstract base class for all filters
 * filters have:
 *    0..i inputs
 *    1..o outputs (result)
 */

#define CLONE(cn) virtual Filter* clone() const { return new cn(*this); }
#define DTOR(classname)   virtual ~classname() {}
#define FFUNC(classname) CLONE(classname) DTOR(classname)

#ifdef FILTERFACTORY
#define FF(classname) FilterFactory ff##classname(new classname());
#else
#define FF(classname)
#endif

class Filter
{
public:
  Filter(const std::string& name, int nInputs = 0, int nResults = 1, const std::string& help = ""):
    dataValid(false), name(name), input(nInputs), result(nResults, NULL), helpstring(help)
  {
    next = first;
    first = this;
  };

  virtual ~Filter()
  {
    for (int i = 0; i < (int)result.size(); ++i)
      delete result[i];
  }

  static void freeAll()
  {
    Filter* f = first->next;
    delete first;
    first = f;
  }

  virtual Filter* clone() const = 0;

  virtual void reset()
  {
    if (dataValid)
      {
        dataValid = false;
        for (int i = 0; i < (int)input.size(); ++i)
          if (input[i].inp != NULL)
            input[i].inp->reset();

        for (int i = 0; i < (int)result.size(); ++i)
          delete result[i];
      }
  }

  virtual std::vector<FilterOutlet>& getInput()
  {
    return input;
  }

  virtual void setInput(Filter* inp, int idx = 0, int i = 0)
  {
    if (i < 0 || i >= (int)input.size())
      throw SdpException(name, "Input index out of range");
    input[i] = FilterOutlet(inp, idx);

  }

  virtual void setInput(const FilterOutlet& inp, int i = 0)
  {
    if (i < 0 || i >= (int)input.size())
      throw SdpException(name, "Input index out of range");
    input[i] = inp;

  }

  virtual const GData* getData(int idx)
  {
    if (fdebug)
      {
        for (int i = 0; i < fdebug; ++i)
          cout << "  ";
        cout << "entering filter " << name << " " << dataValid << endl;
        fdebug++;
      }
    if (!dataValid)
      {
        dataValid = true;
        get_data();
        if (fdebug)
          {
            fdebug--;
            for (int i = 0; i < fdebug; ++i)
              cout << "  ";
            cout << "done filter " << name << endl;
          }
      }
    else if (fdebug)
      fdebug--;

    if (idx < 0 || idx >= (int)result.size())
      throw SdpException(name, "Result index out of range");
    return result[idx];
  }

  virtual std::string getName() const
  {
    return name;
  }

  virtual void setName(const std::string& n)
  {
    name = n;
  }

  virtual std::string help() const
  {
    std::string hs = name;
    if (!helpstring.empty())
      hs += " - " + helpstring;
    return hs;
  }

  virtual void graph_out(ostream& os, set<const Filter*>& fs) const;

protected:
  virtual const GData* getInputData(int i) const;
  virtual const GData* getInputData(int i, int type) const;
  virtual DType getInputType(int i) const;

  template<class T, int dtype>
  const T* getInputPointer(int inputNr = 0) const
  {
    const GData* src = getInputData(inputNr, dtype);
    return dynamic_cast<const T*>(src);
  }

  template<class Tr, class T, int dtype>
  const Tr& getInput(int inputNr = 0, int idx = 0) const
  {
    const GData* src = getInputData(inputNr, dtype);
    return (*dynamic_cast<const T*>(src))[idx];
  }

  virtual const GImage* getInputImage(int i) const;
  virtual const int getInputInt(int i, int def) const;
  virtual const int getInputInt(int i) const;
  virtual const double getInputFloat(int i) const;
  virtual const double getInputFloat(int i, double def) const;
  virtual const string& getInputString(int i) const;
  virtual const string& getInputString(int i, const string& def) const;

  virtual void get_data() = 0;

  bool dataValid;
  std::string name;
  std::vector<FilterOutlet> input;
  std::vector<GData*> result;
  std::string helpstring;
  Filter* next;
  static Filter* first;
};
#endif
