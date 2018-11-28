#include "exceptions.h"

#include "stackmachine.h"

#include "f_eval.h"

void Evaluate::get_data()
{
  string expression = getInputString(0);
  StackMachine sm(expression);

  vector<DType> dts;
  dts.push_back(getInputType(1));
  vector<const GData*> gdp;
  gdp.push_back(getInputData(1));

  if (dts[0].type == DType::integer)
    {
      int i = 2; // remaining inputs
      DType dt;
      do
        {
          dt = getInputType(i);
          if (dt.type != DType::unknown)
            {
              if (dt.type != DType::integer)
                throw SdpException("eval", "expected integer parameter");
              dts.push_back(dt);
              gdp.push_back(getInputData(i));
              ++i;
            }
        }
      while (i < (int)input.size() && dt.type != DType::unknown);

      GInteger* tresult = dynamic_cast<GInteger*>(gdp[0]->clone());
      result[0] = tresult;

      vector<int> values(dts.size(), 0);
      vector<int> index(dts.size(), 0);
      // src1 is controlling source
      for (int i = 0; i < gdp[0]->getSize(); ++i)
        {
          for (unsigned int k = 0; k < dts.size(); ++k)
            {
              const GInteger* ip = dynamic_cast<const GInteger*>(gdp[k]);
              values[k] = (*ip)[index[k]];
              ++index[k];
              if (index[k] >= ip->getSize())
                index[k] = 0;
            }
          (*tresult)[i] = sm.evaluate(values, 0, 0, i);
        }
    }
  else if (dts[0].type == DType::image)
    {
      int i = 2; // remaining inputs
      DType dt;
      do
        {
          dt = getInputType(i);
          if (dt.type != DType::unknown)
            {
              if (dt.type == DType::integer)
                {
                  dts.push_back(dt);
                  gdp.push_back(getInputData(i));
                  ++i;
                }
              else if (dt.type == DType::image)
                {
                  dts.push_back(dt);
                  gdp.push_back(getInputData(i));
                  // add size test !!!!!!!!!!!!!
                  ++i;
                }
              else
                throw SdpException("eval", "expected integer or image parameter");
            }
        }
      while (i < (int)input.size() && dt.type != DType::unknown);

      GImage* tresult = dynamic_cast<GImage*>(gdp[0]->clone());
      result[0] = tresult;

      const GImage* src1 = dynamic_cast<const GImage*>(gdp[0]);

      vector<int> values(dts.size(), 0);
      vector<int> index(dts.size(), 0);

      for (int i = 0; i < src1->getSize(); ++i)
        {
          for (int y = 0; y < src1->ySize(); ++y)
            for (int x = 0; x < src1->xSize(); ++x)
              {
                for (unsigned int k = 0; k < values.size(); ++k)
                  {
                    if (dts[k].type == DType::image)
                      {
                        const GImage* gip = dynamic_cast<const GImage*>(gdp[k]);
                        values[k] = (*gip)[i].getPixel(x, y);
                      }
                    else
                      {
                        const GInteger* ip = dynamic_cast<const GInteger*>(gdp[k]);
                        values[k] = (*ip)[index[k]];
                        ++index[k];
                        if (index[k] >= ip->getSize())
                          index[k] = 0;
                      }
                  }

                int value = sm.evaluate(values, x, y, i);
                (*tresult)[i].setPixelLimited(x, y, value);
                // cout << x << "," << y << endl;
              }
        }
    }
  else
    throw SdpException("evaluate", "Expected integer or image");
}
