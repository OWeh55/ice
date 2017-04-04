#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirfunc.h>
#include <strtool.h>

using namespace std;
using namespace ice;

void read(const string &fn, vector<string> &buffer)
{
  ifstream is(fn);
  string line;
  while (getline(is,line))
    buffer.push_back(line);
}

void write(const string &fn, const vector<string> &buffer)
{
  ofstream os(fn);
  for (string line:buffer)
    os << line << endl;
}

void convert(const string &fn)
{
  vector<string> buffer;
  read(fn,buffer);
  for (auto &line:buffer)
    {
      string hline=trim(line);
      if (hline.substr(0,7)== "Message")
	{
	  line = "throw IceException" + hline.substr(7);
	  //cout << hline << endl;
	}
      else if (hline.substr(0,20)=="#include \"message.h\"")
	{
	  line="#include \"IceException.h\"";
	  // cout << hline << endl;
	}
    }
  // store backup
  string backupFilename=fn+".orig";
  rename(fn.c_str(),backupFilename.c_str());
  // write new file
  write(fn,buffer);
}

int main(int argc,char**argv)
{
  vector<string> files;
  //  Directory(files,"/home/noo/ice/src","*.cpp",DIR_FILE|DIR_WITHPATH);
  //  Directory(files,"/home/noo/ice/src","*.h",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/visual","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/visual","*.h",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/ocr","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/ocr","*.h",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/devices","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/devices","*.h",DIR_FILE|DIR_WITHPATH);
  for (auto file:files)
    {
      cout << file << endl;
      convert(file);
    }
}
