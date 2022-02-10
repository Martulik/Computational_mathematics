#include <iostream>
#include <iomanip>
#include <FORSYTHE.H>
#include "data.hpp"

std::vector< std::vector< double>> inverse_matrix(DECOMP *const decomp)
{
  std::vector< double > e(size_of_matrix, 0);
  std::vector< double > b(size_of_matrix);
  std::vector< std::vector< double>> inv_a(size_of_matrix, std::vector< double >(size_of_matrix));
  for (int i = 0; i < size_of_matrix; ++i) {
    e.at(i) = 1;
    if (i != 0) {
      e.at(i - 1) = 0;
    }
    b = e;
    decomp->Solve(b);
    for (int j = 0; j < size_of_matrix; ++j) {
      inv_a[j][i] = b[j];
    }
  }
  return inv_a;
}

std::vector< std::vector< double>> matrix_multiplication(const std::vector< std::vector< double>> &a,
                                                         const std::vector< std::vector< double>> &b)
{
  std::vector< std::vector< double>> c(a.size(), std::vector< double >(b.begin()->size()));
  for (int i = 0; i < a.size(); i++) {
    for (int j = 0; j < b.begin()->size(); j++) {
      c[i][j] = 0;
      for (int k = 0; k < a.begin()->size(); k++) {
        c[i][j] += (a[i][k] * b[k][j]);
      }
    }
  }
  return c;
}

void output(const double p_i, const double cond_a, const double cond_at, const double delta)
{
  std::cout << std::setw(20) << "P" << std::setw(30) << "COND(A)" << std::setw(30) << "COND(A^t*A)" << "delta" << '\n';
  std::cout << std::setw(20) << p_i << std::setw(30) << cond_a << std::setw(30) << cond_at << delta << '\n';
  std::cout << "--------------------------------------------------\n";
}

double matrix_norm(const std::vector< std::vector< double>> &matrix) // по строкам
{
  double max = 0.0;
  for (int i = 0; i < size_of_matrix; ++i) {
    double sum = 0.0;
    for (int j = 0; j < size_of_matrix; ++j) {
      sum += abs(matrix[i][j]);
    }
    max = max > sum ? max : sum;
  }
  return max;
}

std::vector< std::vector< double>> transpose_matrix(const std::vector< std::vector< double>> &a)
{
  std::vector< std::vector< double>> trans_a(a);
  for (int i = 0; i < size_of_matrix; ++i) {
    for (int j = i; j < size_of_matrix; ++j) {
      std::swap(trans_a[i][j], trans_a[j][i]);
    }
  }
  return trans_a;
}

double calculate_delta(std::vector< double > x1, std::vector< double > x2)
{
  std::vector< double > c;
  c.reserve(x1.size());
  for (int i = 0; i < x1.size(); ++i) {
    c.push_back(x1.at(i) - x2.at(i));
  }
  double norm_c =
    *std::max(c.begin(), c.end()) > abs(*std::min(c.begin(), c.end())) ? *std::max(c.begin(), c.end()) :
    abs(*std::min(c.begin(), c.end()));
  double norm_x1 =
    *std::max(x1.begin(), x1.end()) > abs(*std::min(x1.begin(), x1.end())) ? *std::max(x1.begin(), x1.end()) :
    abs(*std::min(x1.begin(), x1.end()));
  double delta = norm_c / norm_x1;
  return delta;
}

void solve_systems()
{
  for (int i = 0; i < p_size; ++i) {
    std::vector< std::vector< double>> a = A(p[i]);
    std::vector< double > b1 = B(p[i]);
    DECOMP decomp(size_of_matrix, a);
    decomp.Solve(b1);

    std::vector< std::vector< double>> inv_a = inverse_matrix(&decomp);
    double cond1 = matrix_norm(a) * matrix_norm(inv_a);

    std::vector< double > b2 = B(p[0]);
    std::vector< std::vector< double>> trans_a = transpose_matrix(a);
    std::vector< std::vector< double>> trans_a_a = matrix_multiplication(trans_a, a);
    DECOMP decomp2(size_of_matrix, trans_a_a);

    std::vector< double > trans_a_b2(b2.size(), 0);
    for (int i = 0; i < trans_a_b2.size(); ++i) {
      for (int j = 0; j < trans_a_b2.size(); ++j) {
        trans_a_b2[i] += trans_a[i][j] * b2[j];
      }
    }
    decomp2.Solve(trans_a_b2);

    double delta = calculate_delta(b1, trans_a_b2);
    double cond2 = matrix_norm(trans_a_a) * matrix_norm(inverse_matrix(&decomp2));

    output(p[i], cond1, cond2, delta);
  }
}

int main()
{
  std::cout.setf(std::ios_base::fixed | std::ios_base::left);
  std::cout.precision(10);
  solve_systems();

  return 0;
}
