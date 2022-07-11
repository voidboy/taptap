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
#include "hsignal.h"

static void init_stars(s_star **stars)
{
	const int stars_counter = 0.10f * terminal.number_of_lines;

	*stars = malloc(sizeof(s_star) * stars_counter);
	if (*stars == NULL)
		_abort("malloc", 0, __FILE__, __LINE__);
	for (int i = 0; i < stars_counter; i++)
	{
		(*stars)[i].x = -1 * (rand() %
			(terminal.number_of_columns / 3));
		(*stars)[i].y = rand() % terminal.number_of_lines;
		(*stars)[i].speed = (rand() % 3) + 1;
	}
}

void stars_animation(void)
{
	static s_star	*stars = NULL;
	static short	speed = 1;
	const short		stars_counter = 0.10f * terminal.number_of_lines;
	

	if (!stars) init_stars(&stars);

	for (int i = 0; i < stars_counter; i++)
	{
		if (stars[i].x >= terminal.number_of_columns)
		{
			stars[i].x = -1 * (rand() % 
				(terminal.number_of_columns / 3));
			stars[i].y = rand() % terminal.number_of_lines;
		}
		else if (stars[i].x >= 0)
		{
			cursor_move(stars[i].x, stars[i].y);
			change_foreground_color(COLOR_WHITE);
			write(STDOUT_FILENO, ".", 1);
		}
		if (stars[i].speed <= speed) stars[i].x += 1;
	}
	if (++speed > 3) speed = 0;
}

void milli_sleep(long milliseconds)
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

void update(s_word *words, size_t words_counter) 
{
	static short difficulty = 100;
	static short tik = 0;

	for (unsigned long i = 0; i < words_counter; i++)
	{
		if (words[i].status & (MISSED | VALIDATED)) continue ;
		if (words[i].x < 0)
		{
			if (words[i].x + words[i].len < 0)
			{
				if (tik == difficulty)
					words[i].x += 1;
				continue ;
			}
			else 
			{
				int len = words[i].x + words[i].len; 
				cursor_move(0, words[i].y);
				select_color(words[i].x);
				write(STDOUT_FILENO, words[i].value + (-1 * words[i].x), len);
				words[i].status = VISIBLE;
			}
		}
		else 
		{
			int len = words[i].len;
			// Missed the word
			if (words[i].x >= terminal.number_of_columns)
				words[i].status = MISSED;
			else if (words[i].x + len > terminal.number_of_columns)
				len = terminal.number_of_columns - words[i].x;
			if (words[i].status == VISIBLE)
			{
				cursor_move(words[i].x, words[i].y);
				select_color(words[i].x);
				write(STDOUT_FILENO, words[i].value, len);
			}
		}
		if (tik == difficulty)
			words[i].x += 1;
	}
	if (++tik > difficulty) tik = 0;
}
