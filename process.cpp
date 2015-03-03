#include <bits/stdc++.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

using namespace std;

class Process
{
	public:
		void wait();
		void resume();
		void task();
		int getPid();
		static void signalHandler(int sig);
		Process();
};

Process::Process(void)
{
	if (signal(SIGUSR1, signalHandler) == SIG_ERR)
		printf("Unable to create handler for SIGUSR1\n");

	if (signal(SIGUSR2, signalHandler) == SIG_ERR)
		printf("Unable to create handler for SIGUSR2\n");

}

void Process::wait()
{
	kill(getpid(), SIGUSR1);
}

void Process::resume()
{
	kill(getpid(), SIGUSR2);	
}

// This task is user dependent can change accordingly
// Here I have added a task of sorting array using bubble sort

void Process::task()
{
	int t = (rand()%10+1) * 100; // random number between 100 to 1000
	int a[t];
	for(int i=0;i<t;i++)
	{
		a[i] = (rand()%10);
	}
	int i,j;
    for(i=0;i<t;i++)
    {
        for(j=i+1;j<t;j++)
        {
            if(a[j]<a[i])
            {
                swap(a[i],a[j]);
            }
        }
    }
}

int Process::getPid()
{
	return getpid();
}

void Process::signalHandler(int sig)
{
	if(sig==SIGUSR1)
	{
		// issue a wait mutex
		kill(getpid(), SIGSTOP);
	}
	else if(sig==SIGUSR2)
	{
		// issue a signal mutex
		kill(getpid(), SIGCONT);
	}
	else
	{
		printf("Unknown signal\n");
	}
}

int main()
{
	return 0;
}