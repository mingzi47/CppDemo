#include "ThreadPool.hpp"
#include <functional>
#include <iostream>
#include <random>
#include <vector>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);
void simulate_hard_computation() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}
void multiply(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  printf("%d * %d = %d\n", a, b, res);
}
void multiply_output(int &out, const int a, const int b) {
  simulate_hard_computation();
  out = a * b;
  printf("%d * %d = %d\n", a, b, out);
}
int multiply_return(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  printf("%d * %d = %d\n", a, b, res);
  return res;
}
void example() {
  ctp::ThreadPool pool(10);
  for (int i = 1; i <= 3; ++i)
    for (int j = 1; j <= 10; ++j) {
      pool.submit(multiply, i, j);
    }
  int output_ref;
  auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);
  future1.get();
  printf("Last operation result is equals to %d\n", output_ref);
  auto future2 = pool.submit(multiply_return, 5, 3);
  int res = future2.get();
  printf("Last operation result is equals to %d\n", res);
}

int main() {
  example();
  return 0;
}
