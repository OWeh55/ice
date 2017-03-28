#ifndef WXPICIO_H
#define WXPICIO_H

#include "base.h"

namespace  ice
{
  bool WXReadImg(const std::string& fname, Image& img);
  bool WXReadImg(const std::string& fname, Image& ir, Image& ig, Image& ib);
}

#endif
