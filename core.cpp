#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "common.h"
#include "screen.h"
#include "sound.h"

internal rps_t cpu_choice(int* a) {
	int total = a[0] + a[1] + a[2];
	float rand_choice = (float)rand() / (float)RAND_MAX;
	int max = 0;

	if(a[1] > a[max]) {
		max = 1;
	}

	if(a[2] > a[max]) {
		max = 2;
	}

	if(a[max] / (float)total > rand_choice) {
		return counter((rps_t)max);
	} 

	return (rps_t)(rand() % rps_max);
}

internal const char* print_rps_t(rps_t v) {
	switch(v) {
		case rock:
			return "rock";
		case paper:
			return "paper";
		case scissors:
			return "scissors";
	}
	return "(ERROR)";
}

internal int points(rps_t pc, rps_t cpuc) {
	if(counter(cpuc) == pc) {
		return 1;
	}
	if(counter(pc) == cpuc) {
		return -1;
	}
	return 0;
}

internal void update_status(const char* pszFormat, ...) {
	char buf[256];
	va_list args;
	va_start(args, pszFormat);
	vsnprintf(buf, 256, pszFormat, args);
	va_end(args);
	buf[255] = 0;
	show_status(buf);
}

internal void animate_hand(rps_t p, rps_t cpu) {
	show_rps(rock, false, true);
	show_rps(rock, false, false);
	sfx_play(sfx_shake);
	ssleep(1);
	show_rps(rock, true, true);
	show_rps(rock, true, false);
	sfx_play(sfx_shake);
	ssleep(1);
	show_rps(p, false, true);
	show_rps(cpu, false, false);
	sfx_play(sfx_shake);
	ssleep(1);
}

internal void show_instructions(bool clear = false) {
	if(clear) {
		show_status2("                             ");
	} else {
		show_status2("(r)ock - (p)aper - (s)cissors - (q)uit");
	}
}

internal void show_score(int n) {
	update_status("Score: %d", n);
}

internal void show_choice(rps_t c) {
	update_status("You chose %s!", print_rps_t(c));
}

int main(int argc, char** argv) {
	srand(time(NULL));
	if(!init_screen()) {
		fini_screen();
		fprintf(stderr, "Failed to initialize display!\n");
		return -1;
	}
	if(!sfx_init()) {
		fprintf(stderr, "Failed to initialize SFX!\n");
		return -2;
	}

	int freq[rps_max] = {0, 0, 0};
	rps_t pc = rock;
	rps_t cpuc = rock;
	int score = 0;
	do {
		clear_display();
		show_score(score);
		show_instructions();
		pc = input_choice();
		if(pc == rps_max) {
			break;
		}
		show_choice(pc);
		show_instructions(true);

		clear_display();
		cpuc = cpu_choice(freq);
		animate_hand(pc, cpuc); 

		if(pc < rps_max) {
			freq[pc]++;
			int p = points(pc, cpuc);
			score += p;
			if(p > 0) {
				sfx_play(sfx_victory);
				update_status("You've won!");
			} else if(p == 0) {
				sfx_play(sfx_tie);
				update_status("TIE!");
			} else {
				sfx_play(sfx_failure);
				update_status("You've lost!");
			}
		}
		ssleep(2);
	} while(1);
	sfx_shutdown();
	fini_screen();
	return 0;
}
