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
#include "taptap.h"
#include "foo.h"
#include "utils.h"
#include "split.h"

void remove_node(s_llist *Wwords, s_node *node, s_llist *Fwords)
{
	if (node->last)
		node->last->next = node->next;
	else
	{
		Wwords->head = node->next;
		if (node->next)
			node->next->last = NULL;
	}
	if (node->next)
		node->next->last = node->last;
	else
	{
		Wwords->tail = node->last;
		if (node->last)
			node->last->next = NULL;
	}
	node->next = NULL;
	free(node->word->value);
	if (Fwords->tail) 
	{
		Fwords->tail->next = node;
		node->last = Fwords->tail;
		Fwords->tail = node;
	}
	else
	{
		Fwords->head = node;
		Fwords->tail = node;
		node->last = NULL;
	}
	Fwords->size += 1;
}


bool check_words(char *input, s_llist *Wwords, s_llist *Fwords)
{
	s_node *current_node = Wwords->head;
	while (current_node)
	{
		if (!strcmp(current_node->word->value, input))
		{
			remove_node(Wwords, current_node, Fwords);
			return true;
		}
		current_node = current_node->next;
	}
	return false;
}

char *extract_file_content(const char *filename)
{

	struct stat	statbuf;
	FILE		*f = fopen(filename, "r");

	if (f == NULL)
		_abort("fopen", 0, __FILE__, __LINE__);
	if (stat(filename, &statbuf) == -1)
		_abort("stat", -1, __FILE__, __LINE__);
	char *content = malloc(sizeof(char) * (statbuf.st_size + 1));
	if (content == NULL)
		_abort("malloc", 0, __FILE__, __LINE__);
	if (fread(content, 1, statbuf.st_size, f) !=
		(size_t)statbuf.st_size)
		_abort("fread", 0, __FILE__, __LINE__);
	content[statbuf.st_size] = '\0';
	return content;
}


s_word *to_words(char **splitted, size_t splitted_size)
{
	s_word *words_pool = malloc(sizeof(s_word) * splitted_size);
	for (size_t i = 0; i < splitted_size; i++)
	{
		words_pool[i].value = splitted[i];
		words_pool[i].len = strlen(splitted[i]);
		words_pool[i].x = -words_pool[i].len;
		words_pool[i].y = rand() % terminal.number_of_lines;
	}
	free(splitted);
	return words_pool;
}

size_t get_entries(s_entry **entries)
{
	size_t			i, counter = 0;
	struct dirent	*current;
	DIR				*directory;

	directory = opendir("wordlists");
	if (directory == NULL)
		_abort("opendir(\"wordlists\")", 0, __FILE__, __LINE__);
	while ((current = readdir(directory)) != NULL)
	{
		if (strcmp(".",  current->d_name) == 0 ||
			strcmp("..", current->d_name) == 0 ||
			current->d_type != DT_REG)
			continue ;
		else 
			counter++;
	}
	if ((*entries = malloc(sizeof(s_entry)* counter)) == NULL)
		_abort("malloc", 0, __FILE__, __LINE__);
	rewinddir(directory);
	for (i = 0; (current = readdir(directory)) != NULL;)
	{
		if (strcmp(".",  current->d_name) == 0 ||
			strcmp("..", current->d_name) == 0 ||
			current->d_type != DT_REG)
			continue ;
		char	path[256] = {"wordlists/"};
		(*entries)[i].content = extract_file_content(
				strcat(path, current->d_name));
		(*entries)[i].filename = strdup(current->d_name);
		(*entries)[i].words_counter = count_words(
				(*entries)[i].content, " \n\t");
		++i;
	}
	closedir(directory); return counter;
}

s_entry select_wordlist(void)
{
	char	input;
	s_entry	ret, *entries = NULL;
	size_t	selected = 0;
	size_t	counter = get_entries(&entries);

	for (int j = 0; 1; j++)
	{
		if (j == 256) j = 0;
		screen_clear();
		printf("\t\t\t\tpress space to select %s\n\n",
			entries[selected].filename);
		for (size_t i = 0; i < counter; i++)
		{
			if (i == selected)
			{
				change_background_color(COLOR_WHITE);
				change_foreground_color(COLOR_BLACK);
				// FIXME: Sanitize preview, remove \n\t etc.., verify lengh
				printf("%15s - (%4lu words) [%-50.50s]\n",
					entries[i].filename,
					entries[i].words_counter,
					entries[i].content + j);
				reset_color();
			}
			else 
			{
				printf("%15s - (%4lu words) [%-50.50s]\n",
					entries[i].filename,
					entries[i].words_counter,
					entries[i].content);
			}
		}
		if (read(STDIN_FILENO, &input, 1) == 1)
		{
			if (input == 'j' && selected < counter - 1) selected++;
			else if (input == 'k' && selected > 0) selected--;
			else if (input == ' ') break ;
		}
		milli_sleep(50);
	}
	ret = entries[selected];
	for (size_t i = 0; i < counter; i++)
	{
		if (i == selected) continue;
		free(entries[i].content);
		free(entries[i].filename);
	}
	free(entries); return ret;
}
