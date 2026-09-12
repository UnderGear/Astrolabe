#pragma once

#include <numbers>

#include "Fixed.hpp"

namespace Math
{
	constexpr inline i24f8_t E{ std::numbers::e_v<float> };
	constexpr inline i24f8_t Pi{ std::numbers::pi_v<float> };
	constexpr inline i24f8_t INV_PI{ std::numbers::inv_pi_v<float> };
	constexpr inline i24f8_t SQRT2{ std::numbers::sqrt2_v<float> };
	//TODO: add more as necessary
}
