#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirfunc.h>
#include <strtool.h>

using namespace std;
using namespace ice;

bool leftMatch(const string &line,const string &pattern)
{
  int len=pattern.size();
  return line.substr(0,len) == pattern;
}

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

void find(const string &fn)
{
  vector<string> buffer;
  read(fn,buffer);
  bool afterThrow=false;
  int lineNr=0;
  for (auto &line:buffer)
    {
      lineNr++;
      string hline=trim(line);
      if (!afterThrow)
	{
	  if (leftMatch(hline,"throw"))
	    {
	      afterThrow=true;
	      // cout << hline << endl;
	    }
	}
      else
	{
	  if (leftMatch(hline,"return"))
	    {
	      cerr << fn << ":" << lineNr << ":1: error: return unused"<<endl;
	    }
	  else
	    afterThrow=false;
	}
    }
  // store backup
  string backupFilename=fn+".orig";
  //  rename(fn.c_str(),backupFilename.c_str());
  // write new file
  //  write(fn,buffer);
}

int main(int argc,char**argv)
{
  vector<string> files;
  Directory(files,"/home/noo/ice/src","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src","*.h",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/visual","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/visual","*.h",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/ocr","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/ocr","*.h",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/devices","*.cpp",DIR_FILE|DIR_WITHPATH);
  Directory(files,"/home/noo/ice/src/devices","*.h",DIR_FILE|DIR_WITHPATH);
  for (auto file:files)
    {
      cout << file << endl;
      find(file);
    }
}
