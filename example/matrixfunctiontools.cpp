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

typedef void (*testFunction)(double para1, double para2, matrix<double>& v);

// delta
void setFunction0(double para1, double para2, matrix<double>& v) // \delta
{
  int nCols = v.cols();
  int nRows = v.rows();
  for (int r = 0; r < nRows; r++)
    for (int c = 0; c < nCols; c++)
      v[r][c] = 0.0;
  int c0 = nCols / 2;
  c0 = ((c0 + int(para1)) % nCols + nCols) % nCols;
  int r0 = nRows / 2;
  r0 = ((r0 + int(para2)) % nRows + nRows) % nRows;
  v[r0][c0] = 1.0;
}

// gauss like
void setFunction1(double para1, double para2, matrix<double>& v) // \delta
{
  int nCols = v.cols();
  double c0 = nCols / 2;
  int nRows = v.rows();
  double r0 = nRows / 2;
  if (para1 == 0.0)
    para1 = 1;
  if (para2 == 0.0)
    para2 = 1;
  for (int r = 0; r < nRows; r++)
    for (int c = 0; c < nCols; c++)
      {
        double dd = (r - r0) * (r - r0) / para1 + (c - c0) * (c - c0) / para2;
        v[r][c] = exp(-dd);
      }
}

void setFunction(unsigned int type, double para1, double para2, matrix<double>& v)
{
  vector<testFunction> func{setFunction0, setFunction1};

  if (type < func.size())
    func[type](para1, para2, v);
  else
    {
      cerr << "undefined function type" << endl;
      exit(1);
    }
}
