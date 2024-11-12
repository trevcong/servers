/* echoclient.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cnaiapi.h>

#define BUFFSIZE		256
#define INPUT_PROMPT		"Input   > "
#define RECEIVED_PROMPT		"Received> "

int readln(char *, int);

/*-----------------------------------------------------------------------
 *
 * Program: echoclient
 * Purpose: contact echoserver, send user input and print server response
 * Usage:   echoclient <compname> [appnum]
 * Note:    Appnum is optional. If not specified the standard echo appnum
 *          (7) is used.
 *
 *-----------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	computer	comp;
	appnum		app;
	connection	conn;
	char		buff[BUFFSIZE];
	int		expect, received, len;

        //printf("debug: made it to pre-arg-testing\n");
	if (argc < 2 || argc > 3) {
		(void) fprintf(stderr, "usage: %s <compname> [appnum]\n",
			       argv[0]);
		exit(1);
	}

	/* convert the arguments to binary format comp and appnum */

        //printf("debug: made it to pre-cname-conversion\n");
	comp = cname_to_comp(argv[1]);
	if (comp == -1)
		exit(10);

        //printf("debug: made it to pre-appnum-conversion\n");
	if (argc == 3)
		app = (appnum) atoi(argv[2]);
	else
		if ((app = appname_to_appnum("echo")) == -1)
			exit(11);
	
	/* form a connection with the echoserver */

        //printf("debug: comp, appnum = %d, %d\n",comp,app);
        //printf("debug: made it to pre-conn\n");
	conn = make_contact(comp, app);
	if (conn < 0) 
		exit(12);

        //printf("debug: made it to post-conn, pre-printf\n");
	(void) printf(INPUT_PROMPT);
	(void) fflush(stdout);

	/* iterate: read input from the user, send to the server,	*/
	/*	    receive reply from the server, and display for user */

	while((len = readln(buff, BUFFSIZE)) > 0) {

		/* send the input to the echoserver */

		(void) send(conn, buff, len, 0);
		(void) printf(RECEIVED_PROMPT);
		(void) fflush(stdout);

		/* read and print same no. of bytes from echo server */

		expect = len;
		for (received = 0; received < expect;) {
		   len = recv(conn, buff, (expect - received) < BUFFSIZE ?
				 (expect - received) : BUFFSIZE, 0);
			if (len < 0) {
				send_eof(conn);
				return 1;
			}
			(void) write(STDOUT_FILENO, buff, len);
			received += len;
		}
		(void) printf("\n");
		(void) printf(INPUT_PROMPT);
		(void) fflush(stdout);
	}

	/* iteration ends when EOF found on stdin */

	(void) send_eof(conn);
	(void) printf("\n");
	return 0;
}
