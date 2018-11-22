#include "screen.h"
#include <ncurses.h>
#include <unistd.h>

internal WINDOW* wStatus = nullptr;
internal WINDOW* wInput = nullptr;
internal WINDOW* wDisplayL = nullptr;
internal WINDOW* wDisplayR = nullptr;

API bool init_screen() {
	initscr();
	noecho();
	curs_set(0);
	nodelay(stdscr, TRUE);

	int my, mx;
	getmaxyx(stdscr, my, mx);

	wStatus = newwin(1, mx / 2, 0, 0);
	wInput = newwin(1, mx / 2, 0, mx / 2);
	wDisplayL = newwin(my - 2, mx / 2, 2, 0);
	wDisplayR = newwin(my - 2, mx / 2, 2, mx / 2);

	if(!wStatus) {
		return false;
	}
	if(!wInput) {
		return false;
	}
	if(!wDisplayL) {
		return false;
	}
	if(!wDisplayR) {
		return false;
	}
	return true;
}

API bool fini_screen() {
	if(wStatus) {
		delwin(wStatus);
	}
	if(wInput) {
		delwin(wInput);
	}
	if(wDisplayL) {
		delwin(wDisplayL);
	}
	if(wDisplayR) {
		delwin(wDisplayR);
	}

	endwin();

	return true;
}

internal const char* gpszRock =
"\
    _______   \n\
---'   ____)  \n\
      (_____) \n\
      (_____) \n\
      (____)  \n\
---.__(___)   \n\
";

internal const char* gpszRockRight =
"\
   _______\n\
  (____   '---\n\
 (_____)\n\
 (_____)\n\
  (____)\n\
   (___)__.---\n\
";

internal const char* gpszPaper =
" \
     _______       \n\
---'    ____)____  \n\
           ______) \n\
          _______) \n\
         _______)  \n\
---.__________)    \n\
";

internal const char* gpszPaperRight =
" \
       _______\n\
  ____(____   '---\n\
 (______\n\
 (_______\n\
  (_______\n\
    (_________.---\n\
";

internal const char* gpszScissors =
" \
    _______ \n\
---'   ____)____\n\
          ______) \n\
       __________) \n\
      (____) \n\
---.__(___) \n\
";

internal const char* gpszScissorsRight =
" \
         _______ \n\
    ____(____   '---\n\
   (______\n\
  (__________       \n\
        (____)\n\
         (___)__.---\n\
";

API void show_rps(rps_t c, bool shift, bool player) {
	int y = (shift) ? 1 : 0;
	WINDOW* w;
	if(player) {
		w = wDisplayL;
	} else {
		w = wDisplayR;
	}
	wclear(w);
	switch(c) {
		case rock:
			if(player) {
				mvwprintw(w, y, 0, gpszRock);
			} else {
				mvwprintw(w, y, 0, gpszRockRight);
			}
			break;
		case paper:
			if(player) {
				mvwprintw(w, y, 0, gpszPaper);
			} else {
				mvwprintw(w, y, 0, gpszPaperRight);
			}
			break;
		case scissors:
			if(player) {
				mvwprintw(w, y, 0, gpszScissors);
			} else {
				mvwprintw(w, y, 0, gpszScissorsRight);
			}
			break;
		default:
			mvwprintw(w, y, 0, "ERROR");
			break;
	}
	wrefresh(w);
}

internal void show_text(const char* pszStatus, WINDOW* win) {
	wclear(win);
	mvwprintw(win, 0, 0, pszStatus);	
	wrefresh(win);
}

API void show_status(const char* pszStatus) {
	show_text(pszStatus, wStatus);
}

API void show_status2(const char* pszStatus) {
	show_text(pszStatus, wInput);
}

API rps_t input_choice() {
	do {
		auto c = getchar();
		while(c == '\n') {
			c = getchar();
		}

		switch(c) {
			case 'r':
				return rock;
			case 'p':
				return paper;
			case 's':
				return scissors;
			case 'q':
				return rps_max;
		}
	} while(true);
	return rps_max;
}

API void ssleep(int seconds) {
	sleep(seconds);
}

API void clear_display() {
	wclear(wDisplayL);
	wrefresh(wDisplayL);
	wclear(wDisplayR);
	wrefresh(wDisplayR);
}
