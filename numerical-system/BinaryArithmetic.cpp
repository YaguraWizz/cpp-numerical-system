#include "BinaryArithmetic.h"


#include <bitset>
#include <iostream>
#define PRINT_BINARY_VECTOR(vec) print_binary_vector(vec, #vec)
#define _PRINT_BINARY_VECTOR(vec, str) print_binary_vector(vec, str)

template<typename T>
void print_binary_vector(const std::vector<T>& vec, std::string_view msg) {
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    std::cout << msg << ":\n";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::bitset<sizeof(T) * 8> bits(vec[i]);
        std::cout << "[" << i << "]: " << bits << " (" << +vec[i] << ")\n";
    }
}


namespace numeric {
    static constexpr uint64_t SIZE_BYTE = std::numeric_limits<uint8_t>::digits;
    namespace {

        template<typename value_type>
        constexpr size_t bits_per_element = std::numeric_limits<value_type>::digits;

        template<typename value_type>
        static bool get_bit(const std::vector<value_type>& bits, size_t pos) {
            size_t elem_idx = pos / bits_per_element<value_type>;
            size_t bit_idx = pos % bits_per_element<value_type>;
            if (elem_idx >= bits.size()) return false;
            return (bits[elem_idx] >> bit_idx) & 1;
        }

        template<typename value_type>
        static void set_bit(std::vector<value_type>& bits, size_t pos, bool value) {
            size_t elem_idx = pos / bits_per_element<value_type>;
            size_t bit_idx = pos % bits_per_element<value_type>;
            if (elem_idx >= bits.size()) bits.resize(elem_idx + 1, 0);
            value_type mask = static_cast<value_type>(1) << bit_idx;
            if (value)
                bits[elem_idx] |= mask;
            else
                bits[elem_idx] &= ~mask;
        }

        template<typename value_type>
        static size_t bit_count(const std::vector<value_type>& bits) {
            for (size_t i = bits.size() * bits_per_element<value_type>; i > 0; --i) {
                if (get_bit(bits, i - 1)) return i;
            }
            return 0;
        }

        static constexpr bool is_valid_string(std::string_view str) {
            if (str.empty()) return false;

            // Если строка состоит только из одного символа '0', то это допустимо
            if (str.size() > 1 && str.find_first_not_of('0') == std::string_view::npos) {
                return false;  // строка состоит только из нулей
            }

            size_t start = 0;
            char first = str[0];

            if (first == '-') {
                if (str.size() == 1) return false;  // Тире без числа - ошибка
                start = 1;
            }

            // Проверка, что все символы после первого являются цифрами
            for (size_t i = start; i < str.size(); ++i) {
                char ch = str[i];
                if (ch < '0' || ch > '9') {
                    return false;
                }
            }

            return true;
        }
    }


