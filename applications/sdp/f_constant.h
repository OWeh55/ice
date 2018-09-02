#ifndef SDP_F_CONSTANT_H
#define SDP_F_CONSTANT_H

#include "gdata.h"
#include "gnumber.h"
#include "gstring.h"

#include "f_filter.h"

class ConstantFilter: public Filter
{
public:
  ConstantFilter(const GData* val): Filter("constant", 0, 1)
  {
    result[0] = const_cast<GData*>(val);
    dataValid = true;
  }

  ConstantFilter(int val): Filter("constant", 0, 1)
  {
    valuestring = to_string(val);
    result[0] = dynamic_cast<GData*>(new GInteger(val));
    dataValid = true;
  }

  ConstantFilter(vector<int> val): Filter("constant", 0, 1)
  {
    valuestring = "vector<int>";
    result[0] = dynamic_cast<GData*>(new GInteger(val));
    dataValid = true;
  }

  ConstantFilter(double val): Filter("constant", 0, 1)
  {
    valuestring = to_string(val);
    result[0] = dynamic_cast<GData*>(new GFloat(val));
    dataValid = true;
  }

  ConstantFilter(vector<double> val): Filter("constant", 0, 1)
  {
    valuestring = "vector<double>";
    result[0] = dynamic_cast<GData*>(new GFloat(val));
    dataValid = true;
  }

  ConstantFilter(const string& val): Filter("constant", 0, 1)
  {
    valuestring = val;
    result[0] = dynamic_cast<GData*>(new GString(val));
    dataValid = true;
  }

  ConstantFilter(const vector<string>& val): Filter("constant", 0, 1)
  {
    valuestring = "vector<string>";
    result[0] = dynamic_cast<GData*>(new GString(val));
    dataValid = true;
  }

  virtual void reset() {}

  FFUNC(ConstantFilter)

  static string DotEscape(const string& s)
  {
    string result;
    for (int i = 0; i < (int)s.size(); ++i)
      {
        if (s[i] == '|')
          result += "\\|";
        else if (s[i] == '"')
          result += "\\\"";
        else
          result += s[i];
      }
    return result;
  }

  virtual void graph_out(ostream& os, set<const Filter*>& fs) const
  {
    if (fs.count(this) == 0)
      {
        fs.insert(this);
        os << "F" << this << " [shape=record label=\"{{<o0>";
        if (result[0]->getType().type == DType::string_type)
          os << DotEscape('"' + valuestring + '"');
        else
          os << DotEscape(valuestring);
        os << "}}\"]" << endl;
      }
  }

protected:
  std::string valuestring;

  virtual void get_data() {}
};

#endif
