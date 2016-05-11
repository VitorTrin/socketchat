// Esse é um proxy simples


#include "mysocket.h"  
#include <pthread.h>

#define BUFSIZE 100
#define NTHREADS 100
#define LINESIZE 100
#define HEADERSIZE 1000 // número tirado de uma cartola

/* Structure of arguments to pass to client thread */
struct TArgs {
	TSocket cliSock;   /* socket descriptor for client */
};

int population;

void * HandleRequest(void *args) { 

	char request[LINESIZE];
	char header [HEADERSIZE];
	char lastLine[LINESIZE];
	char penultimateline[LINESIZE];
	int stillReading = 1;
	
	memset(lastLine,0,strlen(lastLine));
	memset(penultimateline,0,strlen(penultimateline));



	/* Extract socket file descriptor from argument */
	cliSock = ((struct TArgs *) args) -> cliSock;
	free(args);  /* deallocate memory for argument */


	//Le a linha de requisição e os cabeçalhos até encontrar duas linhas novas consecutivas;

	//ideia: comparar as duas ultimas linhas com "\n\n"||"\n\r\n"||\r\n\r\n" depois de cada readline

	//Requisição

	if (ReadLine(cliSock, request, LINESIZE-1) < 0) 
			{ ExitWithError("ReadLine() failed");

	//Cabeçalho

	while(stillReading)
	{


		strcpy(penultimateline,lastLine);

		if (ReadLine(cliSock, lastLine, LINESIZE-1) < 0) 
			{ ExitWithError("ReadLine() failed");

		strcat(header,lastLine); // adiciona a ultima linha lida 

		//se forem 2 linhas seguidas 

		if((strncasecmp(penultimateline, "\n", 1) == 0 ||
		   (strncasecmp(penultimateline, "\r\n", 2) == 0))
		{
			if((strncasecmp(penultimateline, "\n", 1) == 0 ||
		   	(strncasecmp(penultimateline, "\r\n", 2) == 0))
			{
				stillReading = 0;

			}
		}
	}

	// agora tem de ler a requisição e determinar se é de GET

	strncmp(request, "GET", 3);
	//TODO: Parei Aqui 







}

int main(int argc, char *argv[]) {
	TSocket srvSock, cliSock;        /* server and client sockets */
	struct TArgs *args;              /* argument structure for thread */
	pthread_t threads[NTHREADS];
	int tid = 0;
	fd_set set;  /* file description set */
	int ret, i;
	char str[BUFSIZE];

	if (argc == 1) { ExitWithError("Usage: server <local port>"); }

	/* Create a passive-mode listener endpoint */  
	srvSock = CreateServer(atoi(argv[1]));

	printf("Server read!\n");
	/* Run forever */
	for (;;) { 
		/* Initialize the file descriptor set */
		FD_ZERO(&set);
		/* Include stdin into the file descriptor set */
		FD_SET(STDIN_FILENO, &set);
		/* Include srvSock into the file descriptor set */
		FD_SET(srvSock, &set);

		/* Select returns 1 if input available, -1 if error */
		ret = select (FD_SETSIZE, &set, NULL, NULL, NULL);
		if (ret<0) {
			 WriteError("select() failed"); 
			 break;
		}

		/* Read from stdin */
		if (FD_ISSET(STDIN_FILENO, &set)) {
			scanf("%99[^\n]%*c", str);
			if (strncasecmp(str, "FIM", 3) == 0) break;

		}

		/* Read from srvSock */
		if (FD_ISSET(srvSock, &set)) {
			if (tid == NTHREADS) { 
				WriteError("number of threads is over"); 
				break; 
			}
			
			/* Spawn off separate thread for each client */
			cliSock = AcceptConnection(srvSock);
			population++;

			/* Create separate memory for client argument */
			if ((args = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) { 
				WriteError("malloc() failed"); 
				break;
			}
			args->cliSock = cliSock;

			/* Create a new thread to handle the client requests */
			if (pthread_create(&threads[tid++], NULL, HandleRequest, (void *) args)) { 
				WriteError("pthread_create() failed"); 
				break;
			}
		}
	}
	
	printf("Server will wait for the active threads and terminate!\n");
	/* Wait for all threads to terminate */
	for(i=0; i<tid; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}
