#ifndef SIGNAL_H
#define SIGNAL_H

void setup_sigcallback(void);
void sigresize(int signum);
void sigbyebye(int signum);
void _abort(
	const char *function,
	const int return_code,
	const char *file,
	const int line);

#endif
