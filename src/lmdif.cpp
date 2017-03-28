/************************lmdif.c*************************/
/*
 * Solves or minimizes the sum of squares of m nonlinear
 * functions of n variables.
 *
 * From public domain Fortran version
 * of Argonne National Laboratories MINPACK
 *
 * C translation by Steve Moshier
 * Adaption to ICE by Wolfgang Ortmann
 */

#include <stdio.h>
#include <float.h>
#include <math.h>

#include <iostream>

#include "numbase.h"

#include "lmdif.h"

using namespace std;

namespace ice
{

  /* resolution of arithmetic */
  double MACHEP = 1.2e-16;

  /* smallest nonzero number */
  double DWARF = 1.0e-38;

  double enorm(int n, double x[]);

  int fdjac2(lmfunc fcn, int m, int n, double x[], double fvec[], double fjac[],
             int ldfjac, int* iflag, double epsfcn);

  int qrfac(int m, int n, double a[], int lda,
            int pivot, int ipvt[], int lipvt,
            double rdiag[], double acnorm[]);

  int lmpar(int n, double r[], int ldr, int ipvt[],
            double diag[], double qtb[], double delta,
            double* par, double x[],
            double sdiag[]);

  int qrsolv(int n, double r[], int ldr, int ipvt[],
             double diag[], double qtb[], double x[],
             double sdiag[]);

  /*********************** lmdif.c ****************************/

