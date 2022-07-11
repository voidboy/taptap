#ifndef WORDS_H
#define WORDS_H
char *extract_file_content(const char *filename);
s_word *to_words(char **wordlist, size_t words_counter);
size_t get_entries(s_entry **entries);
s_entry select_wordlist(void);
bool check_words(char *input, s_word *words, size_t words_counter);
#endif