    BinaryArithmetic::BinaryArithmetic(): _storage(0), is_negative(false) {}
    BinaryArithmetic::BinaryArithmetic(const char* value) : BinaryArithmetic(std::string(value)) {}   
    BinaryArithmetic BinaryArithmetic::SumAbs(const BinaryArithmetic& lhs, const BinaryArithmetic& rhs) const noexcept {
        using value_type = BinaryArithmetic::value_type;

        // одинаковые знаки → просто сложить модули и сохранить знак
        BinaryArithmetic result;
        const auto& a = lhs._storage;
        const auto& b = rhs._storage;
        auto& r = result._storage;

        size_t max_size = std::max(a.size(), b.size());
        r.resize(max_size);

        value_type carry = 0;
        for (size_t i = 0; i < max_size; ++i) {
            value_type ai = i < a.size() ? a[i] : 0;
            value_type bi = i < b.size() ? b[i] : 0;

            value_type sum = ai + bi + carry;

            // Проверка на переполнение: если результат меньше одного из операндов, был перенос
            if (sum < ai || sum < bi || (carry && sum == ai)) {
                carry = 1; // Устанавливаем перенос в 1
            }
            else {
                carry = 0; // Переноса нет
            }

            r[i] = sum;
        }

        // Если был перенос, добавляем новый разряд
        if (carry) {
            r.push_back(1);
        }
        return result;
    }
    BinaryArithmetic BinaryArithmetic::SubtractAbs(const BinaryArithmetic& lhs, const BinaryArithmetic& rhs) const noexcept {
        using value_type = BinaryArithmetic::value_type;
        
        BinaryArithmetic result;
        const auto& lhs_d = lhs._storage;
        const auto& rhs_d = rhs._storage;
        auto& res_d = result._storage;

        size_t max_size = lhs_d.size();
        res_d.resize(max_size);

        value_type borrow = 0;

        for (size_t i = 0; i < max_size; ++i) {
            value_type val1 = (i < lhs_d.size()) ? lhs_d[i] : 0;
            value_type val2 = (i < rhs_d.size()) ? rhs_d[i] : 0;

            value_type diff = val1 - val2 - borrow;

            if (diff > val1) {
                diff = (BinaryArithmetic::MAX_VALUE - diff) + 1;
                borrow = 1;
            }
            else {
                borrow = 0;
            }

            res_d[i] = diff;
        }

        // Удаление ведущих нулей
        while (res_d.size() > 1 && res_d.back() == 0) {
            res_d.pop_back();
        }

        // После корректного вызова subtract, borrow не должен остаться
        // Если borrow > 0, это указывает на ошибку в логике вызова
        // или в самой функции subtract. В данном контексте, borrow
        // всегда должен быть 0 после завершения цикла.
        return result;
    }  
    BinaryArithmetic::BinaryArithmetic(std::string_view value) {
        if (!is_valid_string(value)) {
            throw std::invalid_argument("Invalid input string for integral string disability error.");
        }

        if (value.front() == '-') {
            is_negative = true;
            value.remove_prefix(1); // Удаляем знак минус
        }

        // Если строка состоит только из одного нуля, то результат = 0
        if (value.size() == 1 && value.front() == '0') {
            _storage.push_back(0);
            return;
        }

        // Копируем value в std::string для безопасной модификации
        std::string val_str(value);

        auto divideByTwo = [](std::string& str) -> int {
            int remainder = 0;
            std::string result;
            result.reserve(str.size());

            for (char ch : str) {
                int digit = ch - '0';
                int current = remainder * 10 + digit;
                result += static_cast<char>((current / 2) + '0');
                remainder = current % 2;
            }

            // Удаляем ведущие нули
            size_t first_non_zero = result.find_first_not_of('0');
            if (first_non_zero != std::string::npos) {
                str = result.substr(first_non_zero);
            }
            else {
                str = "0";
            }

            return remainder;
            };

        // Переход: сразу пишем в value_type, разворачивая биты
        std::vector<value_type> bitVector;
        value_type current_word = 0;
        int bit_index = 0;

        // Процесс преобразования числа в бинарную форму
        while (val_str != "0") {
            int remainder = divideByTwo(val_str); // Остаток от деления на 2 — это младший бит
            current_word |= static_cast<value_type>(remainder) << (bit_index); 
            bit_index++;

            // Как только слово заполнилось (битов >= VALUE_COUNT_BIT), добавляем его в вектор
            if (bit_index == VALUE_COUNT_BIT) {
                bitVector.push_back(current_word); // Добавляем слово в вектор
                current_word = 0;  // Очищаем для следующего слова
                bit_index = 0;  // Сбрасываем индекс
            }
        }

        // Если остались незаписанные биты, добавляем их в вектор
        if (bit_index != 0) {
            bitVector.push_back(current_word);
        }

        _storage = std::move(bitVector);
    }

