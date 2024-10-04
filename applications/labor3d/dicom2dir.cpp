// programm, welches einen DICOM-Datensatz in eine Verzeichnisstruktur
// umsetzt

#include <ctype.h>

#include <string>

#include <image.h>

const string tempfile = "/tmp/d2d";

using namespace std;

bool verbose = false;
bool doit = true;

void printHex(int h, bool withX = false)
{
  string res;
  if (h != 0)
    {
      while (h != 0)
        {
          int hd = h % 16;
          h /= 16;
          res = "0123456789ABCDEF"[hd] + res;
        }
    }
  else
    res = "0";
  cout << res;
}
void System(const string& cmd)
{
  system(cmd.c_str());
}

string Escape(const string& fn)
{
  string res;
  for (unsigned int i = 0; i < fn.length(); i++)
    {
      char c = fn[i];
      if (isalnum(c) || c == '/' || c == '.' || c == '-')
        res += c;
      else
        res += '_';
    }
  return res;
}

class image
{
public:
#if 0
  int rows;
  int cols;
#endif
  string date;
  string time;
  string instanceUID;
  int number;
  string fileid;
  string basename;
  void print() const
  {
    cout << "image: " << fileid << "=" << instanceUID << "(" << number << ")" << endl;
  }
  void convert(const string& srcdir, const string& dstname) const
  {
    string cmd = "dicom2 " + Escape(srcdir + "/" + fileid) + " -w --to=" + Escape(dstname + "/");
    //+ NumberString(number,-2)+".bmp");
    cout << cmd << endl;
    if (doit)
      System(cmd);
    cmd = "mv " + Escape(dstname + "/" + basename + ".bmp");
    cmd += " " + Escape(dstname + "/img" + NumberString(number, -3) + ".bmp");
    cout << cmd << endl;
    if (doit)
      System(cmd);
  }
};

class Series
{
public:
  string date;
  string time;
  string instanceUID;
  string description;
  int number;
  vector<image> img;

  void print() const
  {
    cout << "series: " << instanceUID << "(" << number << ")" << endl;
    for (unsigned int i = 0; i < img.size(); i++)
      img[i].print();
  }

  void convert(const string& srcdir, const string& dstname) const
  {
    string dirname = Escape(dstname + "/" + NumberString(number, -2) + "_" + description);
    string cmd = "mkdir -p " + dirname;
    cout << cmd << endl;
    System(cmd);
    for (unsigned int i = 0; i < img.size(); i++)
      img[i].convert(srcdir, dirname);
  }
};

class Study
{
public:
  string date;
  string time;
  string instanceUID;
  unsigned int id;
  vector<Series> ser;

  void print() const
  {
    cout << "study: " << instanceUID << endl;
    for (unsigned int i = 0; i < ser.size(); i++)
      ser[i].print();
  }

  void convert(const string& srcdir, const string& dstname) const
  {
    string dirname = Escape(dstname + "/" + date + "_" + time);
    string cmd = "mkdir -p " + dirname;
    cout << cmd << endl;
    System(cmd);
    for (unsigned int i = 0; i < ser.size(); i++)
      ser[i].convert(srcdir, dirname);
  }
};

class Patient
{
public:
  string name;
  unsigned int id;
  vector<Study> stud;

  void print() const
  {
    cout << "patient: " << name << " = " << id << endl;
    for (unsigned int i = 0; i < stud.size(); i++)
      stud[i].print();
  }

  void convert(const string& srcdir, const string& dstname) const
  {
    string dirname = Escape(dstname + "/" + name + "_" + NumberString(id));
    string cmd = "mkdir -p " + dirname;
    cout << cmd << endl;
    System(cmd);
    for (unsigned int i = 0; i < stud.size(); i++)
      stud[i].convert(srcdir, dirname);
  }
};

class parsed_line
{
private:
  string line;
  size_t cursor;
public:
  parsed_line(const string& ln): line(ln), cursor(0) {}
  bool eol() const
  {
    return cursor >= line.size();
  }
  bool skip(char c)
  {
    while (!eol() && line[cursor] != c)
      cursor++;
    if (!eol())
      cursor++;
    return !eol();
  }

  string getToDel(char del)
  {
    string res;
    while (!eol() && line[cursor] != del)
      {
        res += line[cursor];
        cursor++;
      }
    if (!eol())
      cursor++;
    return res;
  }

  unsigned int gethex()
  {
    unsigned int res = 0;
    for (int i = 0; i < 4; i++)
      {
        if (!eol())
          {
            res = res * 16;
            char c = line[cursor];
            switch (c)
              {
              case '0':
                res += 0;
                break;
              case '1':
                res += 1;
                break;
              case '2':
                res += 2;
                break;
              case '3':
                res += 3;
                break;
              case '4':
                res += 4;
                break;
              case '5':
                res += 5;
                break;
              case '6':
                res += 6;
                break;
              case '7':
                res += 7;
                break;
              case '8':
                res += 8;
                break;
              case '9':
                res += 9;
                break;
              case 'A':
              case 'a':
                res += 10;
                break;
              case 'B':
              case 'b':
                res += 11;
                break;
              case 'C':
              case 'c':
                res += 12;
                break;
              case 'D':
              case 'd':
                res += 13;
                break;
              case 'E':
              case 'e':
                res += 14;
                break;
              case 'F':
              case 'f':
                res += 15;
                break;
              default:
                cerr << "Formatfehler in hex-Wert: " << c << endl;
              }
          }
        cursor++;
      }
    if (!eol())
      cursor++; // skip delimiter
    return res;
  }
};

