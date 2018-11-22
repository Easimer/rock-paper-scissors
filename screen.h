#include "common.h"

API bool init_screen();
API bool fini_screen();

API void show_rps(rps_t, bool animate, bool player);
API void show_status(const char*);
API void show_status2(const char*);
API rps_t input_choice();
API void clear_display();

API void ssleep(int seconds);

