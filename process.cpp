#include <bits/stdc++.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

class Process
{
	public:
		void wait();
		void resume();
		void task();
		int getPid();
		sem_t mutex;
		static void signalHandler(int sig);
		Process();
};

Process::Process(void)
{
	if (signal(SIGUSR1, signalHandler) == SIG_ERR)
        printf("Unable to create handler for SIGUSR1\n");

    if (signal(SIGUSR2, signalHandler) == SIG_ERR)
        printf("Unable to create handler for SIGUSR2\n");

    // intialize mutex lock with 0 i.e Process started in wait state
    sem_init(&mutex,0,0);
}

void Process::wait()
{
	kill(getpid(), SIGUSR1);
}

void Process::resume()
{
	kill(getpid(), SIGUSR2);	
}

void Process::task()
{

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
	}
	else if(sig==SIGUSR2)
	{
		// issue a signal mutex
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