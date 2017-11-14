#ifndef SDP_C_FILTER_H
#define SDP_C_FILTER_H

#include "f_filter.h"
#include "gcontur.h"

class ConturFilter: public Filter
{
public:
  ConturFilter(): Filter("filtercontur", 4, 1, "filter conturset - "
                           "<conturlist> <criteria> <min> <max> => <conturlist>")
  {}

  FFUNC(ConturFilter);

protected:
  virtual void get_data()
  {
    const GContur *src = dynamic_cast<const GContur *>(getInputData(0, DType::contur));

    GContur *tresult = new GContur;
    result[0] = tresult;

    int criteria = getInputInt(1);

    if (criteria > 2)
      {
        double min = getInputFloat(2);
        double max = getInputFloat(3);


        for (int i = 0; i < src->getSize(); ++i)
          {
            const Contur &c = (*src)[i];

            double area, convexity, length, formfactor;

            if (c.isClosed())
              {
                FeatureContur(c, length, area, formfactor, convexity);
                switch (criteria)
                  {
                  case 3:
                    if (c.isClosed())
                      if (area >= min && area <= max)
                        (*tresult).push_back(c);
                    break;
                  case 4:
                    if (c.isClosed())
                      if (length >= min && length <= max)
                        (*tresult).push_back(c);
                    break;
                  case 5:
                    if (c.isClosed())
                      if (formfactor >= min && formfactor <= max)
                        (*tresult).push_back(c);
                    break;
                  }
              }
          }
      }
    else
      {
        for (int i = 0; i < src->getSize(); ++i)
          {
            const Contur &c = (*src)[i];
            // criteria < 3
            switch (criteria)
              {
              case 0:
                // open
                if (!c.isClosed())
                  (*tresult).push_back(c);
                break;
              case 1:
                // object
                if (c.isClosed() && !c.isHole())
                  (*tresult).push_back(c);
                break;
              case 2:
                // object
                if (c.isClosed() && c.isHole())
                  (*tresult).push_back(c);
                break;
              }
          }
      }
  }
};

FF(ConturFilter);
#endif
