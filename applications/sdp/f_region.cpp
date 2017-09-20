#include "gregion.h"
#include "gcontur.h"

#include "f_region.h"

void Contur2Region::get_data()
{
  const GContur *cont = dynamic_cast<const GContur *>(getInputData(0, DType::contur));
  GRegion *tresult = new GRegion;
  result[0] = tresult;

  for (int i = 0; i < cont->getSize(); ++i)
    {
      const Contur &cc = (*cont)[i];
      Region res(cc);
      tresult->push_back(res);
    }
};