    std::string BinaryArithmetic::to_string() const noexcept {
        if (_storage.empty() || (_storage.size() == 1 && _storage[0] == 0)) {
            // Обработка пустого вектора или вектора, представляющего ноль
            return "0";
        }

        // Копируем storage для возможной модификации (если требуется для определения точного размера)
        // В данном случае, для to_string const, мы не модифицируем _storage
        // Поэтому прямо используем _storage

        // Конвертация двоичного представления в десятичную строку
        // Используем алгоритм удвоения и прибавления бита

        std::string decimal_result = "0";

        // Итерируемся по словам от старшего к младшему
        for (auto it = _storage.rbegin(); it != _storage.rend(); ++it) {
            uint64_t current_word = *it;

            // Итерируемся по битам внутри слова от старшего к младшему
            for (int i = VALUE_COUNT_BIT - 1; i >= 0; --i) {
                int bit = (current_word >> i) & 1; // Получаем i-й бит

                // Выполняем decimal_result = decimal_result * 2 + bit
                std::string next_decimal_result;
                uint64_t carry = 0;

                // Умножение на 2
                for (size_t j = decimal_result.size(); j > 0; --j) {
                    uint64_t digit = (decimal_result[j - 1] - '0') * 2 + carry;
                    next_decimal_result += static_cast<char>((digit % 10) + '0');
                    carry = digit / 10;
                }
                if (carry) {
                    next_decimal_result += static_cast<char>(carry + '0');
                }
                std::reverse(next_decimal_result.begin(), next_decimal_result.end());

                // Прибавление текущего бита (0 или 1)
                if (bit == 1) {
                    carry = 1; // carry уже может содержать что-то от умножения, но прибавление 1 начинается с 1
                    for (size_t j = next_decimal_result.size(); j > 0; --j) {
                        uint64_t digit = (next_decimal_result[j - 1] - '0') + carry;
                        next_decimal_result[j - 1] = static_cast<char>((digit % 10) + '0');
                        carry = digit / 10; // Обновляем carry для сложения
                    }
                    if (carry) {
                        next_decimal_result.insert(next_decimal_result.begin(), static_cast<char>(carry + '0'));
                    }
                }

                // Удаляем ведущие нули из next_decimal_result, если результат не "0"
                size_t first_non_zero = next_decimal_result.find_first_not_of('0');
                if (first_non_zero == std::string::npos) {
                    decimal_result = "0"; // Результат после операций стал нулем
                }
                else {
                    decimal_result = next_decimal_result.substr(first_non_zero);
                }
            }
        }

        // Обработка знака
        if (is_negative && decimal_result != "0") {
            decimal_result.insert(decimal_result.begin(), '-');
        }

        return decimal_result;
    }


    BinaryArithmetic BinaryArithmetic::abs() const noexcept {
        BinaryArithmetic copu(*this);
        copu.is_negative = false;
        return copu;
    }

