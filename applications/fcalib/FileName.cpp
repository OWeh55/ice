#include <iostream>
#include "FileName.h"


FileName::FileName(std::string n)
{
  size_t lastPathDelimiterPosition = n.rfind("/");
  if (lastPathDelimiterPosition == std::string::npos)
    path = "";
  else
    {
      path = n.substr(0, lastPathDelimiterPosition);
      n = n.substr(lastPathDelimiterPosition + 1);
    }
  size_t lastExtensionDelimiterPosition = n.rfind(".");
  if (lastExtensionDelimiterPosition == std::string::npos)
    {
      extension = "";
      name = n;
    }
  else
    {
      name = n.substr(0, lastExtensionDelimiterPosition);
      extension = n.substr(lastExtensionDelimiterPosition + 1);
    }
  normalize();
}

FileName::FileName(const std::string& path,
                   const std::string& name,
                   const std::string& ext): path(path), name(name), extension(ext)
{
  normalize();
}

std::string FileName::getFilename() const
{
  std::string res = path;
  if (res.empty() || path == "/")
    res += name;
  else
    res += pathdel + name;

  if (!extension.empty())
    res += extdel + extension;
  return res;
}

void FileName::cleanPath()
{
  while (path.length() > 1 && path[path.length() - 1] == '/')
    path.erase(path.length() - 1);
}

void FileName::cleanName()
{
  while (name.length() > 0 && name[0] == pathdel)
    name.erase(0, 1);
  while (name.length() > 0 && name[name.length() - 1] == extdel)
    name.erase(name.length() - 1);
  for (unsigned int i = 0; i < name.length(); i++)
    {
      if (name[i] == pathdel)
        throw "wrong character in name";
    }
}

void FileName::cleanExtension()
{
  while (extension.length() > 0 && extension[0] == extdel)
    extension.erase(0, 1);
  for (unsigned int i = 0; i < extension.length(); i++)
    {
      if (extension[i] == pathdel)
        throw "wrong character in extension";
      if (extension[i] == extdel)
        throw "wrong character in extension";
    }
}

#if 0

using namespace std;

int main(int argc, char** argv)
{
  try
    {
      if (argc == 2)
        {
          FileName fn(argv[1]);
          cout << fn.getPath() << endl;
          cout << fn.getName() << endl;
          cout << fn.getExtension() << endl;
          cout << fn.getFilename() << endl;
        }
      else if (argc == 4)
        {
          FileName fn(argv[1], argv[2], argv[3]);
          cout << fn.getPath() << endl;
          cout << fn.getName() << endl;
          cout << fn.getExtension() << endl;
          cout << fn.getFilename() << endl;
        }
    }
  catch (const char* msg)
    {
      cout << "exception: " << msg << endl;
    }
}
#endif

