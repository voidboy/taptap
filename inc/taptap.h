#ifndef TAPTAP_H
#define TAPTAP_H

#include <stddef.h>
#include <stdbool.h>

#define WORDS_PER_CYCLE 5

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
	bool	is_modded;
	bool	cursor_disable;
	int		Cforeground_color;
	int		Cbackground_color;
} s_terminal;	

typedef struct word {
	int		x;
	int		y;
	char	*value;
	int		len;
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

typedef struct pool {
	s_word	*words;
	size_t	size;
	size_t	cursor;
} s_pool;

typedef struct node {
	s_word		*word;
	struct node	*next;
	struct node	*last;
} s_node;

typedef struct llist {
	s_node	*head;
	s_node	*tail;
	size_t	size;
} s_llist;	
#endif
