#ifndef SDP_SDP_H
#define SDP_SDP_H

#include <vector>
#include <deque>
#include <map>
#include <string>

#include "f_filter.h"
#include "f_dial.h"

typedef std::map<std::string, FilterOutlet> OutletMap; // named filter outlets
typedef std::deque<std::string> FilterDescription;

struct Macro
{
  std::vector<std::string> inputlabels;
  std::vector<std::string> outputlabels;
  FilterDescription fd;
  void clear()
  {
    inputlabels.clear();
    outputlabels.clear();
    fd.clear();
  }
};

typedef std::map<std::string, Macro> Library;

// predefined macros
extern Library macros;

// interactive elements
extern std::vector<Dial*> dials;

// where to find includes ...
extern std::vector<std::string> pathlist;
#endif
