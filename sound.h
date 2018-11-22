#pragma once

#include "common.h"

enum sfx_t {
	sfx_invalid = 0,
	sfx_shake,
	sfx_victory,
	sfx_failure,
	sfx_tie,
	sfx_max
};

API bool sfx_init();
API bool sfx_shutdown();
API bool sfx_play(sfx_t);
