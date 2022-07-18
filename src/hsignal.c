#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "foo.h"
#include "terminal.h"

#define M_SIGS "Received SIGSEGV, quitting.\n"
#define M_SIGQ "Received SIGQUIT, quitting.\n"
#define M_SIGI "Received SIGINT, quitting.\n"
#define M_SIGU "Received unhandled signal, quitting.\n"

void _abort(
	const char *function,
	const int return_code,
	const char *file,
	const int line)
{
	if (terminal.is_modded)
		echo_and_canonical_modes(true);
	if (is_terminal_colored())
		reset_color();
	if (terminal.cursor_disable)
		change_cursor_visibility(true);
	screen_clear();
	dprintf(STDERR_FILENO,
		"error - %s returned %d(%s:%d)\n",
		function, return_code, file, line);
	exit(EXIT_FAILURE);
}

void sigresize(int signum)
{
	if (signum == SIGWINCH)
	{
		terminal.is_dirty = true;
		get_terminal_info();
	}
}

void sigbyebye(int signum)
{
	echo_and_canonical_modes(true);
	change_cursor_visibility(true);
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

void setup_sigcallback(void)
{
	struct sigaction change = 
	(struct sigaction)
	{
		.sa_handler = sigbyebye,
		.sa_flags = 0,
		.sa_restorer = NULL,
	};
	if (sigemptyset(&change.sa_mask) == -1)
		_abort("sigemtpyset", -1, __FILE__, __LINE__);
	if (sigaction(SIGQUIT, &change, NULL) == -1)
		_abort("sigaction(\"SIGQUIT\"", -1, __FILE__, __LINE__);
	if (sigaction(SIGSEGV, &change, NULL) == -1)
		_abort("sigaction(\"SIGSEGV\"", -1, __FILE__, __LINE__);
	if (sigaction(SIGINT, &change, NULL) == -1)
		_abort("sigaction(\"SIGINT\"", -1, __FILE__, __LINE__);
	change.sa_handler = sigresize;
	if (sigaction(SIGWINCH, &change, NULL) == -1)
		_abort("sigaction(\"SIGWINCH\"", -1, __FILE__, __LINE__);
	echo_and_canonical_modes(false);
	change_cursor_visibility(false);
}
