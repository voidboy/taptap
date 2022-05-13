#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <term.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <curses.h>
#include <string.h>
#include "taptap.h"
#include "utils.h"
#define M_SIGS "Received SIGSEGV, quitting.\n"
#define M_SIGQ "Received SIGQUIT, quitting.\n"
#define M_SIGI "Received SIGINT, quitting.\n"
#define M_SIGU "Received unhandled signal, quitting.\n"

static s_terminal terminal =
	(s_terminal)
	{ 
		.number_of_lines = 0,
		.number_of_columns = 0,
		.screen_clear = NULL,
		.cursor_move = NULL,
		.foreground_color = NULL,
		.reset_attributs = NULL,
		.is_dirty = true,
		.is_colored = false,
		.is_modded = false,
	};


__attribute__((always_inline)) inline static void
	change_foreground_color(const int new_color)
{
	char *change = tiparm(terminal.foreground_color, new_color);
	write(STDOUT_FILENO, change, strlen(change));
	terminal.is_colored = true;
}

__attribute__((always_inline)) inline static void
	change_background_color(const int new_color)
{
	char *change = tiparm(terminal.background_color, new_color);
	write(STDOUT_FILENO, change, strlen(change));
	terminal.is_colored = true;
}

__attribute__((always_inline)) inline static void
	select_color(const int posx)
{
	const float urgency = posx / (float)terminal.number_of_columns;

	if (urgency <= 0.5)
		change_foreground_color(COLOR_GREEN);
	else if (urgency <= 0.75)
		change_foreground_color(COLOR_YELLOW);
	else
		change_foreground_color(COLOR_RED);
}

__attribute__((always_inline)) inline static void
	reset_color(void) 
{
	write(STDOUT_FILENO,
		terminal.reset_attributs,
		strlen(terminal.reset_attributs));
	terminal.is_colored = false;
}

__attribute__((always_inline)) inline static void
	screen_clear(void) 
{
	write(STDOUT_FILENO,
		terminal.screen_clear,
		strlen(terminal.screen_clear)); 
}

__attribute__((always_inline)) inline static void
	cursor_move(
	int x,
	int y)
{
	char *move = tgoto(terminal.cursor_move, x, y);
	write(STDOUT_FILENO, move, strlen(move));
}

static void milli_sleep(long milliseconds)
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

static void echo_and_canonical_modes(bool enable)
{
	struct termios	terminal_configuration;

	/* prevent infinity loop between _abort and echo_and_canonical_modes
	 * by setting is_modded first, no matter what happenned */
	terminal.is_modded = !enable;
	if (tcgetattr(STDIN_FILENO, &terminal_configuration) == -1)
		_abort("tcgetattr(\"stdin\")", -1, __FILE__, __LINE__);
	if (enable) 
		terminal_configuration.c_lflag |= (ICANON | ECHO);
	else terminal_configuration.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configuration) == -1)
		_abort("tcsetattr(\"stdin\")", -1, __FILE__, __LINE__);
}

static void sigresize(int signum)
{
	if (signum == SIGWINCH)
	{
		terminal.is_dirty = true;
		get_terminal_info();
	}
}

static void sigbyebye(int signum)
{
	echo_and_canonical_modes(true);
	reset_color();
	screen_clear();
	switch (signum)
	{
		case SIGINT: write(STDERR_FILENO, M_SIGI, strlen(M_SIGI)); 
			break ;
		case SIGQUIT: write(STDERR_FILENO, M_SIGQ, strlen(M_SIGQ));
			break ;
		case SIGSEGV: write(STDERR_FILENO, M_SIGS, strlen(M_SIGS));
			break ;
		default: write(STDERR_FILENO, M_SIGU, strlen(M_SIGU));
	}
	exit(EXIT_SUCCESS);
}

void display_terminal_info(void)
{
	printf("Terminal size: %dx%d\n", 
		terminal.number_of_columns,
		terminal.number_of_lines);
}

void _abort(
	const char *function,
	const int return_code,
	const char *file,
	const int line)
{
	if (terminal.is_modded)
		echo_and_canonical_modes(true);
	if (terminal.is_colored)
		reset_color();
	screen_clear();
	dprintf(STDERR_FILENO,
		"error - %s returned %d(%s:%d)\n",
		function, return_code, file, line);
	exit(EXIT_FAILURE);
}

