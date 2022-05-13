#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
#include <term.h>
#include "utils.h"

int main(int argc, char *argv[])
{

	(void)argc;
	(void)argv;
	get_terminal_info();
	get_terminal_capa();
	setup_sigcallback();
	select_wordlist();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	while (1) 
	{
		char c[50];
		int value;
		ioctl(STDIN_FILENO, TIOCINQ, &value);
		update();
		c[read(STDIN_FILENO, c, value)] = '\0';
		printf("RECEIVED %s\n", c);
		sleep(1);
	}
	//display_terminal_info(&terminal);

	return 0;
}
