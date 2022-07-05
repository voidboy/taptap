#include <term.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include "taptap.h"
#include "hsignal.h"

s_terminal terminal = (s_terminal)
{ 
	.number_of_lines = 0,
	.number_of_columns = 0,
	.screen_clear = NULL,
	.cursor_move = NULL,
	.foreground_color = NULL,
	.reset_attributs = NULL,
	.invisible_cursor = NULL,
	.visible_cursor = NULL,
	.is_dirty = true,
	.is_colored = false,
	.is_modded = false,
	.cursor_disable = false,
};

void echo_and_canonical_modes(bool enable)
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

void display_terminal_info(void)
{
	printf("Terminal size: %dx%d\n", 
		terminal.number_of_columns,
		terminal.number_of_lines);
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
	terminal.invisible_cursor = tigetstr("civis");
	if (terminal.invisible_cursor  == NULL ||
		terminal.invisible_cursor == (char *)-1)
		_abort("tigetstr(\"civis\")",
		(long)terminal.invisible_cursor, __FILE__, __LINE__);
	terminal.visible_cursor = tigetstr("cnorm");
	if (terminal.visible_cursor  == NULL ||
		terminal.visible_cursor == (char *)-1)
		_abort("tigetstr(\"cnorm\")",
		(long)terminal.visible_cursor, __FILE__, __LINE__);
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
