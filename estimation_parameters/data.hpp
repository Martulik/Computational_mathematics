#ifndef DATA_HPP
#define DATA_HPP

#include <cmath>
#include "FORSYTHE.H"

double eps = 0.0001;
const double g = 9.81;
const int M = 1;
double k_global = 0.0;
const int N = 7;
const double x_exp[7] = {0.0, 0.303, -0.465, 0.592, -0.409, 0.164, 0.180};
double x_mod[7] = {0.0};

// MAIN - FMIN - F(p) - RKF45 - f (t, x, p)

double integral(double x)
{
  return std::cos(x * x);
}

double l()
{
  double result, flag;
  int nofun = 0;
  double errest = 0.0;
  QUANC8(integral, 0.0, 1.0, eps, eps, result, errest, nofun, flag);
  double l = result / 0.90452424;
  return l;
}

void f(const double k, const double *y, double *yp)
{
  yp[0] = y[1];
  yp[1] = -((k_global / M) * y[0] + g * (1 - std::cos(y[2])) - (l() + y[0]) * y[3]);
  yp[2] = y[3];
  yp[3] = -(g / (l() + y[0]) * std::sin(y[2]) + 2 / (l() + y[0]) + y[1] * y[3]);
}

void (*f_ptr)(double, double *, double *) = reinterpret_cast<void (*)(double, double *, double *)>(f);

void x_mod_fill()
{
  double tout = 0.4;
  int neqn = 4;
  double work[6 * 4 + 3] = {0.0};
  x_mod[0] = 0.0;
  int iflag = 1;
  for (int i = 1; i < N; ++i) {
    double t = 0.0;
    double x_mod_p[4] = {0.0, 0.0, 0.0, 4.0};
    RKF45(f_ptr, neqn, x_mod_p, t, tout, eps, eps, work, iflag);
    //std::cout << "tout = " << tout << '\n';
    //std::cout << "iflag = " << iflag << '\n';
    x_mod[i] = x_mod_p[0];
    //std::cout << "x_mod[i]  = " << x_mod[i] << '\n';
    tout += 0.4;
  }
}

double f_k(double k)
{
  k_global = k;
  double sum = 0.0;
  x_mod_fill();
  for (int i = 0; i < N; ++i) {
    sum += std::pow(x_exp[i] - x_mod[i], 2);
  }
  return sum;
}

#endif
