#include "FactorialArithmetic.h"

namespace numeric {
	FactorialArithmetic::FactorialArithmetic(const std::string& value)
	{

	}
	FactorialArithmetic::FactorialArithmetic(const char* value) : FactorialArithmetic(std::string(value)) {}
	
	std::string FactorialArithmetic::to_string() const noexcept
	{
		return std::string();
	}
	FactorialArithmetic FactorialArithmetic::abs() const noexcept
	{
		FactorialArithmetic copu(*this);
		copu.is_negative = false;
		return copu;
	}


	bool operator<(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return false;
	}
	FactorialArithmetic operator+(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator-(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator*(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator/(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator%(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator&(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator|(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator^(const FactorialArithmetic& lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator<<(const FactorialArithmetic lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator>>(const FactorialArithmetic lhs, const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}

	FactorialArithmetic operator+(const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
	FactorialArithmetic operator-(const FactorialArithmetic rhs) noexcept
	{
		return FactorialArithmetic();
	}
}