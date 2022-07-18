#ifndef FOO_H
#define FOO_H
#include <term.h>
#include <curses.h>
#include <stdbool.h>
#include "taptap.h"

extern s_terminal terminal;

__attribute__((always_inline)) inline static void
	change_cursor_visibility(bool enable)
{
	char *change = terminal.invisible_cursor;

	if (enable) change = terminal.visible_cursor;
	write(STDOUT_FILENO, change, strlen(change));
	terminal.cursor_disable = !enable;
}

__attribute__((always_inline)) inline static void
	change_foreground_color(const int new_color)
{
	if (new_color == terminal.Cforeground_color) return;
	char *change = tiparm(terminal.foreground_color, new_color);

	write(STDOUT_FILENO, change, strlen(change));
	terminal.Cforeground_color = new_color;
}

__attribute__((always_inline)) inline static void
	change_background_color(const int new_color)
{
	if (new_color == terminal.Cbackground_color) return;
	char *change = tiparm(terminal.background_color, new_color);

	write(STDOUT_FILENO, change, strlen(change));
	terminal.Cbackground_color = new_color;
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
	terminal.Cforeground_color = COLOR_WHITE;
	terminal.Cbackground_color = COLOR_BLACK;
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
#endif