void get_terminal_capa(void)
{

	if (terminal.is_dirty)
	{
		int	err_terminfo;

		if (setupterm(NULL, STDOUT_FILENO, &err_terminfo) == ERR)
			_abort("setupterm", err_terminfo, __FILE__, __LINE__);
		terminal.is_dirty = false;
	}
	terminal.screen_clear = tigetstr("clear");
	if (terminal.screen_clear == NULL ||
		terminal.screen_clear == (char *)-1)
		_abort("tigetstr(\"clear\")",
		(long)terminal.screen_clear, __FILE__, __LINE__);
	terminal.cursor_move = tigetstr("cup");
	if (terminal.cursor_move == NULL ||
		terminal.cursor_move == (char *)-1)
		_abort("tigetstr(\"cup\")",
		(long)terminal.cursor_move, __FILE__, __LINE__);
	terminal.foreground_color = tigetstr("setaf");
	if (terminal.foreground_color == NULL ||
		terminal.foreground_color == (char *)-1)
		_abort("tigetstr(\"setaf\")",
		(long)terminal.foreground_color, __FILE__, __LINE__);
	terminal.background_color = tigetstr("setab");
	if (terminal.background_color == NULL ||
		terminal.background_color == (char *)-1)
		_abort("tigetstr(\"setab\")",
		(long)terminal.background_color, __FILE__, __LINE__);
	terminal.reset_attributs = tigetstr("sgr0");
	if (terminal.reset_attributs == NULL ||
		terminal.reset_attributs == (char *)-1)
		_abort("tigetstr(\"sgr0\")",
		(long)terminal.reset_attributs, __FILE__, __LINE__);
}

void get_terminal_info()
{

	if (terminal.is_dirty)
	{
		int	err_terminfo;

		if (setupterm(NULL, STDOUT_FILENO, &err_terminfo) == ERR)
			_abort("setupterm", err_terminfo, __FILE__, __LINE__);
		terminal.is_dirty = false;
	}
	terminal.number_of_columns = tigetnum("cols");
	if (terminal.number_of_columns < 0)
		_abort("tigetnum(\"cols\")",
		terminal.number_of_columns, __FILE__, __LINE__);
	terminal.number_of_lines = tigetnum("lines");
	if (terminal.number_of_lines < 0)
		_abort("tigetnum(\"lines\")",
		terminal.number_of_lines, __FILE__, __LINE__);
}

void update(void) 
{
	static s_word words[] = {
		{ .x = 3, .y = 1, .value = "poney" }, 
		{ .x = 1, .y = 2, .value = "aurelien" }, 
		{ .x = 5, .y = 3, .value = "bonjour" }, 
		{ .x = 9, .y = 4, .value = "petit" }, 
	};

	screen_clear();
	for (unsigned long i = 0; i < sizeof(words) / sizeof(s_word); i++)
	{
		/* add color green, yellow, red */
		int len = strlen(words[i].value);
		if (words[i].x >= terminal.number_of_columns)
			words[i].x = 0;
		else if (words[i].x + len > terminal.number_of_columns)
			len = terminal.number_of_columns - words[i].x;
		cursor_move(words[i].x, words[i].y);
		select_color(words[i].x);
		write(STDOUT_FILENO, words[i].value, len);
		words[i].x += 1;
	}
}


void setup_sigcallback(void)
{
	struct sigaction change = 
	(struct sigaction)
	{
		.sa_handler = sigbyebye,
		.sa_flags = 0,
		.sa_restorer = NULL,
	};
	sigemptyset(&change.sa_mask);
	sigaction(SIGQUIT, &change, NULL);
	sigaction(SIGSEGV, &change, NULL);
	sigaction(SIGINT, &change, NULL);
	change.sa_handler = sigresize;
	sigaction(SIGWINCH, &change, NULL);
	echo_and_canonical_modes(false);
}

char *select_wordlist(void)
{
	DIR				*directory;
	struct dirent	*current;
	char			preview[3][256] = {{'\0'}};

	/* FIXME: won't work if launched outside taptap dir */
	directory = opendir("wordlists");
	if (directory == NULL)
		_abort("opendir(\"wordlists\")", 0, __FILE__, __LINE__);
	errno = 0;
	for (int i = 1; (current = readdir(directory)) != NULL; i++)
	{
		if (strcmp(".",  current->d_name) == 0 ||
			strcmp("..", current->d_name) == 0)
		{
			i--; continue ;
		}
		char	path[50] = {"wordlists/"};
		strcat(path, current->d_name);
		FILE		*f = fopen(path, "r");
		size_t len = fread(preview[i - 1], 1, 256, f);
		preview[i - 1][len] = '\0';
	}
	if (errno)
	{
		closedir(directory);
		_abort("opendir(\"wordlists\")", 0, __FILE__, __LINE__);
	}
	char input;
	int selected = 0;
	for (int j = 0; 1; j++)
	{
		screen_clear();
		for (int i = 0; i < 3; i++)
		{
			if (i == selected)
			{
				change_background_color(COLOR_WHITE);
				change_foreground_color(COLOR_BLACK);
				printf("%d - [%.50s]\n", i, preview[i] + j);
				reset_color();
			}
			else 
			{
				printf("%d - [%.50s]\n", i, preview[i] + j);
			}
		}
		if (read(STDIN_FILENO, &input, 1) == 1)
		{
			if (input == 'j' && selected > 0) selected--;
			if (input == 'k' && selected < 2) selected++;
		}
		milli_sleep(500);
	}
	//rewinddir(directory);
	closedir(directory);
	return NULL;
}