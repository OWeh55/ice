#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>
#include <unistd.h>

#include "file.h"

using namespace std;

bool fileexists(const std::string& fn)
{
  struct stat buf;
  int rc = stat(fn.c_str(), &buf);
  return rc == 0;
}

void splitFileName(const std::string& fname,
                   std::string& path,
                   std::string& basename,
                   std::string& extension)
{
  size_t poss = fname.find_last_of('/');
  if (poss == string::npos)
    {
      path.clear();
      poss = 0;
    }
  else
    {
      path = fname.substr(0, poss);
      poss++;
    }

  size_t posd = fname.find_last_of('.');
  if (posd == string::npos)
    {
      extension.clear();
      posd = fname.size();
    }
  else
    {
      extension = fname.substr(posd + 1);
    }
  basename = fname.substr(poss, posd - poss);
}

std::string makeFileName(const std::string& path,
                         const std::string& basename,
                         const std::string& extension)
{
  string result = path;
  if (!result.empty() && result.back() != '/')
    result += '/';
  result += basename;
  if (!extension.empty())
    {
      if (extension[0] == '.')
        result += extension;
      else
        result += '.' + extension;
    }
  return result;
}

std::string findFile(const std::string& name, std::vector<std::string>& pathlist)
{
  if (name[0] == '/') //absolute path in name -> return this
    return name;

  bool found = false;
  string realFileName;
  for (int i = 0; i < (int)pathlist.size() && !found; ++i)
    {
      realFileName = makeFileName(pathlist[i], name, "");
      found = fileexists(realFileName);
    }

  if (found)
    return realFileName;
  else
    return name;
}
