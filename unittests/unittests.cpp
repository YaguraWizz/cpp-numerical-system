#include "gtest/gtest.h"	// Подключаем Google Test
#include <BinaryArithmetic.h>
#include <FactorialArithmetic.h>



// Google Test
namespace numeric {

    template<typename _Ty> requires std::is_integral_v<_Ty>
    struct type_info {
        static constexpr _Ty max = std::numeric_limits<_Ty>::max();
        static constexpr _Ty min = std::numeric_limits<_Ty>::min();
    };


    template<typename T>
    class INumericTest : public ::testing::Test {};

    // Добавьте FactorialArithmetic в список, если хотите его тестировать
    using AllTypes = ::testing::Types<BinaryArithmetic/*, FactorialArithmetic*/>;

    TYPED_TEST_SUITE(INumericTest, AllTypes);

    TYPED_TEST(INumericTest, ConstructionAndConversion) {
        const auto test_case = [](auto value) {
            using ValueType = decltype(value);
            TypeParam from_value(value);
            TypeParam from_string(std::to_string(value));

            EXPECT_EQ(from_value.to_string(), std::to_string(value));
            EXPECT_EQ(from_string.to_string(), std::to_string(value));

            if constexpr (std::is_same_v<ValueType, int> || std::is_same_v<ValueType, int64_t>)
            {
                EXPECT_EQ(static_cast<int64_t>(from_value), static_cast<int64_t>(value));
                EXPECT_EQ(static_cast<int64_t>(from_string), static_cast<int64_t>(value));
            }
            else if constexpr (std::is_same_v<ValueType, uint64_t>)
            {
                EXPECT_EQ(static_cast<uint64_t>(from_value), value);
                EXPECT_EQ(static_cast<uint64_t>(from_string), value);
            }
            };

        test_case(0);
        test_case(type_info<uint64_t>::max);
        test_case(type_info<int64_t>::min);
    }
    TYPED_TEST(INumericTest, ComparisonOperators) {
        const auto test_comparison_operators = [](auto v1, auto v2, auto v3) {
            TypeParam a(v1), b(v2), c(v3);
            TypeParam a_str(std::to_string(v1));
            TypeParam b_str(std::to_string(v2));
            TypeParam c_str(std::to_string(v3));

            // Проверка на < и >
            EXPECT_TRUE(a < b);
            EXPECT_TRUE(b > a);
            EXPECT_TRUE(b < c);
            EXPECT_TRUE(c > b);
            EXPECT_TRUE(a < c);
            EXPECT_TRUE(c > a);

            // Проверка <= и >=
            EXPECT_TRUE(a <= b);
            EXPECT_TRUE(b >= a);
            EXPECT_TRUE(b <= c);
            EXPECT_TRUE(c >= b);
            EXPECT_TRUE(a <= c);
            EXPECT_TRUE(c >= a);

            // Проверка ==
            EXPECT_TRUE(a == a);
            EXPECT_TRUE(b == b);
            EXPECT_TRUE(c == c);

            // Проверка !=
            EXPECT_TRUE(a != b);
            EXPECT_TRUE(b != c);
            EXPECT_TRUE(a != c);

            // Сравнение между значениями, созданными по-разному (из строки и числа)
            EXPECT_TRUE(a == a_str);
            EXPECT_TRUE(b == b_str);
            EXPECT_TRUE(c == c_str);

            EXPECT_FALSE(a != a_str);
            EXPECT_FALSE(b != b_str);
            EXPECT_FALSE(c != c_str);

            EXPECT_TRUE(a_str < b_str);
            EXPECT_TRUE(b_str > a_str);
            EXPECT_TRUE(b_str < c_str);
            EXPECT_TRUE(c_str > b_str);
            EXPECT_TRUE(a_str < c_str);
            EXPECT_TRUE(c_str > a_str);

            EXPECT_TRUE(a <= a_str);
            EXPECT_TRUE(b >= b_str);
            EXPECT_TRUE(c == c_str);
        };

        test_comparison_operators(
            type_info<uint64_t>::max - 2,
            type_info<uint64_t>::max - 1,
            type_info<uint64_t>::max
        );

        test_comparison_operators(
            type_info<int64_t>::min,
            type_info<int64_t>::min + 1,
            type_info<int64_t>::min + 2
        );
    }
    TYPED_TEST(INumericTest, ToStringAbs) {
        //GTEST_SKIP();
        const auto test_methods = [](auto value) {
            using ValueType = decltype(value);
            const std::string expected_str = std::to_string(value);

            TypeParam from_val(value);
            TypeParam from_str(expected_str);

            // Проверка to_string
            EXPECT_EQ(from_val.to_string(), expected_str);
            EXPECT_EQ(from_str.to_string(), expected_str);

            // Проверка abs (если value < 0)
            if constexpr (std::is_signed_v<ValueType>) {
                TypeParam abs_val = from_val.abs();
                TypeParam abs_str = from_str.abs();

                EXPECT_EQ(abs_val.to_string(), std::to_string(std::abs(value)));
                EXPECT_EQ(abs_str.to_string(), std::to_string(std::abs(value)));
            }
        };

        // Примеры тестов
        test_methods(0);
        test_methods(1);
        test_methods(4);
        test_methods(10);
        test_methods(123456789ULL);
        test_methods(type_info<uint64_t>::max >> 2); 

        // Отрицательные значения только для signed
        test_methods(-1);
        test_methods(-123456);
    }
    TYPED_TEST(INumericTest, UnaryOperators) {
        const auto test_unary_ops = [](auto raw) {
            using IntT = decltype(raw);
            TypeParam from_val(raw);
            TypeParam from_str(std::to_string(raw));

            // Проверка +a
            EXPECT_EQ(static_cast<IntT>(+from_val), static_cast<IntT>(+raw));
            EXPECT_EQ(static_cast<IntT>(+from_str), static_cast<IntT>(+raw));

            // Проверка -a
            EXPECT_EQ(static_cast<IntT>(-from_val), static_cast<IntT>(-raw));
            EXPECT_EQ(static_cast<IntT>(-from_str), static_cast<IntT>(-raw));

            // Проверка !a
            EXPECT_EQ(static_cast<IntT>(!from_val), static_cast<IntT>(!raw));
            EXPECT_EQ(static_cast<IntT>(!from_str), static_cast<IntT>(!raw));
            };

        test_unary_ops(5);
        test_unary_ops(0);
        test_unary_ops(-8);
    }
    TYPED_TEST(INumericTest, IncrementDecrement) {
        const auto test_inc_dec = [](auto raw) {
            using IntT = decltype(raw);

            // Тест для инициализации от числа
            {
                TypeParam value(raw);

                ++value;
                EXPECT_EQ(static_cast<IntT>(value), raw + 1);

                value++;
                EXPECT_EQ(static_cast<IntT>(value), raw + 2);

                --value;
                EXPECT_EQ(static_cast<IntT>(value), raw + 1);

                value--;
                EXPECT_EQ(static_cast<IntT>(value), raw);
            }

            // Тест для инициализации от строки
            {
                TypeParam value(std::to_string(raw));

                ++value;
                EXPECT_EQ(static_cast<IntT>(value), raw + 1);

                value++;
                EXPECT_EQ(static_cast<IntT>(value), raw + 2);

                --value;
                EXPECT_EQ(static_cast<IntT>(value), raw + 1);

                value--;
                EXPECT_EQ(static_cast<IntT>(value), raw);
            }
            };

        test_inc_dec(1);     // положительное значение
        test_inc_dec(0);     // ноль
        test_inc_dec(-5);    // отрицательное значение
    }
    TYPED_TEST(INumericTest, BitwiseOperators) {
        const auto test_bitwise_ops = [](auto raw_a, auto raw_b) {
            using IntT = decltype(raw_a);

            auto check_all = [&](TypeParam a, TypeParam b) {
                EXPECT_EQ(static_cast<IntT>(a & b), raw_a & raw_b);
                EXPECT_EQ(static_cast<IntT>(a | b), raw_a | raw_b);
                EXPECT_EQ(static_cast<IntT>(a ^ b), raw_a ^ raw_b);
                EXPECT_EQ(static_cast<IntT>(a << b), raw_a << raw_b);
                EXPECT_EQ(static_cast<IntT>(a >> b), raw_a >> raw_b);
                };

            // Конструктор от чисел
            check_all(TypeParam(raw_a), TypeParam(raw_b));

            // Конструктор от строк
            check_all(TypeParam(std::to_string(raw_a)), TypeParam(std::to_string(raw_b)));
            };

        test_bitwise_ops(6, 3);  // 6 = 110, 3 = 011
        test_bitwise_ops(6, 0);  // zero on right
        test_bitwise_ops(0, 3);  // zero on left
    }
    TYPED_TEST(INumericTest, ArithmeticOperators) {
        const auto test_arithmetic = [](auto raw_a, auto raw_b) {
            using IntT = decltype(raw_a);

            auto check_all = [](TypeParam a, TypeParam b, IntT raw_a, IntT raw_b) {
                {
                    TypeParam tmp = a;
                    EXPECT_EQ(static_cast<IntT>(tmp + b), raw_a + raw_b);
                    tmp += b;
                    EXPECT_EQ(static_cast<IntT>(tmp), raw_a + raw_b);
                }
                {
                    TypeParam tmp = a;
                    EXPECT_EQ(static_cast<IntT>(tmp - b), raw_a - raw_b);
                    tmp -= b;
                    EXPECT_EQ(static_cast<IntT>(tmp), raw_a - raw_b);
                }
                {
                    TypeParam tmp = a;
                    EXPECT_EQ(static_cast<IntT>(tmp * b), raw_a * raw_b);
                    tmp *= b;
                    EXPECT_EQ(static_cast<IntT>(tmp), raw_a * raw_b);
                }
                if (raw_b != 0) {
                    TypeParam tmp = a;
                    EXPECT_EQ(static_cast<IntT>(tmp / b), raw_a / raw_b);
                    tmp /= b;
                    EXPECT_EQ(static_cast<IntT>(tmp), raw_a / raw_b);

                    tmp = a;
                    EXPECT_EQ(static_cast<IntT>(tmp % b), raw_a % raw_b);
                    tmp %= b;
                    EXPECT_EQ(static_cast<IntT>(tmp), raw_a % raw_b);
                }
                };

            // Числовая инициализация
            check_all(TypeParam(raw_a), TypeParam(raw_b), raw_a, raw_b);

            // Строковая инициализация
            check_all(TypeParam(std::to_string(raw_a)), TypeParam(std::to_string(raw_b)), raw_a, raw_b);
            };

        // Простые кейсы
        test_arithmetic(6, 3);
        test_arithmetic(10, -5);
        test_arithmetic(-10, -3);
        test_arithmetic(0, 5);
        test_arithmetic(5, 0);  // здесь проверка деления будет пропущена

        // Большие числа: string("111...1") и string("999...9")
        const auto make_big_string = [](char ch, size_t len) {
            return std::string(len, ch);
            };

        const std::vector<size_t> sizes = { 20, 50, 100 }; // разные длины
        for (size_t len : sizes) {
            std::string big_ones = make_big_string('1', len);
            std::string big_nines = make_big_string('9', len);

            TypeParam a(big_ones), b(big_nines);

            // Проверим, что операции работают корректно: (a + b) == (a += b), и т.д.
            {
                TypeParam tmp1 = a; // Копия для оператора +
                TypeParam tmp2 = a; // Копия для оператора +=

                // Сравниваем результат tmp1 + b с результатом tmp2 += b
                EXPECT_EQ(((tmp1 + b).to_string()), ((tmp2 += b).to_string()));
            }
            {
                TypeParam tmp1 = a; // Копия для оператора -
                TypeParam tmp2 = a; // Копия для оператора -=

                EXPECT_EQ((tmp1 - a).to_string(), (tmp2 -= a).to_string());
            }
            {
                TypeParam tmp1 = a; // Копия для оператора *
                TypeParam tmp2 = a; // Копия для оператора *=

                EXPECT_EQ((tmp1 * b).to_string(), (tmp2 *= b).to_string());
            }

            // Для деления — проверка только если b ≠ 0
            if (a.to_string().find_first_not_of('0') != std::string::npos) {
                {
                    TypeParam tmp1 = b; // Копия для оператора /
                    TypeParam tmp2 = b; // Копия для оператора /=

                    // Сравниваем результат tmp1 / a с результатом tmp2 /= a
                    EXPECT_EQ((tmp1 / a).to_string(), (tmp2 /= a).to_string());
                } // tmp1 и tmp2 уничтожаются здесь

                {
                    TypeParam tmp3 = b; // Копия для оператора %
                    TypeParam tmp4 = b; // Копия для оператора %=

                    // Сравниваем результат tmp3 % a с результатом tmp4 %= a
                    EXPECT_EQ((tmp3 % a).to_string(), (tmp4 %= a).to_string());
                } // tmp3 и tmp4 уничтожаются здесь
            }
        }
    }
    TYPED_TEST(INumericTest, DivisionByZero) {
        TypeParam a(42);
        TypeParam zero(0);
        TypeParam a_str(std::to_string(42));
        TypeParam zero_str("0");

        // Числовая и строковая инициализация должны вести себя одинаково
        EXPECT_THROW((a / zero), std::overflow_error);
        EXPECT_THROW((a % zero), std::overflow_error);   
        EXPECT_THROW((a_str / zero_str), std::overflow_error);
        EXPECT_THROW((a_str % zero_str), std::overflow_error);
    }

} // namespace numeric


