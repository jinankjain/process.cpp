#include <bits/stdc++.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>

using namespace std;

class Process
{
    public:
        void waiti();
        void resume();
        void task();
        int getPid();
        static void signalHandler(int sig);
        //Process();
};


void Process::waiti()
{
    kill(getpid(), SIGSTOP);
}

void Process::resume()
{
    kill(getpid(), SIGCONT);    
}

// This task is user dependent can change accordingly
// Here I have added a task of sorting array using bubble sort

void Process::task()
{
    waiti();
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

struct ProcessTable
{
    Process p;
};

class ProcessTableList
{
    public:
        int addProcess();
        void removeProcess(pid_t pid);
};

int ProcessTableList::addProcess()
{
    pid_t child = fork();
    //int status=0;
    if(child == 0)
    {
        Process p;
        p.task(); 
        return child;
    }
    else if(child > 0)
    {
        //waitpid(-1, NULL, 0);
        return child;
        //system("ps -aux | grep a.out");
    }
    else
    {
        perror("Bad fork()"); exit(1);
    }
    
}

void ProcessTableList::removeProcess(pid_t pid)
{

}

int main()
{
    

    int a[10]; // Array storing list of pids

    ProcessTableList plist;
    for(int i=0;i<10;i++)
    {
        a[i] = plist.addProcess();
    }
    int i=0;
    for(i=0;i<10;i++)
    {
        cout<<a[i]<<endl;
    }

    // Wait all the child process to get finished
    for (i = 0; i < 10; ++i) {
        int status;
        while (-1 == waitpid(a[i], &status, 0));
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            cerr << "Process " << i << " (pid " << a[i] << ") failed" << endl;
            exit(1);
        }
    }

    return 0;
}