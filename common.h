#pragma once
#define internal static
#define API extern "C" __attribute__ ((visibility ("default")))

using u8 = unsigned char;
using s16 = signed short;

enum rps_t {
	rock = 0,
	paper,
	scissors,
	rps_max,
};

constexpr rps_t counter(rps_t c) {
	return (rps_t)(((int)c + 1) % rps_max);
}

static_assert(counter(rock) == paper);
static_assert(counter(paper) == scissors);
static_assert(counter(scissors) == rock);
