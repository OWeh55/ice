#include "f_filter.h"

Filter* Filter::first;

const GData* Filter::getInputData(int i) const
{
  if (i < 0 || i >= (int)input.size())
    throw SdpException(name, "Input index out of range");
  if (input[i].inp == NULL)
    return NULL;
  return input[i].inp->getData(input[i].idx);
}

const GData* Filter::getInputData(int i, int type) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    throw SdpException(name, "Input undefined");
  if (!data->getType().matchType(type))
    throw WrongTypeException(name, type,  data->getType().type);
  return data;
}

DType Filter::getInputType(int i) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    return DType(DType::unknown, 0);
  return data->getType();
}

const GImage* Filter::getInputImage(int i) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    throw SdpException(name, "Input undefined");
  if (data->getType().matchType(DType::image))
    return dynamic_cast<const GImage*>(data);
  throw WrongTypeException(name, DType::image, data->getType().type);
}

const int Filter::getInputInt(int i, int def) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    return def;
  if (data->getType().matchType(DType::integer))
    return (*dynamic_cast<const GInteger*>(data))[0];
  throw WrongTypeException(name, DType::integer, data->getType().type);
}

const int Filter::getInputInt(int i) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    throw SdpException(name, "Input undefined");
  if (data->getType().matchType(DType::integer))
    return (*dynamic_cast<const GInteger*>(data))[0];

  throw WrongTypeException(name, DType::integer, data->getType().type);
}

const double Filter::getInputFloat(int i) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    throw SdpException(name, "Input undefined");
  if (data->getType().matchType(DType::integer))
    return (*dynamic_cast<const GInteger*>(data))[0];
  if (data->getType().matchType(DType::floatingpoint))
    return (*dynamic_cast<const GFloat*>(data))[0];
  throw WrongTypeException(name, DType::integer | DType::floatingpoint, data->getType().type);
}

const double Filter::getInputFloat(int i, double def) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    return def;
  if (data->getType().matchType(DType::integer))
    return (*dynamic_cast<const GInteger*>(data))[0];
  if (data->getType().matchType(DType::floatingpoint))
    return (*dynamic_cast<const GFloat*>(data))[0];
  throw WrongTypeException(name, DType::integer | DType::floatingpoint, data->getType().type);
}

const string& Filter::getInputString(int i) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    throw SdpException(name, "Input undefined");
  if (data->getType().matchType(DType::string_type))
    return (*dynamic_cast<const GString*>(data))[0];

  throw WrongTypeException(name, DType::string_type, data->getType().type);
}

const string& Filter::getInputString(int i, const string& def) const
{
  const GData* data = getInputData(i);
  if (data == NULL)
    return def;
  if (data->getType().matchType(DType::string_type))
    return (*dynamic_cast<const GString*>(data))[0];

  throw WrongTypeException(name, DType::string_type, data->getType().type);
}

void Filter::graph_out(ostream& os, set<const Filter*>& fs) const
{
  if (fs.count(this) == 0)
    {
      fs.insert(this);
      for (int i = 0; i < (int)input.size(); ++i)
        {
          if (input[i].inp != NULL)
            input[i].inp->graph_out(os, fs);
        }

      os << "F" << this << " [shape=record,label=\"{{";
      for (int i = 0; i < (int)input.size(); ++i)
        {
          if (i > 0)
            os << "|";
          os << "<i" << i << ">";
          //    if (input[i].inp==NULL)
          //      os << "*";
        }
      os << "}|" << getName() << "|{";
      for (int i = 0; i < (int)result.size(); ++i)
        {
          if (i > 0)
            os << "|";
          os << "<o" << i << ">";
        }
      os << "}}\"]" << endl;

      for (int i = 0; i < (int)input.size(); ++i)
        {
          if (input[i].inp != NULL)
            {
              os << "F" << input[i].inp << ":o" << input[i].idx << ":s";
              os << "->F" << this << ":i" << i << ":n" << endl;
            }
        }

      os << endl;
    }
}
