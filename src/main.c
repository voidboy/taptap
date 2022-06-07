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

s_word *to_words(char **wordlist)
{
	size_t words_counter = 0;
	s_word *words;

	for (size_t i = 0; wordlist[i]; i++)
		words_counter++;
	words = malloc(sizeof(s_word) * words_counter);
	srand(time(NULL));
	for (size_t i = 0; i < words_counter; i++)
	{
		words[i].value = wordlist[i];
		words[i].x = -1 * (rand() % 42);
		words[i].y = +1 * (rand() % 42);
	}
	free(wordlist);
	return words;
}

int main(void)
{

	get_terminal_info();
	get_terminal_capa();
	setup_sigcallback();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	char **wordlist = select_wordlist();
	s_word *words = to_words(wordlist);
	while (1) 
	{
		char c[50];
		int value;
		ioctl(STDIN_FILENO, TIOCINQ, &value);
		update(words);
		c[read(STDIN_FILENO, c, value)] = '\0';
		printf("RECEIVED %s\n", c);
		sleep(1);
	}
	//display_terminal_info(&terminal);

	return 0;
}
