#ifndef UTILS_H
#define UTILS_H
#include "taptap.h"

void _abort(
	const char *function,
	const int return_code,
	const char *file,
	const int line);
void setup_sigcallback(void);
void display_terminal_info(void);
void get_terminal_info();
void get_terminal_capa();
void update(s_word *words);
char **select_wordlist(void);
#endif
