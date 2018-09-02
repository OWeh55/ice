#include <iostream>

#include "file.h"

using namespace std;

int main(int argc, char** argv)
{

  if (argc < 2)
    {
      cout << argv[0] << " <filename>" << endl;
      exit(1);
    }

  string filename = argv[1];

  string path, basename, extension;
  splitFileName(filename, path, basename, extension);
  cout << path << " // " << basename << " // " << extension << endl;
  string filename2 = makeFileName(path, basename, extension);
  cout << filename2 << endl;

  // use basename+extension to test search
  string testname = makeFileName("", basename, extension);
  vector<string> pathlist;
  pathlist.push_back("/etc");
  pathlist.push_back(".");
  string home = getenv("HOME");
  if (!home.empty())
    pathlist.push_back(home);

  cout << "search " << testname << " in: " << endl;
  for (int i = 0; i < pathlist.size(); ++i)
    cout << "    " << pathlist[i] << endl;
  cout << findFile(testname, pathlist) << endl;

}