  int lmdif(lmfunc* fcn, int funcdim, int n, double x[], double fvec[],
            double ftol, double xtol, double gtol, int maxiter, double epsfcn,
            double diag[], int mode, double factor,
            int nprint, int* info, int* iter,
            double fjac[], int ldfjac,
            int ipvt[], double qtf[])
  {
    /*
     *     **********
     *
     *     subroutine lmdif
     *
     *     the purpose of lmdif is to minimize the sum of the squares of
     *     m nonlinear functions in n variables by a modification of
     *     the levenberg-marquardt algorithm. the user must provide a
     *     subroutine which calculates the functions. the jacobian is
     *     then calculated by a forward-difference approximation.
     *
     *  fcn is the name of the user-supplied subroutine which
     *    calculates the functions. fcn must be declared
     *    in an external statement in the user calling
     *    program, and should be written as follows.
     *
     *    subroutine fcn(m,n,x,fvec,iflag)
     *    integer m,n,iflag
     *    double precision x(n),fvec(m)
     *    ----------
     *    calculate the functions at x and
     *    return this vector in fvec.
     *    ----------
     *    return
     *    end
     *
     *    the value of iflag should not be changed by fcn unless
     *    the user wants to terminate execution of lmdif.
     *    in this case set iflag to a negative integer.
     *
     *  m is a positive integer input variable set to the number
     *    of functions.
     *
     *  n is a positive integer input variable set to the number
     *    of variables. n must not exceed m.
     *
     *  x is an array of length n. on input x must contain
     *    an initial estimate of the solution vector. on output x
     *    contains the final estimate of the solution vector.
     *
     *  fvec is an output array of length m which contains
     *    the functions evaluated at the output x.
     *
     *  ftol is a nonnegative input variable. termination
     *    occurs when both the actual and predicted relative
     *    reductions in the sum of squares are at most ftol.
     *    therefore, ftol measures the relative error desired
     *    in the sum of squares.
     *
     *  xtol is a nonnegative input variable. termination
     *    occurs when the relative error between two consecutive
     *    iterates is at most xtol. therefore, xtol measures the
     *    relative error desired in the approximate solution.
     *
     *  gtol is a nonnegative input variable. termination
     *    occurs when the cosine of the angle between fvec and
     *    any column of the jacobian is at most gtol in absolute
     *    value. therefore, gtol measures the orthogonality
     *    desired between the function vector and the columns
     *    of the jacobian.
     *
     *  maxiter is a positive integer input variable. termination
     *    occurs when the number of iteration steps is at least
     *    maxiter.
     *
     *  epsfcn is an input variable used in determining a suitable
     *    step length for the forward-difference approximation. this
     *    approximation assumes that the relative errors in the
     *    functions are of the order of epsfcn. if epsfcn is less
     *    than the machine precision, it is assumed that the relative
     *    errors in the functions are of the order of the machine
     *    precision.
     *
     *  diag is an array of length n. if mode = 1 (see
     *    below), diag is internally set. if mode = 2, diag
     *    must contain positive entries that serve as
     *    multiplicative scale factors for the variables.
     *
     *  mode is an integer input variable. if mode = 1, the
     *    variables will be scaled internally. if mode = 2,
     *    the scaling is specified by the input diag. other
     *    values of mode are equivalent to mode = 1.
     *
     *  factor is a positive input variable used in determining the
     *    initial step bound. this bound is set to the product of
     *    factor and the euclidean norm of diag*x if nonzero, or else
     *    to factor itself. in most cases factor should lie in the
     *    interval (.1,100.). 100. is a generally recommended value.
     *
     *  nprint is an integer input variable that enables controlled
     *    printing of iterates if it is positive. in this case,
     *    fcn is called with iflag = 0 at the beginning of the first
     *    iteration and every nprint iterations thereafter and
     *    immediately prior to return, with x and fvec available
     *    for printing. if nprint is not positive, no special calls
     *    of fcn with iflag = 0 are made.
     *
     *  info is an integer output variable. if the user has
     *    terminated execution, info is set to the (negative)
     *    value of iflag. see description of fcn. otherwise,
     *    info is set as follows.
     *
     *    info = 0  improper input parameters.
     *
     *    info = 1  both actual and predicted relative reductions
     *        in the sum of squares are at most ftol.
     *
     *    info = 2  relative error between two consecutive iterates
     *        is at most xtol.
     *
     *    info = 3  conditions for info = 1 and info = 2 both hold.
     *
     *    info = 4  the cosine of the angle between fvec and any
     *        column of the jacobian is at most gtol in
     *        absolute value.
     *
     *    info = 5  number of calls to fcn has reached or
     *        exceeded maxfev.
     *
     *    info = 6  ftol is too small. no further reduction in
     *        the sum of squares is possible.
     *
     *    info = 7  xtol is too small. no further improvement in
     *        the approximate solution x is possible.
     *
     *    info = 8  gtol is too small. fvec is orthogonal to the
     *        columns of the jacobian to machine precision.
     *
     *  iter is an integer output variable set to the number of
     *    iteration steps.
     *
     *  fjac is an output m by n array. the upper n by n submatrix
     *    of fjac contains an upper triangular matrix r with
     *    diagonal elements of nonincreasing magnitude such that
     *
     *     t     t     t
     *    p *(jac *jac)*p = r *r,
     *
     *    where p is a permutation matrix and jac is the final
     *    calculated jacobian. column j of p is column ipvt(j)
     *    (see below) of the identity matrix. the lower trapezoidal
     *    part of fjac contains information generated during
     *    the computation of r.
     *
     *  ldfjac is a positive integer input variable not less than m
     *    which specifies the leading dimension of the array fjac.
     *
     *  ipvt is an integer output array of length n. ipvt
     *    defines a permutation matrix p such that jac*p = q*r,
     *    where jac is the final calculated jacobian, q is
     *    orthogonal (not stored), and r is upper triangular
     *    with diagonal elements of nonincreasing magnitude.
     *    column j of p is column ipvt(j) of the identity matrix.
     *
     *  qtf is an output array of length n which contains
     *    the first n elements of the vector (q transpose)*fvec.
     *
     *
     *     subprograms called
     *
     *  user-supplied ...... fcn
     *
     *  minpack-supplied ... dpmpar,enorm,fdjac2,lmpar,qrfac
     *
     *  fortran-supplied ... dabs,dmax1,dmin1,dsqrt,mod
     *
     *     argonne national laboratory. minpack project. march 1980.
     *     burton s. garbow, kenneth e. hillstrom, jorge j. more
     *
     *     **********
     */

    int i, iflag, ij, jj, j, l;
    double actred;
    double delta = factor; // dummy initialisation to avoid warning
    double dirder, fnorm, fnorm1, gnorm;
    double par, pnorm, prered, ratio;
    double sum, temp, temp1, temp2, temp3;
    double xnorm = 0.0;  // dummy initialisation to avoid warning

    // wa1, wa2, and wa3 are work arrays of length n.
    double* wa1 = new double [n];
    double* wa2 = new double [n];
    double* wa3 = new double [n];

    // wa4 is a work array of length m.
    double* wa4 = new double [funcdim];

    static double p1 = 0.1;
    static double p5 = 0.5;
    static double p25 = 0.25;
    static double p75 = 0.75;
    static double p0001 = 1.0e-4;

    MACHEP  = DBL_EPSILON;    // machine precision, was 1.2e-16;
    /* smallest nonzero number */
    DWARF   = DBL_MIN;              // was 1.0e-38;

    *info = 0;
    iflag = 0;
    int cycles = 0; // counter for all cycles (also unsuccessfull cycles)

    /*
     *     check the input parameters for errors.
     */
    if ((n <= 0) || (funcdim < n) || (ldfjac < funcdim) || (ftol < 0.0)
        || (xtol < 0.0) || (gtol < 0.0) || (maxiter <= 0)
        || (factor <= 0.0))
      goto L300;

    if (mode == 2)
      {
        /* scaling by diag[] */
        for (j = 0; j < n; j++)
          {
            if (diag[j] <= 0.0)
              goto L300;
          }
      }

    /*
     *     evaluate the function at the starting point
     *     and calculate its norm.
     */
    iflag = fcn(funcdim, n, x, fvec);

    if (iflag < 0)
      goto L300;

    fnorm = enorm(funcdim, fvec);
    /*
     *     initialize levenberg-marquardt parameter and iteration counter.
     */
    par = 0.0;
    *iter = 1;
    /*
     *     beginning of the outer loop.
     */

L30:

    /*
     *   calculate the jacobian matrix.
     */
    iflag = 2;
    fdjac2(fcn, funcdim, n, x, fvec, fjac, ldfjac, &iflag, epsfcn);

    if (iflag < 0)
      goto L300;

    /*
     *   if requested, call fcn to enable printing of iterates.
     */
    if (nprint > 0)
      {
        if ((*iter - 1) % nprint == 0)
          {
            iflag = fcn(funcdim, n, x, fvec);

            if (iflag < 0)
              goto L300;
          }
      }

    /*
     *   compute the qr factorization of the jacobian.
     */
    qrfac(funcdim, n, fjac, ldfjac, 1, ipvt, n, wa1, wa2);

    /*
     *   on the first iteration and if mode is 1, scale according
     *   to the norms of the columns of the initial jacobian.
     */
    if (*iter == 1)
      {
        if (mode != 2)
          {
            for (j = 0; j < n; j++)
              {
                diag[j] = wa2[j];

                if (wa2[j] == 0.0)
                  diag[j] = 1.0;
              }
          }

        /*
         *   on the first iteration, calculate the norm of the scaled x
         *   and initialize the step bound delta.
         */
        for (j = 0; j < n; j++)
          wa3[j] = diag[j] * x[j];

        xnorm = enorm(n, wa3);
        delta = factor * xnorm;

        if (delta == 0.0)
          delta = factor;
      }

    /*
     *   form (q transpose)*fvec and store the first n components in
     *   qtf.
     */
    for (i = 0; i < funcdim; i++)
      wa4[i] = fvec[i];

    jj = 0;

    for (j = 0; j < n; j++)
      {
        temp3 = fjac[jj];

        if (temp3 != 0.0)
          {
            sum = 0.0;
            ij = jj;

            for (i = j; i < funcdim; i++)
              {
                sum += fjac[ij] * wa4[i];
                ij += 1;  /* fjac[i+m*j] */
              }

            temp = -sum / temp3;
            ij = jj;

            for (i = j; i < funcdim; i++)
              {
                wa4[i] += fjac[ij] * temp;
                ij += 1;  /* fjac[i+m*j] */
              }
          }

        fjac[jj] = wa1[j];
        jj += funcdim + 1;  /* fjac[j+m*j] */
        qtf[j] = wa4[j];
      }

    /*
     *   compute the norm of the scaled gradient.
     */
    gnorm = 0.0;

    if (fnorm != 0.0)
      {
        jj = 0;

        for (j = 0; j < n; j++)
          {
            l = ipvt[j];

            if (wa2[l] != 0.0)
              {
                sum = 0.0;
                ij = jj;

                for (i = 0; i <= j; i++)
                  {
                    sum += fjac[ij] * (qtf[i] / fnorm);
                    ij += 1; /* fjac[i+m*j] */
                  }

                gnorm = Max(gnorm, fabs(sum / wa2[l]));
              }

            jj += funcdim;
          }
      }

    /*
     *   test for convergence of the gradient norm.
     */
    if (gnorm <= gtol)
      *info = 4;

    if (*info != 0)
      goto L300;

    /*
     *   rescale if necessary.
     */
    if (mode != 2)
      {
        for (j = 0; j < n; j++)
          diag[j] = Max(diag[j], wa2[j]);
      }

    /*
     *   beginning of the inner loop.
     */
L200:
    /*
     *      determine the levenberg-marquardt parameter.
     */
    lmpar(n, fjac, ldfjac, ipvt, diag, qtf, delta, &par, wa1, wa2);

    /*
     *      store the direction p and x + p. calculate the norm of p.
     */
    for (j = 0; j < n; j++)
      {
        wa1[j] = -wa1[j];
        wa2[j] = x[j] + wa1[j];
        wa3[j] = diag[j] * wa1[j];
      }

    pnorm = enorm(n, wa3);

    /*
     *      on the first iteration, adjust the initial step bound.
     */
    if (*iter == 1)
      delta = Min(delta, pnorm);

    /*
     *      evaluate the function at x + p and calculate its norm.
     */
    iflag = fcn(funcdim, n, wa2, wa4);

    if (iflag < 0)
      goto L300;

    fnorm1 = enorm(funcdim, wa4);
    /*
     *      compute the scaled actual reduction.
     */
    actred = -1.0;

    if ((p1 * fnorm1) < fnorm)
      {
        temp = fnorm1 / fnorm;
        actred = 1.0 - temp * temp;
      }

    /*
     *      compute the scaled predicted reduction and
     *      the scaled directional derivative.
     */
    jj = 0;

    for (j = 0; j < n; j++)
      {
        wa3[j] = 0.0;
        l = ipvt[j];
        temp = wa1[l];
        ij = jj;

        for (i = 0; i <= j; i++)
          {
            wa3[i] += fjac[ij] * temp;
            ij += 1; /* fjac[i+m*j] */
          }

        jj += funcdim;
      }

    temp1 = enorm(n, wa3) / fnorm;
    temp2 = (sqrt(par) * pnorm) / fnorm;
    prered = temp1 * temp1 + (temp2 * temp2) / p5;
    dirder = -(temp1 * temp1 + temp2 * temp2);
    /*
     *      compute the ratio of the actual to the predicted
     *      reduction.
     */
    ratio = 0.0;

    if (prered != 0.0)
      ratio = actred / prered;

    /*
     *      update the step bound.
     */
    if (ratio <= p25)
      {
        if (actred >= 0.0)
          temp = p5;
        else
          temp = p5 * dirder / (dirder + p5 * actred);

        if (((p1 * fnorm1) >= fnorm)
            || (temp < p1))
          temp = p1;

        delta = temp * Min(delta, pnorm / p1);
        par = par / temp;
      }
    else
      {
        if ((par == 0.0) || (ratio >= p75))
          {
            delta = pnorm / p5;
            par = p5 * par;
          }
      }

    /*
     *      test for successful iteration.
     */
    if (ratio >= p0001)
      {
        /*
         *      successful iteration. update x, fvec, and their norms.
         */
        for (j = 0; j < n; j++)
          {
            x[j] = wa2[j];
            wa2[j] = diag[j] * x[j];
          }

        for (i = 0; i < funcdim; i++)
          fvec[i] = wa4[i];

        xnorm = enorm(n, wa2);
        fnorm = fnorm1;
        (*iter) += 1;
      }

    cycles++;

    /*
     *      tests for convergence.
     */
    if ((fabs(actred) <= ftol)
        && (prered <= ftol)
        && (p5 * ratio <= 1.0))
      *info = 1;

    if (delta <= xtol * xnorm)
      *info = 2;

    if ((fabs(actred) <= ftol)
        && (prered <= ftol)
        && (p5 * ratio <= 1.0)
        && (*info == 2))
      *info = 3;

    if (*info != 0)
      goto L300;

    /*
     *      tests for termination and stringent tolerances.
     */
    if (cycles >= maxiter)
      *info = 5;

    if ((fabs(actred) <= MACHEP)
        && (prered <= MACHEP)
        && (p5 * ratio <= 1.0))
      *info = 6;

    if (delta <= MACHEP * xnorm)
      *info = 7;

    if (gnorm <= MACHEP)
      *info = 8;

    if (*info != 0)
      goto L300;

    /*
     *      end of the inner loop. repeat if iteration unsuccessful.
     */
    if (ratio < p0001)
      goto L200;

    /*
     *   end of the outer loop.
     */
    goto L30;

L300:

    /*
     *     termination, either normal or user imposed.
     */
    if (iflag < 0)
      *info = iflag;

    iflag = 0;

    if (nprint > 0)
      iflag = fcn(funcdim, n, x, fvec);

    delete [] wa1;
    delete [] wa2;
    delete [] wa3;
    delete [] wa4;
    return 0;
  }
  /************************lmpar.c*************************/

