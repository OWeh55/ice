// Die Eigenschaft, dass Ganglien Sehreize hemmen, wenn in der Umgebung
// starke Reize vorliegen, wird durch LSI-Filter genähert

#include <image.h>

int main(int argc, char* argv[])
{
  string filename = "test_rgb.jpg";

  bool writefile = false;

  int size = 11;
  int n1 = 3;

  double v1 = 1.0 / (n1 * n1);

  string dir = "";

  int c;

  while ((c = getopt(argc, argv, "f:v:n:s:d:o")) >= 0)
    {
      switch (c)
        {
        case 'f':
          filename = optarg;
          break;
        case 'v':
          v1 = atof(optarg);
          break;
        case 'n':
          n1 = atol(optarg);
          v1 = 1.0 / (n1 * n1);
          break;
        case 's':
          size = atol(optarg);
          break;
        case 'd':
          dir = optarg;
          break;
        case 'o':
          writefile = true;
          break;
        }
    }

  vector<string> files;

  if (!dir.empty())
    Directory(files, dir + "/*.bmp");
  else
    {
      dir = ".";
      files.push_back(filename);
    }

  Matrix mask(size, size);

  //  v1=1.0/(n1*n1);

  int o1 = (size - n1) / 2;
  double v2 = (double)(size * size - n1 * n1) / (double)(n1 * n1) * v1;

  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      {
        mask[i][j] = -v1;
      }

  for (int i = o1; i < size - o1; i++)
    for (int j = o1; j < size - o1; j++)
      {
        mask[i][j] = v2;
      }

  while (!files.empty())
    {
      cout << (dir + "/" + filename) << endl;
      filename = files.back();
      files.pop_back();

      Image src = ReadImg(dir + "/" + filename);
      Image dest = NewImg(src);

      Show(ON, src);
      Show(ON, dest);

      LSIImg(src, mask, 128, dest);

      if (writefile)
        WriteImg(dest, filename);
      else
        GetChar();
    }

  return 0;
}
