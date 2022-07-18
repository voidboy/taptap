#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "taptap.h"
#include "foo.h"
#include "words.h"
#include "hsignal.h"

void init_stars(s_star **stars)
{
	const int stars_counter = 0.10f * terminal.number_of_lines;

	*stars = malloc(sizeof(s_star) * stars_counter);
	if (*stars == NULL)
		_abort("malloc", 0, __FILE__, __LINE__);
	for (int i = 0; i < stars_counter; i++)
	{
		(*stars)[i].speed = rand() % 3;
		(*stars)[i].x = -(*stars)[i].speed;
		(*stars)[i].y = rand() % terminal.number_of_lines;
	}
}

void stars_animation(s_star *stars, const short stars_speed)
{
	const short	stars_counter = 0.10f * terminal.number_of_lines;
	

	for (int i = 0; i < stars_counter; i++)
	{
		if (stars[i].x == terminal.number_of_columns)
		{
			stars[i].speed = rand() % 3;
			stars[i].x = -stars[i].speed;
			stars[i].y = rand() % terminal.number_of_lines;
		}
		else if (stars[i].x >= 0)
		{
			cursor_move(stars[i].x, stars[i].y);
			change_foreground_color(COLOR_WHITE);
			write(STDOUT_FILENO, ".", 1);
		}
		if (stars[i].speed <= stars_speed) stars[i].x += 1;
	}
}

void milli_sleep(const long milliseconds)
{
	struct timeval			now;
	struct timeval			ref;
	const struct timespec	req = 
		(struct timespec)
		{
			.tv_sec = 0,
			.tv_nsec = 1e6,
		};

	gettimeofday(&ref, NULL);
	while (1)
	{
		nanosleep(&req, NULL);
		gettimeofday(&now, NULL);
		const unsigned long t1 = ref.tv_sec * 1e6 + ref.tv_usec
			+ milliseconds * 1e3;
		const unsigned long t2 = now.tv_sec * 1e6 + now.tv_usec;
		if (t2 >= t1) break ;
	}
}

void display_words(s_llist *Wwords) 
{
	const s_node *current_node = Wwords->head;
	while (current_node)
	{
		if (current_node->word->x < 0)
		{
			if (current_node->word->x + current_node->word->len >= 0)
			{
				int len = current_node->word->x + current_node->word->len; 
				cursor_move(0, current_node->word->y);
				select_color(current_node->word->x);
				write(STDOUT_FILENO, current_node->word->value +
					(-1 * current_node->word->x), len);
			}
		}
		else 
		{
			int len = current_node->word->len;
			if (current_node->word->x + len > terminal.number_of_columns)
				len = terminal.number_of_columns - current_node->word->x;
			cursor_move(current_node->word->x, current_node->word->y);
			select_color(current_node->word->x);
			write(STDOUT_FILENO, current_node->word->value, len);
		}
		current_node = current_node->next;
	}
}

void update_words(s_llist *Wwords, s_llist *Fwords) 
{
	s_node *current_node = Wwords->head;
	while (current_node)
	{
		if (current_node->word->x == terminal.number_of_columns)
			remove_node(Wwords, current_node, Fwords);
		else
			current_node->word->x += 1;
		current_node = current_node->next;
	}
}
