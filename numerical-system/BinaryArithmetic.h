#pragma once
#include "Internal.h"


#include <string>
#include <vector>
#include <stdexcept>


namespace numeric {
	
	class BinaryArithmetic : 
		public Comparable<BinaryArithmetic>,
		public Arithmetic<BinaryArithmetic>,
		public BitwiseArithmetic<BinaryArithmetic>
	{
	private:
		using value_type = std::make_unsigned_t<uint16_t>;
		static constexpr int VALUE_SIZE = sizeof(value_type);
		static constexpr int MAX_VALUE = std::numeric_limits<value_type>::max();
		static constexpr int MIN_VALUE = std::numeric_limits<value_type>::min();
		static constexpr int VALUE_COUNT_BIT = std::numeric_limits<value_type>::digits;

		std::vector<value_type> _storage;
		bool is_negative = false;

		BinaryArithmetic SumAbs(const BinaryArithmetic& lhs, const BinaryArithmetic& rhs) const noexcept;
		BinaryArithmetic SubtractAbs(const BinaryArithmetic& lhs, const BinaryArithmetic& rhs) const noexcept;
	public:
		BinaryArithmetic();
		BinaryArithmetic(std::string_view value);
		BinaryArithmetic(const char* value);

		template<typename _Ty> requires std::is_integral_v<_Ty>
		BinaryArithmetic(_Ty value) {
			is_negative = (value < 0);

			using UnsignedTy = std::make_unsigned_t<_Ty>;
			UnsignedTy abs_value = static_cast<UnsignedTy>(is_negative ? -value : value);

			_storage.clear();

			while (abs_value != 0) {
				value_type raw_word = static_cast<value_type>(abs_value & MAX_VALUE); // берем младшие 16 бит
				_storage.push_back(raw_word);                                             // сохраняем
				abs_value >>= VALUE_COUNT_BIT;                                        // сдвигаемся дальше
			}

			if (_storage.empty()) {
				_storage.push_back(0);
				is_negative = false;
			}
		}

		template<typename _Ty> requires std::is_integral_v<_Ty>
		operator _Ty() const {
			auto is_zero = [&]() -> bool {
				for (auto word : _storage) {
					if (word != 0) return false;
				}
				return true;
				};

			// Случай 1: Число равно нулю
			if (is_zero()) { return _Ty(0); }
				
			// Случай 2: Приведение к bool
			if constexpr (std::is_same_v<_Ty, bool>) {
				// Если число не ноль, оно приводится к true
				return true;
			}
			

			// Общий случай: 
			// Приведение к другому интегральному типу (_Ty не bool и число не 0)
			// Определяем количество бит в целевом типе _Ty
			const size_t bits_in_ty = sizeof(_Ty) * 8;

			// Будем собирать младшие биты в беззнаковый тип, достаточно широкий для _Ty.
			// uint64_t достаточно для всех стандартных интегральных типов (<= 64 бит).
			uint64_t unsigned_magnitude = 0;
			size_t bits_copied = 0;
		
			// Итерируемся по словам _storage от младшего к старшему
			for (size_t i = 0; i < _storage.size(); ++i) {
				// Если мы уже собрали достаточно бит для _Ty, останавливаемся
				if (bits_copied >= bits_in_ty) { break; }		
				uint64_t current_word = _storage[i];

				// Сколько бит из текущего слова нам нужно скопировать
				// Это либо все биты в слове, либо только оставшиеся до bits_in_ty
				size_t bits_from_word = std::min((size_t)VALUE_COUNT_BIT, bits_in_ty - bits_copied);

				// Маска для выделения нужных младших бит из текущего слова
				// Если нужно 64 бита, маска ~0ULL (все единицы)
				// Иначе (1ULL << bits_from_word) - 1
				uint64_t mask = (bits_from_word == VALUE_COUNT_BIT) ? ~0ULL : (1ULL << bits_from_word) - 1;

				// Выделяем нужную часть слова
				uint64_t word_part = current_word & mask;

				// Сдвигаем эту часть на нужную позицию в итоговом беззнаковом значении
				// и добавляем (через побитовое ИЛИ) к накопителю.
				// Гарантируется, что (word_part << bits_copied) не переполнит unsigned_magnitude,
				// т.к. мы останавливаемся, когда bits_copied достигнет bits_in_ty (<= 64).
				unsigned_magnitude |= (word_part << bits_copied);

				// Обновляем счетчик скопированных бит
				bits_copied += bits_from_word;
			}

			if constexpr (std::is_unsigned_v<_Ty>) {
				// Беззнаковое преобразование — просто возвращаем
				return static_cast<_Ty>(unsigned_magnitude);
			}
			else {
				// Знаковое преобразование — учитываем знак
				using Signed64 = std::make_signed_t<decltype(unsigned_magnitude)>;
				Signed64 signed_val = static_cast<Signed64>(unsigned_magnitude);
				signed_val *= is_negative ? -1 : 1;
				return static_cast<_Ty>(signed_val);
			}
		}

		template<typename _Ty> requires std::is_integral_v<_Ty>
		BinaryArithmetic operator=(_Ty value) {
			*this = BinaryArithmetic(value);
			return *this;
		}

		std::string to_string() const noexcept;
		BinaryArithmetic abs() const noexcept;

		friend bool operator<(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;

		friend BinaryArithmetic operator/(const BinaryArithmetic& lhs, const BinaryArithmetic rhs);
		friend BinaryArithmetic operator%(const BinaryArithmetic& lhs, const BinaryArithmetic rhs);
		friend BinaryArithmetic operator+(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;
		friend BinaryArithmetic operator-(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;
		friend BinaryArithmetic operator*(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;
		
		friend BinaryArithmetic operator&(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;
		friend BinaryArithmetic operator|(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;
		friend BinaryArithmetic operator^(const BinaryArithmetic& lhs, const BinaryArithmetic rhs) noexcept;
		
		friend BinaryArithmetic operator<<(const BinaryArithmetic lhs, const BinaryArithmetic rhs) noexcept;
		friend BinaryArithmetic operator>>(const BinaryArithmetic lhs, const BinaryArithmetic rhs) noexcept;
		
		friend BinaryArithmetic operator+(const BinaryArithmetic rhs) noexcept;
		friend BinaryArithmetic operator-(const BinaryArithmetic rhs) noexcept;
	};



}