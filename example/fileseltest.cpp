#include <visual/ConsoleWin.h>
#include <visual/App.h>
#include <wx/filedlg.h>
#include <image.h>

void print(vector<string> l)
{
  for (unsigned int i = 0; i < l.size(); i++)
    cout << l[i] << endl;
}

int main(int argc, char* argv[])
{
  string path = ".";

  if (argc > 1)
    path = argv[1];

  if (path[path.size() - 1] != '/')
    path += "/";

  OpenAlpha("SelFile-Test");
#if 0
  vector<string> dir;
  cout << "=== Dateien ===" << endl;
  Directory(dir, path, "*.jpg", DIR_FILE);
  print(dir);
  dir.clear();
  cout << "=== Verzeichnisse ===" << endl;
  Directory(dir, path, "*", DIR_DIR | DIR_WITHPATH);
  print(dir);
#endif
  Image img;
  string fn;

  if (SelFile(path + "/*.jpg", fn, path, DIR_FILE | DIR_SELPATH))
    {
      Print(path + "\n" + fn + "\n");
      img = ReadImg(path + "/" + fn);
      Show(ON, img);
    }
  else
    Print("No file selected");

  GetChar();

  return OK;
}
