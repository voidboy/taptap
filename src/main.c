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
	struct timeval ref;
	struct timeval now;


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
		char c[50];
		int value;
		ioctl(STDIN_FILENO, TIOCINQ, &value);
		screen_clear();
		gettimeofday(&now, NULL);
		cursor_move(10, 10);
		change_foreground_color(COLOR_WHITE);
		printf("%3.2f\n",
			(now.tv_sec - ref.tv_sec) + 
			(double)now.tv_usec * 1e-6);
		stars_animation();	
		update(words, wordlist.words_counter);
		c[read(STDIN_FILENO, c, value)] = '\0';
		//printf("RECEIVED %s\n", c);
		milli_sleep(5);
	}
	//display_terminal_info(&terminal);

	return 0;
}
