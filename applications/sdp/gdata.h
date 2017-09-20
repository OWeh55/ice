#ifndef GEN_DATA_H
#define GEN_DATA_H

#include <vector>
#include <string>

#include "exceptions.h"

#include "gdtype.h"

// base class generic data

class GData
{
public:
  virtual DType getType() const = 0;
  virtual int getSize() const = 0;

  virtual GData *clone() const = 0;

  virtual void checkIndex(int i) const
  {
    if (i < 0 || i >= getSize())
      {
        throw SdpException("GData", "Element index out of range");
      }
  }

  virtual ~GData() {};
};
#endif
