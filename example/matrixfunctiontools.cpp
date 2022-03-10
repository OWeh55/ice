void print(const matrix<double>& m,
           bool index = true, bool centered = true,
           bool sc = false)
{
  int width = 8;
  int cols = m.cols();
  int rows = m.rows();

  int c0 = centered ? cols / 2 : 0;
  int r0 = centered ? rows / 2 : 0;

  if (index)
    {
      // head line
      cout << setw(width) << ".";
      for (int i = 0; i < cols; ++i)
        {
          cout << setw(width) << (i - c0) << " ";
        }
      cout << endl;
    }

  double sum2 = 0;
  for (int r = 0; r < rows; r++)
    {
      if (index)
        cout << setw(width) << (r - r0);

      for (int c = 0; c < cols; c++)
        {
          double val = m[r][c];
          sum2 += val * val;
          if (sc)
            cout << setw(width) << setprecision(2) << scientific << val << " ";
          else
            cout << setw(width) << setprecision(4) << fixed << val << " ";
        }
      cout << endl;
    }
  cout << "sum: " << sqrt(sum2) << endl;
}

void printc(const matrix<double>& v1, const matrix<double>& v2,
            bool index = true, bool centered = true,
            bool sc = false)
{
  print(v1, index, centered, sc);
  cout << "-----" << endl;
  print(v2, index, centered, sc);
  cout << "-----" << endl;
}

typedef void (*testFunction)(double para, vector<double>& v);

// delta
void setFunction0(double para, vector<double>& v) // \delta
{
  int size = v.size();
  for (int i = 0; i < size; i++)
    v[i] = 0.0;
  int i0 = (int(para) % size + size) % size;
  v[i0] = 1.0;
}

// sin
void setFunction1(double para, vector<double>& v)
{
  int size = v.size();
  if (para == 0.0) para = 1.0;
  for (int i = 0; i < size; i++)
    {
      v[i] = sin(para * 2 * M_PI * i / size);
    }
}

// cos
void setFunction2(double para, vector<double>& v)
{
  int size = v.size();
  if (para == 0.0) para = 1.0;
  for (int i = 0; i < size; i++)
    {
      v[i] = cos(para * 2 * M_PI * i / size);
    }
}

// rectangle
void setFunction3(double para, vector<double>& v)
{
  int size = v.size();
  if (para == 0.0)
    para = size / 2.0;
  for (int i = 0; i < size; i++)
    {
      if (i < para)
        v[i] = 1.0;
      else
        v[i] = 0.0;
    }
}

void setFunction(unsigned int type, double para, vector<double>& v)
{
  vector<testFunction> func{setFunction0, setFunction1, setFunction2, setFunction3};
  if (type < func.size())
    func[type](para, v);
  else
    {
      cerr << "undefined function type" << endl;
      exit(1);
    }
}
