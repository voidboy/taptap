#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <term.h>
#include "foo.h"
#include "utils.h"
#include "taptap.h"
#include "split.h"
#include "words.h"

int main(void)
{
	struct timeval 	ref;
	struct timeval 	now;
	int				cursor = 0;
	size_t			validated = 0;
	char			c[50] = {0};


	get_terminal_info();
	get_terminal_capa();
	setup_sigcallback();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	srand(time(NULL));

	s_entry wordlist = select_wordlist();
	char **splitted = split(wordlist.content, " \n\t");
	s_word *words = to_words(splitted, wordlist.words_counter);

	
	gettimeofday(&ref, NULL);
	while (1) 
	{
		screen_clear();
		gettimeofday(&now, NULL);
		stars_animation();	
		update_words(words, wordlist.words_counter);
		display_words(words, wordlist.words_counter);
		if (read(STDIN_FILENO, &c[cursor], 1) == 1)
		{
			if (c[cursor] == 0x7f)
				c[cursor > 0 ? --cursor : cursor] = '\0';
			else
				c[++cursor] = '\0';
			if (check_words(c, words, wordlist.words_counter))
			{
				validated++;
				c[0] = '\0';
				cursor = 0;
			}
		}
		cursor_move(10, 10);
		change_foreground_color(COLOR_WHITE);
		printf("[%15s] %3.2f %3ld/%3ld\n", c,
			(now.tv_sec - ref.tv_sec) + 
			(double)now.tv_usec * 1e-6,
			validated, wordlist.words_counter);
		milli_sleep(5);
	}
	//display_terminal_info(&terminal);

	return 0;
}
