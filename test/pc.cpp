#include "Galois/Substrate/PerThreadStorage.h"
#include "Galois/Timer.h"
#include "Galois/Galois.h"

#include <cstdlib>
#include <iostream>

using namespace galois::Substrate;

int num = 1;

template<typename T>
struct testL {
  PerThreadStorage<T>& b;

  testL(PerThreadStorage<T>& B) :b(B) {}
  void operator()(unsigned t, unsigned n) {
    for (int x = 0; x < num; ++x) {
      *b.getLocal() += x;
    }
  }
};

template<typename T>
struct testR {
  PerThreadStorage<T>& b;

  testR(PerThreadStorage<T>& B) :b(B) {}
  void operator()(unsigned t, unsigned n) {
    for (int x = 0; x < num; ++x) {
      *b.getRemote((t + 1) % n) += x;
    }
  }
};

template<typename T>
void testf(const char* str) {
  PerThreadStorage<T> b;
  std::cout << "\nRunning: " << str << " sizeof " << sizeof(PerThreadStorage<T>) << "\n";
  galois::Timer tL;
  tL.start();
  galois::on_each(testL<T>(b));
  tL.stop();
  galois::Timer tR;
  tR.start();
  galois::on_each(testR<T>(b));
  tR.stop();
  std::cout << str << " L: " << tL.get() << " R: " << tR.get() << '\n';
}

int main(int argc, char** argv) {
  if (argc > 1)
    num = atoi(argv[1]);
  if (num <= 0)
    num = 1024 * 1024 * 1024;

  unsigned M = galois::Substrate::getThreadPool().getMaxThreads();

  while (M) {
    galois::setActiveThreads(M); //galois::Runtime::LL::getMaxThreads());
    std::cout << "Using " << M << " threads\n";

    testf<int>("int");
    testf<double>("double");

    M /= 2;
  }

  return 0;
}
