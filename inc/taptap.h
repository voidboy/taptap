#ifndef TAPTAP_H
#define TAPTAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct terminal {
	int		number_of_lines;
	int		number_of_columns;
	char	*screen_clear;
	char	*cursor_move;
	char	*foreground_color;
	char	*background_color;
	char	*reset_attributs;
	char	*invisible_cursor;
	char	*visible_cursor;
	bool	is_dirty;
	bool	is_colored;
	bool	is_modded;
	bool	cursor_disable;
} s_terminal;	

typedef enum status {
	INVISIBLE,
	VISIBLE,
	MISSED,
	VALIDATED,
} e_status;

typedef struct word {
	int			x;
	int			y;
	const char *value;
	int			len;
	e_status	status;
} s_word;

typedef struct entry {
	char	*filename;
	char	*content;
	size_t	words_counter;
} s_entry;

typedef struct star {
	int		x;
	int		y;
	short	speed;
} s_star;
#endif
