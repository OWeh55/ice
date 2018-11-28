#include "string.h"

#define FILTERFACTORY

#include "filterfactory.h"

std::vector<const Filter*> FilterFactory::fmap;
FilterFactory ff;

#include "f_info.h"
#include "f_dial.h"
#include "f_readimg.h"
#include "f_display.h"
#include "f_writeimg.h"
#include "f_invers.h"
#include "f_convert.h"
#include "f_rgb.h"
#include "f_sidebyside.h"
#include "f_stack.h"
#include "f_nop.h"
#include "f_generate.h"
#include "f_fork.h"

#include "f_max.h"
#include "f_binary.h"
#include "f_mask.h"

#include "f_clip.h"
#include "f_pad.h"
#include "f_resize.h"
#include "f_flip.h"

#include "f_fourier.h"
#include "f_add.h"
#include "f_sub.h"
#include "f_smear.h"
#include "f_grad.h"
#include "f_direction.h"
#include "f_dob.h"
#include "f_morphology.h"
#include "f_distance.h"
#include "f_polynomfilter.h"
#include "f_compare.h"

#include "f_osmear.h"
#include "f_odob.h"
#include "f_skeleton.h"
#include "f_normalize.h"

#include "f_pointset.h"
#include "f_draw.h"
#include "f_delaunay.h"

#include "f_contur.h"
#include "f_cfilter.h"
#include "f_region.h"

#include "f_histogram.h"

#include "f_concat.h"
#include "f_eval.h"

FilterFactory::FilterFactory(const Filter* f)
{
  fmap.push_back(f);
}

void FilterFactory::printFilters() const
{
  vector<string> filterlist;
  for (int i = 0; i < (int)fmap.size(); ++i)
    filterlist.push_back(fmap[i]->help());

  sort(filterlist.begin(), filterlist.end());

  for (int i = 0; i < (int)filterlist.size(); ++i)
    cout << filterlist[i] << endl;
}

Filter* FilterFactory::mkFilter(const string& name)
{
  const Filter* foundfilter = NULL;
  for (int i = 0; (foundfilter == NULL) && i < (int)fmap.size(); ++i)
    {
      string fname = fmap[i]->getName();
      if (stringMatch(name, fname))
        foundfilter = fmap[i];
    }
  if (foundfilter == NULL)
    throw SdpException("FilterFactory", "Unknown Filter " + name);

  return foundfilter->clone();
}