  int lmpar(int n, double r[], int ldr, int ipvt[],
            double diag[], double qtb[], double delta,
            double* par, double x[],
            double sdiag[])
  {
    /*     **********
     *
     *     subroutine lmpar
     *
     *     given an m by n matrix a, an n by n nonsingular diagonal
     *     matrix d, an m-vector b, and a positive number delta,
     *     the problem is to determine a value for the parameter
     *     par such that if x solves the system
     *
     *      a*x = b ,   sqrt(par)*d*x = 0 ,
     *
     *     in the least squares sense, and dxnorm is the euclidean
     *     norm of d*x, then either par is zero and
     *
     *      (dxnorm-delta) .le. 0.1*delta ,
     *
     *     or par is positive and
     *
     *      abs(dxnorm-delta) .le. 0.1*delta .
     *
     *     this subroutine completes the solution of the problem
     *     if it is provided with the necessary information from the
     *     qr factorization, with column pivoting, of a. that is, if
     *     a*p = q*r, where p is a permutation matrix, q has orthogonal
     *     columns, and r is an upper triangular matrix with diagonal
     *     elements of nonincreasing magnitude, then lmpar expects
     *     the full upper triangle of r, the permutation matrix p,
     *     and the first n components of (q transpose)*b. on output
     *     lmpar also provides an upper triangular matrix s such that
     *
     *       t   t         t
     *      p *(a *a + par*d*d)*p = s *s .
     *
     *     s is employed within lmpar and may be of separate interest.
     *
     *     only a few iterations are generally needed for convergence
     *     of the algorithm. if, however, the limit of 10 iterations
     *     is reached, then the output par will contain the best
     *     value obtained so far.
     *
     *     the subroutine statement is
     *
     *  subroutine lmpar(n,r,ldr,ipvt,diag,qtb,delta,par,x,sdiag,
     *       wa1,wa2)
     *
     *     where
     *
     *  n is a positive integer input variable set to the order of r.
     *
     *  r is an n by n array. on input the full upper triangle
     *    must contain the full upper triangle of the matrix r.
     *    on output the full upper triangle is unaltered, and the
     *    strict lower triangle contains the strict upper triangle
     *    (transposed) of the upper triangular matrix s.
     *
     *  ldr is a positive integer input variable not less than n
     *    which specifies the leading dimension of the array r.
     *
     *  ipvt is an integer input array of length n which defines the
     *    permutation matrix p such that a*p = q*r. column j of p
     *    is column ipvt(j) of the identity matrix.
     *
     *  diag is an input array of length n which must contain the
     *    diagonal elements of the matrix d.
     *
     *  qtb is an input array of length n which must contain the first
     *    n elements of the vector (q transpose)*b.
     *
     *  delta is a positive input variable which specifies an upper
     *    bound on the euclidean norm of d*x.
     *
     *  par is a nonnegative variable. on input par contains an
     *    initial estimate of the levenberg-marquardt parameter.
     *    on output par contains the final estimate.
     *
     *  x is an output array of length n which contains the least
     *    squares solution of the system a*x = b, sqrt(par)*d*x = 0,
     *    for the output par.
     *
     *  sdiag is an output array of length n which contains the
     *    diagonal elements of the upper triangular matrix s.
     *
     *
     *     subprograms called
     *
     *  minpack-supplied ... dpmpar,enorm,qrsolv
     *
     *  fortran-supplied ... dabs,dmax1,dmin1,dsqrt
     *
     *     argonne national laboratory. minpack project. march 1980.
     *     burton s. garbow, kenneth e. hillstrom, jorge j. more
     *
     *     **********
     */
    int i, iter, ij, jj, j, jm1, jp1, k, l, nsing;
    double dxnorm, fp, gnorm, parc, parl, paru;
    double sum, temp;
    static double p1 = 0.1;
    static double p001 = 0.001;
    extern double DWARF;

    //  wa1 and wa2 are work arrays of length n.
    double* wa1 = new double [n];
    double* wa2 = new double [n];

    /*
     *     compute and store in x the gauss-newton direction. if the
     *     jacobian is rank-deficient, obtain a least squares solution.
     */
    nsing = n;
    jj = 0;

    for (j = 0; j < n; j++)
      {
        wa1[j] = qtb[j];

        if ((r[jj] == 0.0) && (nsing == n))
          nsing = j;

        if (nsing < n)
          wa1[j] = 0.0;

        jj += ldr + 1; /* [j+ldr*j] */
      }

    if (nsing >= 1)
      {
        for (k = 0; k < nsing; k++)
          {
            j = nsing - k - 1;
            wa1[j] = wa1[j] / r[j + ldr * j];
            temp = wa1[j];
            jm1 = j - 1;

            if (jm1 >= 0)
              {
                ij = ldr * j;

                for (i = 0; i <= jm1; i++)
                  {
                    wa1[i] -= r[ij] * temp;
                    ij += 1;
                  }
              }
          }
      }

    for (j = 0; j < n; j++)
      {
        l = ipvt[j];
        x[l] = wa1[j];
      }

    /*
     *     initialize the iteration counter.
     *     evaluate the function at the origin, and test
     *     for acceptance of the gauss-newton direction.
     */
    iter = 0;

    for (j = 0; j < n; j++)
      wa2[j] = diag[j] * x[j];

    dxnorm = enorm(n, wa2);
    fp = dxnorm - delta;

    if (fp <= p1 * delta)
      {
        goto L220;
      }

    /*
     *     if the jacobian is not rank deficient, the newton
     *     step provides a lower bound, parl, for the zero of
     *     the function. otherwise set this bound to zero.
     */
    parl = 0.0;

    if (nsing >= n)
      {
        for (j = 0; j < n; j++)
          {
            l = ipvt[j];
            wa1[j] = diag[l] * (wa2[l] / dxnorm);
          }

        jj = 0;

        for (j = 0; j < n; j++)
          {
            sum = 0.0;
            jm1 = j - 1;

            if (jm1 >= 0)
              {
                ij = jj;

                for (i = 0; i <= jm1; i++)
                  {
                    sum += r[ij] * wa1[i];
                    ij += 1;
                  }
              }

            wa1[j] = (wa1[j] - sum) / r[j + ldr * j];
            jj += ldr; /* [i+ldr*j] */
          }

        temp = enorm(n, wa1);
        parl = ((fp / delta) / temp) / temp;
      }

    /*
     *     calculate an upper bound, paru, for the zero of the function.
     */
    jj = 0;

    for (j = 0; j < n; j++)
      {
        sum = 0.0;
        ij = jj;

        for (i = 0; i <= j; i++)
          {
            sum += r[ij] * qtb[i];
            ij += 1;
          }

        l = ipvt[j];
        wa1[j] = sum / diag[l];
        jj += ldr; /* [i+ldr*j] */
      }

    gnorm = enorm(n, wa1);
    paru = gnorm / delta;

    if (paru == 0.0)
      paru = DWARF / Min(delta, p1);

    /*
     *     if the input par lies outside of the interval (parl,paru),
     *     set par to the closer endpoint.
     */
    *par = Max(*par, parl);
    *par = Min(*par, paru);

    if (*par == 0.0)
      *par = gnorm / dxnorm;

    /*
     *     beginning of an iteration.
     */
L150:
    iter += 1;

    /*
     *   evaluate the function at the current value of par.
     */
    if (*par == 0.0)
      *par = Max(DWARF, p001 * paru);

    temp = sqrt(*par);

    for (j = 0; j < n; j++)
      wa1[j] = temp * diag[j];

    qrsolv(n, r, ldr, ipvt, wa1, qtb, x, sdiag);

    for (j = 0; j < n; j++)
      wa2[j] = diag[j] * x[j];

    dxnorm = enorm(n, wa2);
    temp = fp;
    fp = dxnorm - delta;

    /*
     *   if the function is small enough, accept the current value
     *   of par. also test for the exceptional cases where parl
     *   is zero or the number of iterations has reached 10.
     */
    if ((fabs(fp) <= p1 * delta)
        || ((parl == 0.0) && (fp <= temp) && (temp < 0.0))
        || (iter == 10))
      goto L220;

    /*
     *   compute the newton correction.
     */
    for (j = 0; j < n; j++)
      {
        l = ipvt[j];
        wa1[j] = diag[l] * (wa2[l] / dxnorm);
      }

    jj = 0;

    for (j = 0; j < n; j++)
      {
        wa1[j] = wa1[j] / sdiag[j];
        temp = wa1[j];
        jp1 = j + 1;

        if (jp1 < n)
          {
            ij = jp1 + jj;

            for (i = jp1; i < n; i++)
              {
                wa1[i] -= r[ij] * temp;
                ij += 1; /* [i+ldr*j] */
              }
          }

        jj += ldr; /* ldr*j */
      }

    temp = enorm(n, wa1);
    parc = ((fp / delta) / temp) / temp;

    /*
     *   depending on the sign of the function, update parl or paru.
     */
    if (fp > 0.0)
      parl = Max(parl, *par);

    if (fp < 0.0)
      paru = Min(paru, *par);

    /*
     *   compute an improved estimate for par.
     */
    *par = Max(parl, *par + parc);
    /*
     *   end of an iteration.
     */
    goto L150;

L220:

    /*
     *     termination.
     */
    if (iter == 0)
      *par = 0.0;

    delete [] wa1;
    delete [] wa2;
    return 0;
  }
  /************************qrfac.c*************************/
  int qrfac(int rows, int cols, double a[], int lda,
            int pivot, int ipvt[], int lipvt,
            double rdiag[], double acnorm[])
  {
    /*
     *     **********
     *
     *     subroutine qrfac
     *
     *     this subroutine uses householder transformations with column
     *     pivoting (optional) to compute a qr factorization of the
     *     m by n matrix a. that is, qrfac determines an orthogonal
     *     matrix q, a permutation matrix p, and an upper trapezoidal
     *     matrix r with diagonal elements of nonincreasing magnitude,
     *     such that a*p = q*r. the householder transformation for
     *     column k, k = 1,2,...,min(m,n), is of the form
     *
     *          t
     *      i - (1/u(k))*u*u
     *
     *     where u has zeros in the first k-1 positions. the form of
     *     this transformation and the method of pivoting first
     *     appeared in the corresponding linpack subroutine.
     *
     *  rows is a positive integer input variable set to the number
     *    of rows of a.
     *
     *  cols is a positive integer input variable set to the number
     *    of columns of a.
     *
     *  a is an m by n array. on input a contains the matrix for
     *    which the qr factorization is to be computed. on output
     *    the strict upper trapezoidal part of a contains the strict
     *    upper trapezoidal part of r, and the lower trapezoidal
     *    part of a contains a factored form of q (the non-trivial
     *    elements of the u vectors described above).
     *
     *  lda is a positive integer input variable not less than m
     *    which specifies the leading dimension of the array a.
     *
     *  pivot is a logical input variable. if pivot is set true,
     *    then column pivoting is enforced. if pivot is set false,
     *    then no column pivoting is done.
     *
     *  ipvt is an integer output array of length lipvt. ipvt
     *    defines the permutation matrix p such that a*p = q*r.
     *    column j of p is column ipvt(j) of the identity matrix.
     *    if pivot is false, ipvt is not referenced.
     *
     *  lipvt is a positive integer input variable. if pivot is false,
     *    then lipvt may be as small as 1. if pivot is true, then
     *    lipvt must be at least n.
     *
     *  rdiag is an output array of length n which contains the
     *    diagonal elements of r.
     *
     *  acnorm is an output array of length n which contains the
     *    norms of the corresponding columns of the input matrix a.
     *    if this information is not needed, then acnorm can coincide
     *    with rdiag.
     *
     *     argonne national laboratory. minpack project. march 1980.
     *     burton s. garbow, kenneth e. hillstrom, jorge j. more
     *
     *     **********
     */
    int i, ij, jj, j, jp1, k, kmax, minmn;
    double ajnorm, sum, temp;
    static double p05 = 0.05;
    //  wa is a work array of length cols
    double* wa = new double[cols];
    /*
     *     compute the initial column norms and initialize several arrays.
     */
    ij = 0;

    for (j = 0; j < cols; j++)
      {
        acnorm[j] = enorm(rows, &a[ij]);
        rdiag[j] = acnorm[j];
        wa[j] = rdiag[j];

        if (pivot != 0)
          ipvt[j] = j;

        ij += rows; /* m*j */
      }

    /*
     *     reduce a to r with householder transformations.
     */
    minmn = Min(rows, cols);

    for (j = 0; j < minmn; j++)
      {
        if (pivot != 0)
          {
            /*
             *   bring the column of largest norm into the pivot position.
             */
            kmax = j;

            for (k = j; k < cols; k++)
              {
                if (rdiag[k] > rdiag[kmax])
                  kmax = k;
              }

            if (kmax != j)
              {

                ij = rows * j;
                jj = rows * kmax;

                for (i = 0; i < rows; i++)
                  {
                    temp = a[ij]; /* [i+m*j] */
                    a[ij] = a[jj]; /* [i+m*kmax] */
                    a[jj] = temp;
                    ij += 1;
                    jj += 1;
                  }

                rdiag[kmax] = rdiag[j];
                wa[kmax] = wa[j];
                k = ipvt[j];
                ipvt[j] = ipvt[kmax];
                ipvt[kmax] = k;
              }
          }

        /*
         *   compute the householder transformation to reduce the
         *   j-th column of a to a multiple of the j-th unit vector.
         */
        jj = j + rows * j;
        ajnorm = enorm(rows - j, &a[jj]);

        if (ajnorm != 0.0)
          {
            if (a[jj] < 0.0)
              ajnorm = -ajnorm;

            ij = jj;

            for (i = j; i < rows; i++)
              {
                a[ij] /= ajnorm;
                ij += 1; /* [i+m*j] */
              }

            a[jj] += 1.0;
            /*
             *   apply the transformation to the remaining columns
             *   and update the norms.
             */
            jp1 = j + 1;

            if (jp1 < cols)
              {
                for (k = jp1; k < cols; k++)
                  {
                    sum = 0.0;
                    ij = j + rows * k;
                    jj = j + rows * j;

                    for (i = j; i < rows; i++)
                      {
                        sum += a[jj] * a[ij];
                        ij += 1; /* [i+m*k] */
                        jj += 1; /* [i+m*j] */
                      }

                    temp = sum / a[j + rows * j];
                    ij = j + rows * k;
                    jj = j + rows * j;

                    for (i = j; i < rows; i++)
                      {
                        a[ij] -= temp * a[jj];
                        ij += 1; /* [i+m*k] */
                        jj += 1; /* [i+m*j] */
                      }

                    if ((pivot != 0) && (rdiag[k] != 0.0))
                      {
                        temp = a[j + rows * k] / rdiag[k];
                        temp = Max(0.0, 1.0 - temp * temp);
                        rdiag[k] *= sqrt(temp);
                        temp = rdiag[k] / wa[k];

                        if ((p05 * temp * temp) <= MACHEP)
                          {
                            rdiag[k] = enorm(rows - j - 1, &a[jp1 + rows * k]);
                            wa[k] = rdiag[k];
                          }
                      }
                  }
              }
          }

        rdiag[j] = -ajnorm;
      }

    delete [] wa;
    return 0;
  }
  /************************qrsolv.c*************************/
  int qrsolv(int n, double r[], int ldr, int ipvt[],
             double diag[], double qtb[], double x[],
             double sdiag[])
  {
    /*
     *     **********
     *
     *     subroutine qrsolv
     *
     *     given an m by n matrix a, an n by n diagonal matrix d,
     *     and an m-vector b, the problem is to determine an x which
     *     solves the system
     *
     *      a*x = b ,   d*x = 0 ,
     *
     *     in the least squares sense.
     *
     *     this subroutine completes the solution of the problem
     *     if it is provided with the necessary information from the
     *     qr factorization, with column pivoting, of a. that is, if
     *     a*p = q*r, where p is a permutation matrix, q has orthogonal
     *     columns, and r is an upper triangular matrix with diagonal
     *     elements of nonincreasing magnitude, then qrsolv expects
     *     the full upper triangle of r, the permutation matrix p,
     *     and the first n components of (q transpose)*b. the system
     *     a*x = b, d*x = 0, is then equivalent to
     *
     *       t     t
     *      r*z = q *b ,  p *d*p*z = 0 ,
     *
     *     where x = p*z. if this system does not have full rank,
     *     then a least squares solution is obtained. on output qrsolv
     *     also provides an upper triangular matrix s such that
     *
     *       t   t     t
     *      p *(a *a + d*d)*p = s *s .
     *
     *     s is computed within qrsolv and may be of separate interest.
     *
     *  n is a positive integer input variable set to the order of r.
     *
     *  r is an n by n array. on input the full upper triangle
     *    must contain the full upper triangle of the matrix r.
     *    on output the full upper triangle is unaltered, and the
     *    strict lower triangle contains the strict upper triangle
     *    (transposed) of the upper triangular matrix s.
     *
     *  ldr is a positive integer input variable not less than n
     *    which specifies the leading dimension of the array r.
     *
     *  ipvt is an integer input array of length n which defines the
     *    permutation matrix p such that a*p = q*r. column j of p
     *    is column ipvt(j) of the identity matrix.
     *
     *  diag is an input array of length n which must contain the
     *    diagonal elements of the matrix d.
     *
     *  qtb is an input array of length n which must contain the first
     *    n elements of the vector (q transpose)*b.
     *
     *  x is an output array of length n which contains the least
     *    squares solution of the system a*x = b, d*x = 0.
     *
     *  sdiag is an output array of length n which contains the
     *    diagonal elements of the upper triangular matrix s.
     *
     *     argonne national laboratory. minpack project. march 1980.
     *     burton s. garbow, kenneth e. hillstrom, jorge j. more
     *
     *     **********
     */
    int i, ij, ik, kk, j, jp1, k, kp1, l, nsing;
    double cos, cotan, qtbpj, sin, sum, tan, temp;
    static double p25 = 0.25;
    static double p5 = 0.5;
    //  wa is a work array of length n.
    double* wa = new double[n];

    /*
     *     copy r and (q transpose)*b to preserve input and initialize s.
     *     in particular, save the diagonal elements of r in x.
     */
    kk = 0;

    for (j = 0; j < n; j++)
      {
        ij = kk;
        ik = kk;

        for (i = j; i < n; i++)
          {
            r[ij] = r[ik];
            ij += 1;   /* [i+ldr*j] */
            ik += ldr; /* [j+ldr*i] */
          }

        x[j] = r[kk];
        wa[j] = qtb[j];
        kk += ldr + 1; /* j+ldr*j */
      }

    /*
     *     eliminate the diagonal matrix d using a givens rotation.
     */
    for (j = 0; j < n; j++)
      {
        /*
         *   prepare the row of d to be eliminated, locating the
         *   diagonal element using p from the qr factorization.
         */
        l = ipvt[j];

        if (diag[l] == 0.0)
          goto L90;

        for (k = j; k < n; k++)
          sdiag[k] = 0.0;

        sdiag[j] = diag[l];
        /*
         *   the transformations to eliminate the row of d
         *   modify only a single element of (q transpose)*b
         *   beyond the first n, which is initially zero.
         */
        qtbpj = 0.0;

        for (k = j; k < n; k++)
          {
            /*
             *      determine a givens rotation which eliminates the
             *      appropriate element in the current row of d.
             */
            if (sdiag[k] == 0.0)
              continue;

            kk = k + ldr * k;

            if (fabs(r[kk]) < fabs(sdiag[k]))
              {
                cotan = r[kk] / sdiag[k];
                sin = p5 / sqrt(p25 + p25 * cotan * cotan);
                cos = sin * cotan;
              }
            else
              {
                tan = sdiag[k] / r[kk];
                cos = p5 / sqrt(p25 + p25 * tan * tan);
                sin = cos * tan;
              }

            /*
             *      compute the modified diagonal element of r and
             *      the modified element of ((q transpose)*b,0).
             */
            r[kk] = cos * r[kk] + sin * sdiag[k];
            temp = cos * wa[k] + sin * qtbpj;
            qtbpj = -sin * wa[k] + cos * qtbpj;
            wa[k] = temp;
            /*
             *      accumulate the tranformation in the row of s.
             */
            kp1 = k + 1;

            if (n > kp1)
              {
                ik = kk + 1;

                for (i = kp1; i < n; i++)
                  {
                    temp = cos * r[ik] + sin * sdiag[i];
                    sdiag[i] = -sin * r[ik] + cos * sdiag[i];
                    r[ik] = temp;
                    ik += 1; /* [i+ldr*k] */
                  }
              }
          }

L90:
        /*
         *   store the diagonal element of s and restore
         *   the corresponding diagonal element of r.
         */
        kk = j + ldr * j;
        sdiag[j] = r[kk];
        r[kk] = x[j];
      }

    /*
     *     solve the triangular system for z. if the system is
     *     singular, then obtain a least squares solution.
     */
    nsing = n;

    for (j = 0; j < n; j++)
      {
        if ((sdiag[j] == 0.0) && (nsing == n))
          nsing = j;

        if (nsing < n)
          wa[j] = 0.0;
      }

    if (nsing < 1)
      goto L150;

    for (k = 0; k < nsing; k++)
      {
        j = nsing - k - 1;
        sum = 0.0;
        jp1 = j + 1;

        if (nsing > jp1)
          {
            ij = jp1 + ldr * j;

            for (i = jp1; i < nsing; i++)
              {
                sum += r[ij] * wa[i];
                ij += 1; /* [i+ldr*j] */
              }
          }

        wa[j] = (wa[j] - sum) / sdiag[j];
      }

L150:

    /*
     *     permute the components of z back to components of x.
     */
    for (j = 0; j < n; j++)
      {
        l = ipvt[j];
        x[l] = wa[j];
      }

    delete [] wa;
    return 0;
  }
  /************************enorm.c*************************/

