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

void increase_Wwords(s_llist *Wwords, s_llist *Fwords, s_pool *words_pool)
{
	if (Fwords->size >= WORDS_PER_CYCLE)
	{
		s_node *free_node = Fwords->head;
		free_node->last = Wwords->tail;
		if (!Wwords->head)
			Wwords->head = free_node;
		for (int i = 0; i < WORDS_PER_CYCLE; i++)
		{
			free_node->word = &words_pool->words[words_pool->cursor++];
			free_node = free_node->next;
		}
		if (Wwords->tail)
			Wwords->tail->next = Fwords->head;
		Fwords->head = free_node;
		if (free_node)
		{
			Wwords->tail = free_node->last;
			free_node->last = NULL;
		}
		else
		{
			Wwords->tail = Fwords->tail;
			Fwords->tail = NULL;
		}
		Wwords->tail->next = NULL;
		Fwords->size -= WORDS_PER_CYCLE;
	}
	else 
	{
		s_node *new_node, *last = Wwords->tail;
		for (int i = 0; i < WORDS_PER_CYCLE; i++)
		{
			new_node = malloc(sizeof(s_node));
			new_node->word = &words_pool->words[words_pool->cursor++];
			new_node->next = NULL;
			if (last)
				last->next = new_node;
			new_node->last = last;
			last = new_node;
			if (!Wwords->head) Wwords->head = new_node;
		}
		Wwords->tail = new_node;
	}
}

int main(void)
{
	struct timeval 	ref;
	struct timeval 	now;

	int				cursor = 0;
	size_t			validated = 0;
	size_t			wpm = 0, counter = 0;
	size_t			tick = 0;
	char			c[50] = {0};

	s_llist			wait_words = {
		.head = NULL,
		.tail = NULL,
		.size = 0,
	};
	s_llist			free_words = {
		.head = NULL,
		.tail = NULL,
		.size = 0,
	};
	s_star			*stars;

	get_terminal_info();
	get_terminal_capa();
	setup_sigcallback();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	srand(time(NULL));

	s_entry wordlist = select_wordlist();
	char **splitted = split(wordlist.content, " \n\t");
	s_pool words_pool = {
		.words = to_words(splitted,
				wordlist.words_counter),
		.size = wordlist.words_counter,
		.cursor = 0,
	};

	
	init_stars(&stars);
	gettimeofday(&ref, NULL);
	increase_Wwords(&wait_words, &free_words, &words_pool);
	while (1) 
	{
		gettimeofday(&now, NULL);
		screen_clear();
		stars_animation(stars, tick % 3);	

		const double delta_time = 
			(now.tv_sec + (double)now.tv_usec * 1e-6) - 
			(ref.tv_sec + (double)ref.tv_usec * 1e-6);
		if (tick && tick % 100 == 0)
		{
			update_words(&wait_words, &free_words);
			wpm = (counter / 5) / (delta_time / 60);
		}
		display_words(&wait_words);
		if (read(STDIN_FILENO, &c[cursor], 1) == 1)
		{
			counter++;
			if (c[cursor] == 0x7f)
				c[cursor > 0 ? --cursor : cursor] = '\0';
			else if (c[cursor] == 0x1b)
				break ;
			else
				c[++cursor] = '\0';
			if (check_words(c, &wait_words, &free_words))
			{
				validated++;
				c[0] = '\0';
				cursor = 0;
			}
		}
		cursor_move(10, 10);
		change_foreground_color(COLOR_WHITE);
		printf("[%15s] %3.2f %3ld/%3ld - %ld WPM\n", c,
			delta_time, validated, wordlist.words_counter, wpm);
		if (tick++ == 1000)
		{
			increase_Wwords(&wait_words, &free_words, &words_pool);
			tick = 0;
		}
		milli_sleep(5);
	}
	return 0;
}
