#include <matrixtemplate.h>
#include <matrixtools.h>
#include <vectorarith.h>
#include <vectortools.h>
#include <MatrixAlgebra.h>
#include "LMSolver.h"

using namespace std;
using namespace ice;

namespace ice
{
  bool LMSolver::getJacobian(const vector<double>& x,
                             const vector<int>& select,
                             const vector<double>& fv0,
                             ice::matrix<double>& jm) const
  {
    vector<double> fv(functionDim);
    vector<double> x1(x);
    int oldState = fn.getState();
    fn.setState(fn.jacobian);
    for (int j = 0; j < nVars; ++j)
      {
        int idx = select[j];
        double temp = x[idx];

        double delta = eps;

        x1[idx] = temp + delta;

        fn(x1, fv);

        // cout << x1 << " -> " << fv << endl;
        for (int i = 0; i < functionDim; ++i)
          {
            jm[i][j] = (fv[i] - fv0[i]) / delta;
            // cout << (fv[i] - fv0[i]) / delta << endl;
          }

        x1[idx] = temp;
      }
    fn.setState(oldState);
    return true;
  }

  void LMSolver::solve(vector<double>& x, const vector<int>& select)
  {
    nIterations = 0;
    nVars = select.size();
    if (nVars > functionDim || nVars > (int)x.size())
      {
        throw logic_error("too much variables");
      }

    for (unsigned int i = 0; i < select.size(); ++i)
      if (select[i] >= (int)x.size() || select[i] < 0)
        {
          throw out_of_range("index out of range");
        }

    vector<double> functionValue(functionDim);

    fn(x, functionValue);
    double functionNorm2 = ice::norm2(functionValue);

    double lmPara = -1; // Levenberg-Marquard-Parameter
    stop = 0;
    ice::matrix<double> jm(functionDim, nVars);
    bool newJacobi = true;
    while (stop == 0)
      {
        if (newJacobi)
          {
            getJacobian(x, select, functionValue, jm);
            newJacobi = false;
          }

        //      cout << "Jacobi: " << jm << endl;
        //      cout << "functionValue: " << functionValue << endl;
        ice::matrix<double> A = !jm * jm;
        //      cout << "A: " << A << endl;

        if (lmPara < 0)
          {
            double ndiag = 0;
            for (int i = 0; i < nVars; ++i)
              {
                ndiag += A[i][i] * A[i][i];
              }
            ndiag = sqrt(ndiag);
            lmPara = 0.1 * ndiag;
          }
        // cout << "lmPara: " << lmPara << endl;

        for (int i = 0; i < nVars; ++i)
          {
            A[i][i] += lmPara;
          }

        // cout << "A_reg: " << A << endl;

        vector<double> b = !jm * functionValue;
        // cout << "b: " << b << endl;

        vector<double> deltaX = solveLinearEquation(A, b);
        // cout << "deltaX: " << deltaX << endl;

        // evaluate step
        bool useStep = true;

        vector<double> newX(x);

        bool equal = true;
        for (int i = 0; i < nVars; ++i)
          {
            int idx = select[i];
            double temp = x[idx];
            newX[idx] = temp - deltaX[i];
            if (newX[idx] != temp)
              {
                equal = false;
              }
          }

        vector<double> newFunctionValue(functionDim);
        fn(newX, newFunctionValue);

        double newFNorm2 = ice::norm2(newFunctionValue);

        vector<double> predictedFunctionValue = functionValue - jm * deltaX;
        double predictedFNorm2 = norm2(predictedFunctionValue);

        double Improvement = functionNorm2 - newFNorm2;
        double predictedImprovement = functionNorm2 - predictedFNorm2;

        double q = predictedImprovement > 0 ? Improvement / predictedImprovement : 0.5;
        /*
        cout << "Improvement: " << Improvement<<endl;
        cout << "predictedImprovement: " << predictedImprovement<<endl;
        cout << "q: " << q << endl;
        */
        if (q < 0.3)
          {
            useStep = false;
            // useStep=Improvement>0;
            lmPara *= 2;
          }
        else if (q > 0.8)
          {
            lmPara = lmPara / 2;
            if (lmPara < DBL_EPSILON * 100)
              {
                lmPara = DBL_EPSILON * 100;
              }
          }

        double maxCos = 0;
        for (int i = 0; i < nVars; ++i)
          {
            double sum2 = 0;
            double sumx = 0;
            for (int k = 0; k < functionDim; ++k)
              {
                double val = jm[k][i];
                sum2 += val * val;
                sumx += val * functionValue[k];
              }
            double cos = sumx / sqrt(sum2 * functionNorm2);
            // cout << "cos: " << cos << endl;
            if (fabs(cos) > maxCos)
              {
                maxCos = fabs(cos);
              }
          }

        // cout << "maxCos: " << maxCos << endl;
        if (maxCos < gtol)
          {
            stop |= 4;
          }
        // cout << "Improvement: " << Improvement << " ?? " << (ftol * ftol) << endl;
        // cout << "predictedImprovement: " << predictedImprovement << endl;

        if (Improvement < ftol2 && predictedImprovement < ftol2)
          {
            stop |= 1;
          }

        if (useStep)
          {
            x = newX;
            newJacobi = true;
            functionValue = newFunctionValue;
            functionNorm2 = newFNorm2;
            //cout << "newX: " << newX << endl;
            //cout << norm2(deltaX) << " ?? " << xtol2 << endl;
            if (equal || norm2(deltaX) <= xtol2)
              {
                stop |= 2;
              }

          }
        // cout << "nIterations: " << nIterations << endl;
        nIterations++;
        if (nIterations > maxIterations)
          {
            stop |= 8;
          }
      }
    errorValue = sqrt(functionNorm2);
  }

  void LMSolver::solve(vector<double>& x)
  {
    vector<int> select(x.size());
    for (unsigned int i = 0; i < select.size(); ++i)
      {
        select[i] = i;
      }
    solve(x, select);
  }
}