class tag
{
private:

public:
  tag(): high(0), low(0), value(""), lastvalue("") {}
  tag(const string& n)
  {
    parsed_line pl(n);

    pl.skip('(');

    high = pl.gethex();

    low = pl.gethex();

    pl.skip(']');
    pl.skip('[');

    lastvalue = value = pl.getToDel(']');
    while (pl.skip('['))
      {
        lastvalue = pl.getToDel(']');
        value += "/" + lastvalue;
      }
    cout << "tag: ";
    printHex(high);
    cout << " / ";
    printHex(low);
    cout << endl;
  }
  unsigned int id() const
  {
    return (high << 16) + low;
  }
  bool equal(int hi, int lo) const
  {
    return hi == high && lo == low;
  }

  int high;
  int low;
  string value;
  string lastvalue;
};

bool readtag(FILE* f, tag& c)
{
  char line[500];
  char* res;
  int cursor;
  do
    {
      res = fgets(line, 500, f);
      if (res == NULL)
        throw "eof";
      //  cout << ":" << line << endl;
      cursor = 0;
      while (line[cursor] == ' ')
        cursor++;
    }
  while (res[cursor] != '(');

  c = tag(line);
  return true;
}

void readPatient(FILE* f, Patient& pat)
{
  tag t;
  do
    {
      readtag(f, t);
      if (t.equal(0x10, 0x10))
        pat.name = t.value;
      else if (t.equal(0x10, 0x20))
        pat.id = Integer(t.value);

    }
  while (!t.equal(0xFFFE, 0xE00D));
}

void readStudy(FILE* f, Study& std)
{
  tag t;
  do
    {
      readtag(f, t);
      if (t.equal(0x08, 0x20))
        std.date = t.value;
      else if (t.equal(0x08, 0x30))
        std.time = t.value;
      else if (t.equal(0x20, 0x0D))
        std.instanceUID = t.value;
      else if (t.equal(0x20, 0x10))
        std.id = Integer(t.value);
    }
  while (!t.equal(0xFFFE, 0xE00D));
}

void readSeries(FILE* f, Series& ser)
{
  tag t;
  do
    {
      readtag(f, t);
      if (t.equal(0x08, 0x21))
        ser.date = t.value;
      else if (t.equal(0x08, 0x31))
        ser.time = t.value;
      else if (t.equal(0x08, 0x103E))
        ser.description = t.value;
      else if (t.equal(0x20, 0x0E))
        ser.instanceUID = t.value;
      else if (t.equal(0x20, 0x11))
        ser.number = Integer(t.value);
    }
  while (!t.equal(0xFFFE, 0xE00D));
}

void readImage(FILE* f, image& img)
{
  tag t;
  do
    {
      readtag(f, t);
      if (t.equal(0x04, 0x1500))
        {
          img.fileid = t.value;
          img.basename = t.lastvalue;
          // cout << img.fileid << " == " << img.basename << endl;
        }
      else if (t.equal(0x08, 0x23))
        img.date = t.value;
      else if (t.equal(0x08, 0x33))
        img.time = t.value;
      else if (t.equal(0x08, 0x18))
        img.instanceUID = t.value;
      else if (t.equal(0x20, 0x13))
        {
          img.number = Integer(t.value);
          cout << "instance: " << img.number << endl;
        }
#if 0
      else if (t.equal(0x28, 0x11))
        img.cols = Integer(t.value);
      else if (t.equal(0x28, 0x10))
        img.rows = Integer(t.value);
#endif
      else if (t.equal(0xFFFE, 0xE000)) // embedded sequence (?)
        {
          cout << "embedded" << endl;
          tag th;
          do
            {
              readtag(f, th);
            }
          while (!th.equal(0xFFFE, 0xE00D));
          cout << "end embedded" << endl;
        }
    }
  while (!t.equal(0xFFFE, 0xE00D));
  cout << "---- end image " << endl;
}

int main(int argc, char** argv)
{
  vector<Patient> pat;

  string srcdir = "import/dvd";
  string dstdir = "import/bmp";

  string dicomdir = srcdir + "/DICOMDIR";

  tag t;
  FILE* dd = popen(("medcon -alias -f " + dicomdir).c_str(), "r");
  try
    {
      while (readtag(dd, t))
        {
          if (t.equal(0xFFFE, 0xE000))
            {
              do
                {
                  readtag(dd, t);
                }
              while (! t.equal(0x0004, 0x1430));

              cout << "new item " << t.value << endl;

              if (t.value == "PATIENT")
                {
                  pat.push_back(Patient());
                  readPatient(dd, pat.back());
                }
              else if (t.value == "STUDY")
                {
                  pat.back().stud.push_back(Study());
                  readStudy(dd, pat.back().stud.back());
                }
              else if (t.value == "SERIES")
                {
                  pat.back().stud.back().ser.push_back(Series());
                  readSeries(dd, pat.back().stud.back().ser.back());
                }
              else if (t.value == "IMAGE")
                {
                  pat.back().stud.back().ser.back().img.push_back(image());
                  readImage(dd, pat.back().stud.back().ser.back().img.back());
                }
              else
                cout << t.high << " | " << t.low << ": " << t.value << endl;
            }
        }
    }
  catch (const char* msg)
    {
      if (strcmp(msg, "eof") != 0)
        cerr << "Exeption: " << msg << endl;
    }
  pclose(dd);
#if 0
  for (unsigned int i = 0; i < pat.size(); i++)
    pat[i].print();
#endif
  for (unsigned int i = 0; i < pat.size(); i++)
    pat[i].convert(srcdir, dstdir);

  return 0;
}