  double enorm(int n, double x[])
  {
    /*
     *     **********
     *
     *     function enorm
     *
     *     given an n-vector x, this function calculates the
     *     euclidean norm of x.
     *
     *     the euclidean norm is computed by accumulating the sum of
     *     squares in three different sums. the sums of squares for the
     *     small and large components are scaled so that no overflows
     *     occur. non-destructive underflows are permitted. underflows
     *     and overflows do not occur in the computation of the unscaled
     *     sum of squares for the intermediate components.
     *     the definitions of small, intermediate and large components
     *     depend on two constants, rdwarf and rgiant. the main
     *     restrictions on these constants are that rdwarf**2 not
     *     underflow and rgiant**2 not overflow. the constants
     *     given here are suitable for every known computer.
     *
     *  n is a positive integer input variable.
     *
     *  x is an input array of length n.
     *
     *     argonne national laboratory. minpack project. march 1980.
     *     burton s. garbow, kenneth e. hillstrom, jorge j. more
     *
     *     **********
     */
    int i;
    double agiant, floatn, s1, s2, s3, xabs, x1max, x3max;
    double ans, temp;
    static double rdwarf = 3.834e-20;
    static double rgiant = 1.304e19;

    s1 = 0.0;
    s2 = 0.0;
    s3 = 0.0;
    x1max = 0.0;
    x3max = 0.0;
    floatn = n;
    agiant = rgiant / floatn;

    for (i = 0; i < n; i++)
      {
        xabs = fabs(x[i]);

        if ((xabs > rdwarf) && (xabs < agiant))
          {
            /*
             *      sum for intermediate components.
             */
            s2 += xabs * xabs;
            continue;
          }

        if (xabs > rdwarf)
          {
            /*
             *         sum for large components.
             */
            if (xabs > x1max)
              {
                temp = x1max / xabs;
                s1 = 1.0 + s1 * temp * temp;
                x1max = xabs;
              }
            else
              {
                temp = xabs / x1max;
                s1 += temp * temp;
              }

            continue;
          }

        /*
         *         sum for small components.
         */
        if (xabs > x3max)
          {
            temp = x3max / xabs;
            s3 = 1.0 + s3 * temp * temp;
            x3max = xabs;
          }
        else
          {
            if (xabs != 0.0)
              {
                temp = xabs / x3max;
                s3 += temp * temp;
              }
          }
      }

    /*
     *     calculation of norm.
     */
    if (s1 != 0.0)
      {
        temp = s1 + (s2 / x1max) / x1max;
        ans = x1max * sqrt(temp);
        return (ans);
      }

    if (s2 != 0.0)
      {
        if (s2 >= x3max)
          temp = s2 * (1.0 + (x3max / s2) * (x3max * s3));
        else
          temp = x3max * ((s2 / x3max) + (x3max * s3));

        ans = sqrt(temp);
      }
    else
      {
        ans = x3max * sqrt(s3);
      }

    return ans;
  }

