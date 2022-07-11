#include <stdlib.h>
#include <stdbool.h>

static bool is_in(const char c, const char *charset)
{
	while (*charset)
	{
		if (c == *charset) 
			return true;
		else
			charset++;
	}
	return false;
}


size_t count_words(const char *str, const char *charset)
{
	int		i;
	size_t	words_count;
	char	last;

	words_count = 0;
	i = 0;
	last = charset[0];
	while (str[i])
	{
		if (is_in(last, charset) && !is_in(str[i], charset))
			words_count++;
		last = str[i];
		i++;
	}
	return (words_count);
}

static int find_next_word(char *dest, const char *str, const char *charset)
{
	int i;

	i = 0;
	while (str[i] && !is_in(str[i], charset))
	{
		if (dest)
			dest[i] = str[i];
		i++;
	}
	if (dest)
		dest[i] = '\0';
	return (i);
}

static char **alloc_bf(int words_counter, char const *str, const char *charset)
{
	char	**s;
	int		i;
	int		word_len;

	if ((s = malloc(sizeof(char *) * (words_counter + 1))) == NULL)
		return (NULL);
	i = 0;
	while (i < words_counter)
	{
		while (is_in(*str, charset))
			str++;
		word_len = find_next_word(NULL, str, charset);
		if ((s[i++] = malloc(sizeof(char) * word_len + 1)) == NULL)
		{
			while (--i >= 0)
				free(s[i]);
			free(s);
			return (NULL);
		}
		str += word_len + 1;
	}
	s[i] = NULL;
	return (s);
}

char **split(const char *str, const char *charset)
{
	int		i;
	int		j;
	size_t	counter_words;
	char	**s;

	if (!str)
		return (NULL);
	if ((s = alloc_bf(count_words(str, charset), str, charset)) == NULL)
		return (NULL);
	i = 0;
	j = 0;
	counter_words = 0;
	const size_t total_words = count_words(str, charset);
	
	while (counter_words < total_words)
	{
		while (is_in(str[i], charset))
			i++;
		i += find_next_word(s[j++], str + i, charset);
		counter_words++;
	}
	return (s);
}
