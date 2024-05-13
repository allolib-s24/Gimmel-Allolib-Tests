
#include <chrono>
static long long timeElapsed = 0L;
static long long iterations = 0L;

#define BENCHMARK_CODE(code) { \
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); \
        code \
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); \
        timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count(); \
		std::cout << timeElapsed << "ns" << std::endl; \
	}

#define BENCHMARK_CODE_AVG(code) { \
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); \
        code \
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); \
        timeElapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count(); \
        iterations++; \
        if (iterations > 48000 * 3) { \
			std::cout << timeElapsed / iterations << "ns avg every 3 sec" << std::endl; \
			timeElapsed = iterations = 0; \
		} \
   }
