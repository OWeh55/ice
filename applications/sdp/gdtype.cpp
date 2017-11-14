#include "gdtype.h"

const std::vector<std::string> typestring =
{
  "integer", "floating point", "string",
  "point", "contur", "region",
  "image", "graph"
};

std::string DType::getString(int i)
{
  if (i == 0)
    return "unknown";
  std::string result;
  for (int k = 0; k < (int)typestring.size(); ++k)
    {
      if (i & 1)
        {
          if (!result.empty())
            result += ", ";
          result += typestring[k];
        }
      i >>= 1;
    }
  return result;
}

std::string DType::getString() const
{
  return getString(type);
}
