#ifndef UTILS_H
#define UTILS_H
#include "taptap.h"

void init_stars(s_star **stars);
void stars_animation(s_star *stars, const short stars_speed);
void _abort(
	const char *function,
	const int return_code,
	const char *file,
	const int line);
void setup_sigcallback(void);
void display_terminal_info(void);
void get_terminal_info();
void get_terminal_capa();
void display_words(s_llist *Wwords);
void update_words(s_llist *Wwords, s_llist *Fwords); 
void milli_sleep(const long milliseconds);
s_entry select_wordlist(void);
s_word *to_words(char **wordlist, size_t words_counter);
#endif
