#ifndef SDP_FILTER_FACTORY_H
#define SDP_FILTER_FACTORY_H

#include <vector>
#include <string>

#include "f_filter.h"

class FilterFactory
{
public:
  FilterFactory() {}

  FilterFactory(const Filter *f);

  void printFilters() const;

  Filter *mkFilter(const string &name);

private:
  static std::vector<const Filter *> fmap;
};

extern FilterFactory ff;
#endif
