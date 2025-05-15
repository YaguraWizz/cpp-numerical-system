#pragma once

#include <stdint.h>
#include <limits>
#include <string_view>
#include <type_traits>


namespace numeric {
    template <typename T>
    struct Comparable {
        friend bool operator==(const T& lhs, const T& rhs) {
            return !(lhs < rhs) && !(rhs < lhs);
        }

        friend bool operator!=(const T& lhs, const T& rhs) {
            return !(lhs == rhs);
        }

        friend bool operator>(const T& lhs, const T& rhs) {
            return rhs < lhs;
        }

        friend bool operator<=(const T& lhs, const T& rhs) {
            return !(rhs < lhs);
        }

        friend bool operator>=(const T& lhs, const T& rhs) {
            return !(lhs < rhs);
        }
    };

    template <typename T>
    struct Arithmetic {
        T& operator+=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self + rhs;
            return self;
        }

        T& operator-=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self - rhs;
            return self;
        }

        T& operator*=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self * rhs;
            return self;
        }

        T& operator/=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self / rhs;
            return self;
        }

        T& operator%=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self % rhs;
            return self;
        }

        // Префиксный инкремент (++x)
        T operator++() {
            T& self = static_cast<T&>(*this);
            self = self + T{ 1 };
            return self;
        }

        // Постфиксный инкремент (x++)
        T operator++(int) {
            T& self = static_cast<T&>(*this);
            T old = self;
            self = self + T{ 1 };
            return old;
        }

        // Префиксный декремент (--x)
        T operator--() {
            T& self = static_cast<T&>(*this);
            self = self - T{ 1 };
            return self;
        }

        // Постфиксный декремент (x--)
        T operator--(int) {
            T& self = static_cast<T&>(*this);
            T old = self;
            self = self - T{ 1 };
            return old;
        }
    };

    template <typename T>
    struct BitwiseArithmetic {
        T& operator&=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self & rhs;
            return self;
        }

        T& operator|=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self | rhs;
            return self;
        }

        T& operator^=(const T& rhs) {
            T& self = static_cast<T&>(*this);
            self = self ^ rhs;
            return self;
        }

        T& operator<<=(int shift) {
            T& self = static_cast<T&>(*this);
            self = self << shift;
            return self;
        }

        T& operator>>=(int shift) {
            T& self = static_cast<T&>(*this);
            self = self >> shift;
            return self;
        }
    };

}