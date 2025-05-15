#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cmath>     

#include <tuple>        
#include <stdexcept>    
#include <functional>

#include <BinaryArithmetic.h>
#include <FactorialArithmetic.h>
using namespace numeric;


namespace benchmark {

    class timer {
    public:
        // Запускает таймер — сохраняем текущее время
        void start() {
            start_time = std::chrono::high_resolution_clock::now();
        }

        // Завершает таймер и возвращает продолжительность в секундах
        double end() const {
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;
            return elapsed.count();
        }

    private:
        std::chrono::high_resolution_clock::time_point start_time;
    };

    // Helper function to generate a large string of a specific digit
    std::string make_big_string(char digit, size_t len) {
        if (len == 0) return "0";
        return std::string(len, digit);
    }

    template<typename T>
    class BenchmarkRunner {
    private:
        int iterations;
        std::string type_name; // Friendly name for the type

    public:
        BenchmarkRunner(int iters, const std::string& name)
            : iterations(iters), type_name(name) {
        }

        // --- Измерение времени конкретной операции ---
        // Принимает строку с названием операции и лямбда-функцию, выполняющую операцию

        double measure_operation(const std::string& op_name, std::function<void()> operation) const {
            timer t;
            try {
                t.start();
                operation(); // Execute the timed operation
                return t.end();
            }
            catch (const std::exception& e) {
                std::cerr << "    Error during " << op_name << " benchmark for " << type_name << ": " << e.what() << std::endl;
                return -1.0; // Indicate error
            }
            catch (...) {
                std::cerr << "    Unknown error during " << op_name << " benchmark for " << type_name << std::endl;
                return -1.0; // Indicate error
            }
        }

        // --- Запуск бенчмарков для всех операций на заданных значениях ---

        void run_all_operations(const std::string& scale_name, const T& val1, const T& val2, const T& divisor) const {
            std::cout << "\n  --- " << type_name << " (" << scale_name << " Data) ---" << std::endl;
            std::cout << "  Iterations: " << iterations << std::endl;
            std::cout << std::fixed << std::setprecision(6);

            T result; // Use a dummy variable to store results to prevent optimization skipping the operation

            // Addition
            double time_add = measure_operation("Addition", [&]() {
                for (int i = 0; i < iterations; ++i) { result = val1 + val2; }
                });
            if (time_add >= 0) std::cout << "    Addition:       " << time_add << " s" << std::endl;

            // Subtraction
            double time_sub = measure_operation("Subtraction", [&]() {
                for (int i = 0; i < iterations; ++i) { result = val1 - val2; }
                });
            if (time_sub >= 0) std::cout << "    Subtraction:    " << time_sub << " s" << std::endl;


            // Multiplication
            double time_mul = measure_operation("Multiplication", [&]() {
                for (int i = 0; i < iterations; ++i) { result = val1 * val2; }
                });
            if (time_mul >= 0) std::cout << "    Multiplication: " << time_mul << " s" << std::endl;

            // Division
             // Check for division by zero robustly (requires == operator or to_string)
            bool is_divisor_zero = false;
            try {
                if (divisor == T(0)) is_divisor_zero = true;
            }
            catch (...) {
                // If T(0) or operator== fails, try string conversion if available
                try {
                    if (divisor.to_string() == "0") is_divisor_zero = true;
                }
                catch (...) {
                    // Fallback: assume not zero or cannot check
                }
            }

            if (is_divisor_zero) {
                std::cout << "    Division:       Skipped (Division by zero)." << std::endl;
            }
            else {
                double time_div = measure_operation("Division", [&]() {
                    for (int i = 0; i < iterations; ++i) { result = val1 / divisor; }
                    });
                if (time_div >= 0) std::cout << "    Division:       " << time_div << " s" << std::endl;
            }

            // Modulo
            if (is_divisor_zero) {
                std::cout << "    Modulo:         Skipped (Modulo by zero)." << std::endl;
            }
            else {
                double time_mod = measure_operation("Modulo", [&]() {
                    for (int i = 0; i < iterations; ++i) { result = val1 % divisor; }
                    });
                if (time_mod >= 0) std::cout << "    Modulo:         " << time_mod << " s" << std::endl;
            }


            // Comparison (example with '<')
            bool cmp_result; // Dummy variable for comparison
            double time_cmp = measure_operation("Comparison", [&]() {
                for (int i = 0; i < iterations; ++i) { cmp_result = (val1 < val2); }
                });
            if (time_cmp >= 0) std::cout << "    Comparison (<): " << time_cmp << " s" << std::endl;

            // --- Construction from string (optional, but good to measure) ---
            // Note: This measures construction from num1_str string
            double time_construct = measure_operation("Construction", [&]() {
                // Assuming T has a constructor from const std::string&
                T constructed_val(val1.to_string()); // Convert back to string then reconstruct
                });
            if (time_construct >= 0) std::cout << "    Construction:   " << time_construct << " s" << std::endl;


            std::cout << "  -------------------------------------------" << std::endl;
        }
    };

   
    // Для BinaryArithmetic
    void run_benchmarks_binary_bigint(const std::string& scale_name,
        const std::string& num1_str,
        const std::string& num2_str,
        const std::string& divisor_str,
        int iterations) {
        std::cout << "\n--- Benchmarking Scale: " << scale_name << " ---" << std::endl;
        try {
            BinaryArithmetic num1(num1_str);
            BinaryArithmetic num2(num2_str);
            BinaryArithmetic divisor(divisor_str);

            BenchmarkRunner<BinaryArithmetic> runner(iterations, "Binary BigInt");
            runner.run_all_operations(scale_name, num1, num2, divisor);

        }
        catch (const std::exception& e) {
            std::cerr << "  Error setting up Binary BigInt benchmark for " << scale_name << ": " << e.what() << std::endl;
        }
    }

