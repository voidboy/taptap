typedef struct terminal {
	int		number_of_lines;
	int		number_of_columns;
	char	*screen_clear;
	char	*cursor_move;
	char	*foreground_color;
	char	*background_color;
	char	*reset_attributs;
	bool	is_dirty;
	bool	is_colored;
	bool	is_modded;
} s_terminal;	

typedef struct word {
	int			x;
	int			y;
	const char *value;
} s_word;
