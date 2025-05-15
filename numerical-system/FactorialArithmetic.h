#pragma once
#include "Internal.h"
#include <vector>
#include <string>


namespace numeric {

	class FactorialArithmetic :
		public Comparable<FactorialArithmetic>,
		public Arithmetic<FactorialArithmetic>,
		public BitwiseArithmetic<FactorialArithmetic>
	{
	private:
		using value_type = uint16_t;
		static constexpr int VALUE_SIZE = sizeof(value_type);
		static constexpr int VALUE_COUNT_BIT = std::numeric_limits<value_type>::digits;

		std::vector<value_type> _storage;
		bool is_negative = false;
	public:
		FactorialArithmetic() = default;
		FactorialArithmetic(const std::string& value);
		FactorialArithmetic(const char* value);

		template<typename _Ty> requires std::is_integral_v<_Ty>
		FactorialArithmetic(_Ty value) {


		}

		template<typename _Ty> requires std::is_integral_v<_Ty>
		operator _Ty() {
			if (_storage.empty()) return 0;
			return static_cast<_Ty>(_storage[0]);
		}

		template<typename _Ty> requires std::is_integral_v<_Ty>
		FactorialArithmetic operator=(_Ty value) {
			*this = FactorialArithmetic(value);
			return *this;
		}

		std::string to_string() const noexcept;

		FactorialArithmetic abs() const noexcept;
		friend bool operator<(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept;

		friend FactorialArithmetic operator+(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept;
		friend FactorialArithmetic operator-(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept;
		friend FactorialArithmetic operator*(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept;
		friend FactorialArithmetic operator/(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept;
		friend FactorialArithmetic operator%(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept;

		friend FactorialArithmetic operator<<(const FactorialArithmetic lhs, const FactorialArithmetic rhs) noexcept;
		friend FactorialArithmetic operator>>(const FactorialArithmetic lhs, const FactorialArithmetic rhs) noexcept;

		friend FactorialArithmetic operator+(const FactorialArithmetic rhs) noexcept;
		friend FactorialArithmetic operator-(const FactorialArithmetic rhs) noexcept;
	};




}