    // Для FactorialArithmetic
    void run_benchmarks_factorial_bigint(const std::string& scale_name,
        const std::string& num1_str,
        const std::string& num2_str,
        const std::string& divisor_str,
        int iterations) {
        std::cout << "\n--- Benchmarking Scale: " << scale_name << " ---" << std::endl;
        try {
            FactorialArithmetic num1(num1_str);
            FactorialArithmetic num2(num2_str);
            FactorialArithmetic divisor(divisor_str);

            BenchmarkRunner<FactorialArithmetic> runner(iterations, "Factorial BigInt");
            runner.run_all_operations(scale_name, num1, num2, divisor);

        }
        catch (const std::exception& e) {
            std::cerr << "  Error setting up Factorial BigInt benchmark for " << scale_name << ": " << e.what() << std::endl;
        }
    }
}


int main() {
    std::cout << "=========== Generic Arithmetic Benchmark ===========" << std::endl;

    // --- Data Scales Definition ---
    // Small: Fits easily in int
    std::string small_num1 = "98765";
    std::string small_num2 = "12345";
    std::string small_divisor = "7";
    int small_iterations = 1000000;

    // Medium: Fits in long long, but not int
    std::string medium_num1 = "987654321098765"; // ~10^15
    std::string medium_num2 = "123456789012345"; // ~10^14
    std::string medium_divisor = "999";
    int medium_iterations = 100000;

    // Large: Exceeds long long, fits in BigInt
    std::string large_num1 = benchmark::make_big_string('9', 50);           // 50 nines
    std::string large_num2 = benchmark::make_big_string('1', 50);           // 50 ones
    // Use a smaller number for division/modulo if 50-digit division is too slow
    std::string large_div_base = benchmark::make_big_string('7', 30);       // 30 sevens
    std::string large_divisor = "4567";
    int large_iterations = 1000; // Fewer iterations for large numbers

    // Very Large: Requires significant BigInt capacity
    std::string very_large_num1 = benchmark::make_big_string('5', 200);     // 200 fives
    std::string very_large_num2 = benchmark::make_big_string('3', 200);     // 200 threes
    std::string very_large_div_base = benchmark::make_big_string('8', 100); // 100 eights
    std::string very_large_divisor = "87654";
    int very_large_iterations = 50; // Very few iterations for very large numbers

    // --- Run Benchmarks for Each Type and Scale ---

    // Benchmarking BinaryArithmetic::BigInt
    benchmark::run_benchmarks_binary_bigint("Small", small_num1, small_num2, small_divisor, small_iterations);
    benchmark::run_benchmarks_binary_bigint("Medium", medium_num1, medium_num2, medium_divisor, medium_iterations);
    benchmark::run_benchmarks_binary_bigint("Large", large_num1, large_num2, large_divisor, large_iterations);
    benchmark::run_benchmarks_binary_bigint("Very Large", very_large_num1, very_large_num2, very_large_divisor, very_large_iterations);
    // Optional: Test division/modulo on slightly smaller large numbers if the main large ones are too slow
    benchmark::run_benchmarks_binary_bigint("Large (Div/Mod Base)", large_div_base, large_num2.substr(0, large_div_base.length()), large_divisor, large_iterations * 2);
    benchmark::run_benchmarks_binary_bigint("Very Large (Div/Mod Base)", very_large_div_base, very_large_num2.substr(0, very_large_div_base.length()), very_large_divisor, very_large_iterations * 2);


    // Benchmarking FactorialArithmetic::FactorialBigInt
    benchmark::run_benchmarks_factorial_bigint("Small", small_num1, small_num2, small_divisor, small_iterations);
    benchmark::run_benchmarks_factorial_bigint("Medium", medium_num1, medium_num2, medium_divisor, medium_iterations);
    benchmark::run_benchmarks_factorial_bigint("Large", large_num1, large_num2, large_divisor, large_iterations);
    benchmark::run_benchmarks_factorial_bigint("Very Large", very_large_num1, very_large_num2, very_large_divisor, very_large_iterations);
    // Optional: Test division/modulo on slightly smaller large numbers
    benchmark::run_benchmarks_factorial_bigint("Large (Div/Mod Base)", large_div_base, large_num2.substr(0, large_div_base.length()), large_divisor, large_iterations * 2);
    benchmark::run_benchmarks_factorial_bigint("Very Large (Div/Mod Base)", very_large_div_base, very_large_num2.substr(0, very_large_div_base.length()), very_large_divisor, very_large_iterations * 2);

    std::cout << "\n=========== Benchmark Complete ===========" << std::endl;

    return 0;
}