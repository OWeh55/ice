#include "f_filter.h"

#include "filterfactory.h"

#include "makefilter.h"

Filter* mkFilter(const std::string& filter)
{
  if (filter.empty())
    {
      ff.printFilters();
      return NULL;
    }
  else
    return ff.mkFilter(filter);
}

