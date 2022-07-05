#ifndef UTILS_H
#define UTILS_H
#include "taptap.h"

void stars_animation(void);
void _abort(
	const char *function,
	const int return_code,
	const char *file,
	const int line);
void setup_sigcallback(void);
void display_terminal_info(void);
void get_terminal_info();
void get_terminal_capa();
void update(s_word *words, size_t words_counter);
void milli_sleep(long milliseconds);
s_entry select_wordlist(void);
s_word *to_words(char **wordlist, size_t words_counter);
#endif