  /************************fdjac2.c*************************/

  int fdjac2(lmfunc fcn,
             int m, int n, double x[], double fvec[],
             double fjac[],
             int ldfjac, int* iflag, double epsfcn)
  {
    /*
     *     **********
     *
     *     subroutine fdjac2
     *
     *     this subroutine computes a forward-difference approximation
     *     to the m by n jacobian matrix associated with a specified
     *     problem of m functions in n variables.
     *
     *  fcn is the name of the user-supplied subroutine which
     *    calculates the functions. fcn must be declared
     *    in an external statement in the user calling
     *    program, and should be written as follows.
     *
     *    subroutine fcn(m,n,x,fvec,iflag)
     *    integer m,n,iflag
     *    double precision x(n),fvec(m)
     *    ----------
     *    calculate the functions at x and
     *    return this vector in fvec.
     *    ----------
     *    return
     *    end
     *
     *    the value of iflag should not be changed by fcn unless
     *    the user wants to terminate execution of fdjac2.
     *    in this case set iflag to a negative integer.
     *
     *  m is a positive integer input variable set to the number
     *    of functions.
     *
     *  n is a positive integer input variable set to the number
     *    of variables. n must not exceed m.
     *
     *  x is an input array of length n.
     *
     *  fvec is an input array of length m which must contain the
     *    functions evaluated at x.
     *
     *  fjac is an output m by n array which contains the
     *    approximation to the jacobian matrix evaluated at x.
     *
     *  ldfjac is a positive integer input variable not less than m
     *    which specifies the leading dimension of the array fjac.
     *
     *  iflag is an integer variable which can be used to terminate
     *    the execution of fdjac2. see description of fcn.
     *
     *  epsfcn is an input variable used in determining a suitable
     *    step length for the forward-difference approximation. this
     *    approximation assumes that the relative errors in the
     *    functions are of the order of epsfcn. if epsfcn is less
     *    than the machine precision, it is assumed that the relative
     *    errors in the functions are of the order of the machine
     *    precision.
     *
     *     subprograms called
     *
     *  user-supplied ...... fcn
     *
     *  minpack-supplied ... dpmpar
     *
     *  fortran-supplied ... dabs,dmax1,dsqrt
     *
     *     argonne national laboratory. minpack project. march 1980.
     *     burton s. garbow, kenneth e. hillstrom, jorge j. more
     *
     **********
     */
    int i, j, ij;
    double eps, delta_xj, temp;

    //  wa is a work array of length m.
    double* wa = new double[m];

    eps = sqrt(Max(epsfcn, MACHEP));
    ij = 0;

    *iflag = fcn(m, n, x, wa); //!!!

    if (*iflag < 0)
      {
        delete [] wa;
        return 0;
      }

    for (j = 0; j < n; j++)
      {
        temp = x[j];
        delta_xj = eps * fabs(temp);

        if (delta_xj == 0.0)
          delta_xj = eps;

        x[j] = temp + delta_xj;

        *iflag = fcn(m, n, x, wa);

        if (*iflag < 0)
          {
            delete [] wa;
            return 0;
          }

        x[j] = temp;

        for (i = 0; i < m; i++)
          {
            fjac[ij] = (wa[i] - fvec[i]) / delta_xj;
            ij ++;  /* fjac[i+m*j] */
          }
      }

    delete [] wa;
    return 0;
  }

