#include <iostream>
#include <fstream>
#include <sstream>
#include "tv.h"

bool isWhite(char c)
{
  return (c == ' ') || (c == '\n') || (c == '\t') || (c == 13);
}

void skipSpace(const std::string& s, unsigned int& pos)
{
  // ignore spaces
  while (pos < s.size() && isWhite(s[pos]))
    ++pos;
}

void skipWord(const std::string& s, unsigned int& pos)
{
  // ignore all until space
  while (pos < s.size() && ! isWhite(s[pos]))
    {
      ++pos;
    }
  // ignore spaces
  skipSpace(s, pos);
}

string readWord(const std::string& s, unsigned int& pos)
{
  string res;
  while (pos < s.size() && !isWhite(s[pos]))
    {
      res += s[pos];
      ++pos;
    }
  return res;
}

int readPosInt(const std::string& s, unsigned int& pos)
{
  int i = 0;
  skipSpace(s, pos);
  unsigned int posStart = pos;
  while (pos < s.size() && isdigit(s[pos]))
    {
      i = 10 * i + (s[pos] - '0');
      ++pos;
    }
  //  cout << i << endl;
  if (posStart == pos)
    {
      std::cerr << ">>" << s << "<< at " << pos << std::endl;
      throw "No positiv number found";
    }
  return i;
}

void ReadWorld1(std::vector<Triangle>& world, istream& is, int flags)
{
  string line;
  std::vector<Vector3> points;
  Vector3 color(1.0, 1.0, 1.0);
  while (getline(is, line))
    {
      switch (line[0])
        {
        case 'P': // Point
        {
          std::istringstream iss(line.substr(1));
          float x, y, z;
          iss >> x;
          iss >> y;
          iss >> z;
          if (flags & 1) x = -x;
          if (flags & 2) y = -y;
          if (flags & 4) z = -z;
          points.push_back(Vector3(x, y, z));
        }
        break;
        case 'C': // Color
        {
          std::istringstream iss(line.substr(1));
          iss >> color[0];
          iss >> color[1];
          iss >> color[2];
        }
        break;
        case 'T': // Triangle
        {
          unsigned int pos = 1;
          int i1 = readPosInt(line, pos);
          int i2 = readPosInt(line, pos);
          int i3 = readPosInt(line, pos);
          Triangle t(points[i1], points[i2], points[i3], color);
          world.push_back(t);
        }
        break;
        default:
          cout << line << endl;
        }
    }
}

void ReadWorld2(std::vector<Triangle>& world, istream& is, int flags)
{
  string line;
  std::vector<Vector3> points;
  Vector3 color(1.0, 1.0, 1.0);
  while (getline(is, line))
    {
      unsigned int i = 0;

      skipSpace(line, i);

      string id = readWord(line, i);

      if (id == "v") // vertex
        {
          std::istringstream iss(line.substr(i));
          float x, y, z;
          iss >> x;
          iss >> y;
          iss >> z;
          if (flags & 1) x = -x;
          if (flags & 2) y = -y;
          if (flags & 4) z = -z;
          points.push_back(Vector3(x, y, z));
          // cout << "v " << x << " " << y << " " << z << endl;
        }
      else if (id == "f")
        {
          int i1 = readPosInt(line, i);
          if (line[i] == '/')
            skipWord(line, i);
          int i2 = readPosInt(line, i);
          if (line[i] == '/')
            skipWord(line, i);
          int i3 = readPosInt(line, i);
          if (line[i] == '/')
            skipWord(line, i);

          Triangle t(points[i1 - 1], points[i2 - 1], points[i3 - 1], color);
          world.push_back(t);
          // cout << "T " << i1 << " " << i2 << " " << i3 << endl;

          skipSpace(line, i);

          while (i < line.size())
            {
              i2 = i3;
              i3 = readPosInt(line, i);
              if (line[i] == '/')
                skipWord(line, i);
              Triangle t(points[i1 - 1], points[i2 - 1], points[i3 - 1], color);
              world.push_back(t);
              // cout << "T " << i1 << " " << i2 << " " << i3 << endl;
              skipSpace(line, i);
            }
        }
    }
}

void ReadWorld(std::vector<Triangle>& world, const string& fname, int flags)
{
  if (fname == "-")
    {
      ReadWorld1(world, cin, flags);
    }
  else
    {
      ifstream is(fname);
      if (!is.good())
        throw "Cannot open input file";
      if (fname.substr(fname.size() - 4) == ".obj")
        ReadWorld2(world, is, flags);
      else
        ReadWorld1(world, is, flags);
    }
}
