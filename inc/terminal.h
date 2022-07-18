#ifndef TERMINAL_H
#define TERMINAL_H

bool is_terminal_colored(void);
void echo_and_canonical_modes(bool enable);
void display_terminal_info(void);
void get_terminal_capa(void);
void get_terminal_info();

#endif
