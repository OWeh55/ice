void print(const vector<double>& v,
           bool index = true, bool centered = false,
           bool sc = false)
{
  int width = 8;
  int dim = v.size();
  int fm = dim / 2;
  if (index)
    {
      if (centered)
        {
          for (int i = -fm; i < -fm + dim; ++i)
            cout << setw(width) << i << " ";
        }
      else
        {
          for (int i = 0; i < dim; ++i)
            {
              int f = i;
              if (f >= fm)
                f -= dim;
              cout << setw(width) << f << " ";
            }
        }

      cout << endl;
    }
  double sum2 = 0;
  for (int idx = 0; idx < dim; ++idx)
    {
      sum2 += v[idx] * v[idx];
      if (sc)
        cout << setw(width) << setprecision(2) << scientific << v[idx] << " ";
      else
        cout << setw(width) << setprecision(4) << fixed << v[idx] << " ";
    }
  cout << "(" << sqrt(sum2) << ")";
  cout << endl;
}

void printc(const vector<double>& v1, const vector<double>& v2,
            bool index = true, bool centered = false,
            bool sc = false)
{
  print(v1, index, centered, sc);
  print(v2, false, centered, sc);
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

// noise
void setFunction4(double para, vector<double>& v)
{
  int size = v.size();
  if (para == 0.0) // default of para !!
    para = 1.0;
  for (int i = 0; i < size; i++)
    {
      v[i] = drand48() * para;
    }
}


void setFunction(unsigned int type, double para, vector<double>& v)
{
  vector<testFunction> func{setFunction0, setFunction1, setFunction2, setFunction3, setFunction4};
  if (type < func.size())
    func[type](para, v);
  else
    {
      cerr << "undefined function type" << endl;
      exit(1);
    }
}