  int lmdifx(lmfunc* fnc, int funcdim, int onr, double x[],
             double ftol, double xtol, double gtol,
             int maxiter, double epsfcn, double factor,
             int nprint, int* info, int* nfev,
             int fd1)
  {
    // Variablen für lmdif-Aufruf
    double* v = new double[funcdim];
    double* diag = new double[onr];

    double* fjac = new double[funcdim * onr];

    int* ipvt = new int[onr];
    double* qtf = new double[onr];

    int rc = lmdif(fnc, funcdim, onr, x, v,
                   ftol, xtol, gtol, maxiter, epsfcn,
                   diag, 1, // mode equals to 1, because diag is uninitialized
                   factor,
                   nprint, info, nfev,
                   fjac, fd1, ipvt, qtf);

    delete [] v;
    delete [] diag;
    delete [] fjac;
    delete [] ipvt;
    delete [] qtf;
    return rc;
  }

  std::string LMDifMessage(int info)
  {
    switch (info)
      {
      case 0:
        return "improper input parameters";
      case 1:
        return "both actual and predicted relative reductions in "
               "the sum of squares are at most ftol.";
      case 2:
        return "relative error between two consecutive iterates is at most xtol.";
      case 3:
        return "actual and predicted relative reductions are at most ftol and "
               "relative error between two consecutive iterates is at most xtol.";
      case 4:
        return "the cosine of the angle between fvec and any column of the"
               " jacobian is at most gtol in absolute value.";
      case 5:
        return "number of iterations has reached maxiter.";
      case 6:
        return "ftol is too small. no further reduction in the sum of squares"
               " is possible.";
      case 7:
        return "xtol is too small. no further improvement in the approximate "
               "solution x is possible.";
      case 8:
        return "gtol is too small. fvec is orthogonal to the columns of the "
               "jacobian to machine precision.";
      }

    return "Unknown info number";
  }
}
