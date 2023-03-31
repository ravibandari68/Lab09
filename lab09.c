
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid,ppid;
int status;
int i=0;

static void sig_int(int signo)
{
	signal(signo, SIG_IGN);
	printf("\nReceived SIGINT signal - %d",signo);
	kill(pid,SIGINT);
	waitpid(pid, &status, WNOHANG);
	if (WIFEXITED(status))
	{
		printf("\nChild process exited with status = %d.\n", WEXITSTATUS(status));
	}
	else
	{
		printf("\nChild process did not terminate normally!\n");
	}
	fflush(stdout);
	signal(signo, sig_int);
}

static void sig_stp(int signo)
{
	signal(signo, SIG_IGN);
	printf("\nReceived SIGTSTP signal - %d",signo);
	kill(pid,SIGTSTP);
	waitpid(pid, &status, WNOHANG);
	if (WIFEXITED(status))
	{
		printf("\nChild process exited with status = %d.\n", WEXITSTATUS(status));
	}
	else
	{
		printf("\nChild process did not terminate normally!\n");
	}
	fflush(stdout);
	signal(signo, sig_stp);
}

static void sig_quit(int signo)
{
	signal(signo, SIG_IGN);
	printf("\nReceived SIGQUIT signal - %d",signo);
	i=1;
	kill(ppid,SIGCONT);
}

int main(int argc, char **argv)
{
    if (argc < 2)
	{
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(-1);
    }
	
	ppid = getpid();
	pid = fork();
	
    if (pid == 0)		// this is the child process
	{
        execvp(argv[1], &argv[1]);
        printf("If you see this statement then execl failed.\n");
		perror("execvp");
		exit(-1);
    }
	else if (pid > 0)	// this is the parent process
	{		
        if (signal(SIGINT, sig_int) == SIG_ERR)		//Control-C signal
		{
			printf("Unable to catch SIGINT\n");
		}
		if (signal(SIGTSTP, sig_stp) == SIG_ERR)	//Control-Z signal
		{
			printf("Unable to catch SIGTSTP\n");
		}
		if (signal(SIGQUIT, sig_quit) == SIG_ERR)	//Control-\ signal
		{
			printf("Unable to catch SIGQUIT\n");
		}

		printf("Wait for the child process to terminate\n");
		
		while(i!=1)
		{
			pause();
		}
    }
	else
	{
        perror("fork");
        exit(EXIT_FAILURE);
    }
	
	printf("\n[%ld]: Exiting program .....\n", (long)getpid());
    
    return 0;
}
