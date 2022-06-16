#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <term.h>
#include "utils.h"
#include "taptap.h"
#include "split.h"

int main(void)
{
	get_terminal_info();
	get_terminal_capa();
	setup_sigcallback();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	s_entry wordlist = select_wordlist();
	char **splitted = split(wordlist.content, " \n\t");
	s_word *words = to_words(splitted, wordlist.words_counter);
	while (1) 
	{
		char c[50];
		int value;
		ioctl(STDIN_FILENO, TIOCINQ, &value);
		update(words, wordlist.words_counter);
		c[read(STDIN_FILENO, c, value)] = '\0';
		//printf("RECEIVED %s\n", c);
		milli_sleep(500);
	}
	//display_terminal_info(&terminal);

	return 0;
}
