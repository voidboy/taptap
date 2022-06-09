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

static bool is_availaible
	(int x, int y, int len, short **matrix)
{
	for (int i = 0; i < len; i++)
	{
		if (x + len > 41) return false;
		if (matrix[y][x + i]) return false;
	}
	return true;
}


s_word *to_words(char **wordlist)
{
	short **matrix = calloc(42, sizeof(short *));
	for (int i = 0; i < 42; i++)
		matrix[i] = calloc(42, sizeof(short));
	size_t words_counter = 0;
	s_word *words;

	for (size_t i = 0; wordlist[i]; i++)
		words_counter++;
	words = malloc(sizeof(s_word) * words_counter);
	srand(time(NULL));
	for (size_t i = 0; i < 42; i++)
	{
		int x = rand() % 42;
		int y = rand() % 42;
		
		while (!is_availaible(x, y, strlen(wordlist[i]), matrix))
		{
			x = rand() % 42;
			y = rand() % 42;
		}
		for (int i = 0; i < (int)strlen(wordlist[i]); i++)
			matrix[y][x + i] = 1;
		words[i].value = wordlist[i];
		words[i].x = -1 * x;
		words[i].y = +1 * y;
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
