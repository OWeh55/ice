#ifndef _FILENAME_H
#define _FILENAME_H

#include <string>

class FileName
{
public:
  static const char pathdel = '/';
  static const char extdel = '.';

  FileName(std::string filename);
  FileName(const std::string& path,
           const std::string& name,
           const std::string& ext);

  std::string getFilename() const;

  std::string getName() const
  {
    return name;
  }
  std::string getPath() const
  {
    return path;
  }
  std::string getExtension() const
  {
    return extension;
  }

  void getName(const std::string& n)
  {
    name = n;
  }
  void getPath(const std::string& p)
  {
    path = p;
  }
  void getExtension(const std::string& x)
  {
    extension = x;
  }

private:
  void cleanPath();
  void cleanName();
  void cleanExtension();
  void normalize()
  {
    cleanPath();
    cleanName();
    cleanExtension();
  }
  std::string path;
  std::string name;
  std::string extension;
};

#endif