    bool operator<(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept {
        using value_type = BinaryArithmetic::value_type;

        // 1. Сравнение знаков
        if (lhs.is_negative != rhs.is_negative) {
            return lhs.is_negative; // Отрицательное < Положительного
        }

        // Лямбда для сравнения абсолютных значений по модулю.
        // Возвращает:
        // -1 если |lhs| < |rhs|
        //  1 если |lhs| > |rhs|
        //  0 если |lhs| == |rhs|
        auto compare_magnitudes_result = [](const auto& l_storage, const auto& r_storage) -> int {
            size_t max_size = std::max(l_storage.size(), r_storage.size());

            // Итерируемся от самой старшей возможной позиции слова вниз к 0
            for (size_t i = max_size; i-- > 0;) {
                value_type word1 = (i < l_storage.size()) ? l_storage[i] : 0;
                value_type word2 = (i < r_storage.size()) ? r_storage[i] : 0;

                if (word1 < word2) {
                    return -1; // Нашли первое отличие, |l| < |r|
                }
                if (word1 > word2) {
                    return 1; // Нашли первое отличие, |l| > |r|
                }
                // Если слова равны, продолжаем к следующей (более младшей) позиции
            }

            // Если цикл завершен без отличий, абсолютные значения равны
            return 0;
        };

        int magnitude_comparison = compare_magnitudes_result(lhs._storage, rhs._storage);

        // 2. Оба положительные (или оба равны нулю)
        if (!lhs.is_negative) {
            // Для положительных: lhs < rhs только если |lhs| < |rhs|
            return magnitude_comparison < 0; // true только если результат сравнения модулей < 0 (-1)
        }
        else { // 3. Оба отрицательные
            // Для отрицательных: lhs < rhs только если |lhs| > |rhs|
            return magnitude_comparison > 0; // true только если результат сравнения модулей > 0 (1)
        }
    }
   
    BinaryArithmetic operator+(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept {
        if (lhs.is_negative == rhs.is_negative) {
            // a + b | (-a) + (-b)
            BinaryArithmetic result = lhs.SumAbs(lhs, rhs);
            result.is_negative = lhs.is_negative;
            return result;
        }
        else if (lhs.abs() == rhs.abs()) {
            return 0;
        }
        else
        {
            const bool lhsAbsGe = lhs.abs() > rhs.abs();
            BinaryArithmetic larger = lhsAbsGe ? lhs : rhs;
            BinaryArithmetic smaller = lhsAbsGe ? rhs : lhs;

            BinaryArithmetic result = larger.SubtractAbs(larger, smaller);
            result.is_negative = larger.is_negative;
            return result;
        }
    }
    BinaryArithmetic operator-(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept { 
        BinaryArithmetic negatedRhs = rhs;
        negatedRhs.is_negative = !negatedRhs.is_negative;
        return lhs + negatedRhs;
    }
    BinaryArithmetic operator*(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept {
        if (lhs == BinaryArithmetic(0) || rhs == BinaryArithmetic(0)) return 0;

        BinaryArithmetic result(0);     
        BinaryArithmetic abs_lhs = lhs.abs();
        BinaryArithmetic abs_rhs = rhs.abs();

        size_t total_bits = bit_count(abs_rhs._storage);
        for (size_t i = 0; i < total_bits; ++i) {
            if (get_bit(abs_rhs._storage, i)) {
                result += (abs_lhs << BinaryArithmetic(i));      
            }
        }

        // Корректировка нулевого знака
        if (result == BinaryArithmetic(0)) result.is_negative = false;

        if (lhs.is_negative && rhs.is_negative) result.is_negative = false;
        else if (!lhs.is_negative && rhs.is_negative) result.is_negative = true;
        else if (lhs.is_negative && !rhs.is_negative) result.is_negative = true;
        return result;
    }  

    BinaryArithmetic operator/(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) {
        if (rhs == BinaryArithmetic(0)) throw std::overflow_error("Division by zero");
        if (lhs == BinaryArithmetic(0)) return 0;

        BinaryArithmetic abs_lhs = lhs.abs();
        BinaryArithmetic abs_rhs = rhs.abs();

        if (abs_lhs < abs_rhs) return 0;

        BinaryArithmetic quotient(0);
        quotient.is_negative = lhs.is_negative != rhs.is_negative;
        size_t bit_diff = bit_count(abs_lhs._storage) - bit_count(abs_rhs._storage);

        for (int i = static_cast<int>(bit_diff); i >= 0; --i) {
            BinaryArithmetic divisor = abs_rhs << BinaryArithmetic(i);
            if (abs_lhs >= divisor) {
                abs_lhs = abs_lhs - divisor;
                set_bit(quotient._storage, i, true);
            }
        }

        // Удаление ведущих нулей
        while (!quotient._storage.empty() && quotient._storage.back() == 0) {
            quotient._storage.pop_back();
        }
        if (quotient._storage.empty()) {
            quotient._storage.push_back(0);
        }
        return quotient;
    }
    BinaryArithmetic operator%(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) {
        if (rhs == BinaryArithmetic(0)) throw std::overflow_error("Division by zero");

        BinaryArithmetic quotient = lhs / rhs;
        BinaryArithmetic remainder = lhs - (quotient * rhs);
        remainder.is_negative = lhs.is_negative;
        return remainder;
    }


    BinaryArithmetic operator&(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept
    {
        using value_type = BinaryArithmetic::value_type;
        BinaryArithmetic result;

        const auto& a = lhs._storage;
        const auto& b = rhs._storage;
        auto& r = result._storage;

        size_t max_size = std::max(a.size(), b.size());
        r.resize(max_size);

        // Iterate over each bit of both operands and perform bitwise AND
        for (size_t i = 0; i < max_size; ++i) {
            value_type ai = i < a.size() ? a[i] : 0;
            value_type bi = i < b.size() ? b[i] : 0;

            // Perform bitwise AND and mirror the result
            value_type buf = ai & bi;
            r[i] = buf;
        }

        return result;
    }
    BinaryArithmetic operator|(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept
    {
        using value_type = BinaryArithmetic::value_type;
        BinaryArithmetic result;

        const auto& a = lhs._storage;
        const auto& b = rhs._storage;
        auto& r = result._storage;

        size_t max_size = std::max(a.size(), b.size());
        r.resize(max_size);

        // Iterate over each bit of both operands and perform bitwise OR
        for (size_t i = 0; i < max_size; ++i) {
            value_type ai = i < a.size() ? a[i] : 0;
            value_type bi = i < b.size() ? b[i] : 0;

            // Perform bitwise OR and mirror the result
            value_type buf = ai | bi;
            r[i] = buf;
        }

        return result;
    }
    BinaryArithmetic operator^(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept
    {
        using value_type = BinaryArithmetic::value_type;
        BinaryArithmetic result;

        const auto& a = lhs._storage;
        const auto& b = rhs._storage;
        auto& r = result._storage;

        size_t max_size = std::max(a.size(), b.size());
        r.resize(max_size);

        // Iterate over each bit of both operands and perform bitwise XOR
        for (size_t i = 0; i < max_size; ++i) {
            value_type ai = i < a.size() ? a[i] : 0;
            value_type bi = i < b.size() ? b[i] : 0;

            // Perform bitwise XOR and mirror the result
            value_type buf = ai ^ bi;
            r[i] = buf;
        }

        return result;
    }

    BinaryArithmetic operator<<(const BinaryArithmetic lhs, const BinaryArithmetic rhs) noexcept {
        using value_type = BinaryArithmetic::value_type;
        BinaryArithmetic result(lhs);  // Копируем lhs в результат
        auto& r = result._storage;     // Результат сдвига
        
        // Преобразуем каждый элемент rhs._storage в сдвиг и применяем его
        for (size_t j = 0; j < rhs._storage.size(); ++j) {
            // Разворачиваем текущий элемент rhs._storage[j] в тип value_type
            value_type shift = rhs._storage[j];

            // Проходим по всем элементам lhs._storage и сдвигаем их на полученный сдвиг
            for (size_t i = 0; i < lhs._storage.size(); ++i) {
                value_type buf = r[i];  // Сохраняем текущее значение

                // Сдвигаем биты в текущем элементе
                r[i] <<= shift;

                // Если старший бит был равен 1, переносим его в следующий элемент
                if (buf >> (BinaryArithmetic::VALUE_COUNT_BIT - 1) != 0) {
                    value_type bit = buf << (BinaryArithmetic::VALUE_COUNT_BIT - shift);

                    if (i + 1 < r.size()) {
                        r[i + 1] |= bit;  // Переносим бит в старший разряд следующего элемента
                    }
                    else {
                        r.push_back(bit);
                    }
                }
            }
        }
        return result;
    }
    BinaryArithmetic operator>>(const BinaryArithmetic lhs, const BinaryArithmetic rhs) noexcept {
        using value_type = BinaryArithmetic::value_type;
        BinaryArithmetic result(lhs);  // Копируем lhs в результат
        auto& r = result._storage;     // Результат сдвига

        // Преобразуем каждый элемент rhs._storage в сдвиг и применяем его
        for (size_t j = 0; j < rhs._storage.size(); ++j) {
            // Разворачиваем текущий элемент rhs._storage[j] в тип value_type
            value_type shift = rhs._storage[j];

            // Проходим по всем элементам lhs._storage и сдвигаем их на полученный сдвиг
            for (size_t i = 0; i < lhs._storage.size(); ++i) {
                value_type buf = r[i];  // Сохраняем текущее значение

                // Сдвигаем биты в текущем элементе
                r[i] >>= shift;

                // Если старший бит был равен 1, переносим его в следующий элемент
                if (buf >> (BinaryArithmetic::VALUE_COUNT_BIT - 1) != 0) {
                    value_type bit = buf << (BinaryArithmetic::VALUE_COUNT_BIT - shift);

                    if (i + 1 < r.size()) {
                        r[i + 1] |= bit;  // Переносим бит в старший разряд следующего элемента
                    }
                    else {
                        r.push_back(bit);
                    }
                }
            }
        }
        return result;
    }

    BinaryArithmetic operator+(const BinaryArithmetic rhs) noexcept {
        return BinaryArithmetic(rhs);
    }
    BinaryArithmetic operator-(const BinaryArithmetic rhs) noexcept {
        BinaryArithmetic copy(rhs);
        copy.is_negative = !rhs.is_negative;
        return copy;
    }
}