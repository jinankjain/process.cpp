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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdint.h>
#include <random>
#include <time.h>

using namespace std;

void die(char *s)
{
    perror(s);
    exit(1);
}

struct listPid
{
    int child_pid[2];
    int child_size[2];
    int completed[2];
    int waitingTime[2];
    double cpuburst[2];
};

class Process
{
    public:
        void waiti();
        void resume();
        void task();
        int getPid();
        static void signalHandler(int sig);
        int array_size;
        Process();
};

Process::Process(void)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    array_size = (dis(gen))*10000;
}

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

    sleep(3);
    
    struct timespec st,en;
    double runtime;


    struct listPid *tt, *lt;

    int shmid;
    key_t key1, key;
    key1 = 1111;
    key = 1235;

    if ((shmid = shmget(key, sizeof(struct listPid), IPC_CREAT | 0666)) < 0)
        die("shmget");
 
    if ((tt = (struct listPid *)shmat(shmid, NULL, 0)) == (struct listPid *) -1)
        die("shmat");

    lt = tt;


    int *spid,*pid;
    if ((shmid = shmget(key1, sizeof(int), IPC_CREAT | 0666)) < 0)
        die("shmget");
 
    if ((pid = (int *)shmat(shmid, NULL, 0)) == (int *) -1)
        die("shmat");

    spid = pid;

    cout<<"Process with "<<getPid()<<" begins\n";

    clock_gettime(CLOCK_MONOTONIC,&st);

    int t = array_size; // random number between 100 to 1000
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

    clock_gettime(CLOCK_MONOTONIC,&en);
    runtime = en.tv_sec - st.tv_sec+(en.tv_nsec-st.tv_nsec)/(1e9);

    for(int i=0;i<2;i++)
    {
        if(lt->child_pid[i] == getPid())
        {
            lt->completed[i] = 1;
            lt->cpuburst[i] = runtime;
        }
    }
    cout<<"Completed !!! "<<getpid();
    kill(*spid, SIGCONT);

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
        int addProcess(int count);
        void removeProcess(pid_t pid);
};

int ProcessTableList::addProcess(int count)
{
    pid_t child = fork();
    //int status=0;
    if(child == 0)
    {
        Process p;
        sem_t * sem = sem_open ( "semname" , O_CREAT | O_EXCL , S_IRUSR | S_IWUSR , 0) ;
        if ( sem != SEM_FAILED )
        {
            //printf ( " created new semaphore !\n " ) ;
        }
        else if ( errno == EEXIST ) 
        {
            //printf ( " semaphore appears to exist already !\n " );
            sem = sem_open ( "semname" , 0) ;
        }
        
        int shmidi;
        int shmid;
        key_t key;

        struct listPid *tt, *lt;

        key = 1235;
 
        if ((shmid = shmget(key, sizeof(struct listPid), IPC_CREAT | 0666)) < 0)
            die("shmget");
 
        if ((tt = (struct listPid *)shmat(shmid, NULL, 0)) == (struct listPid *) -1)
            die("shmat");

        lt = tt;
        lt->child_pid[count] = p.getPid();
        lt->child_size[count] = p.array_size;
        lt->completed[count] = 0;
        sem_post(sem);
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
    
    int a[2]; // Array storing list of pids

    int main_pid = getpid();
    
    int shmid;
    key_t counter_key;

    ProcessTableList plist;
    
    // First Come First Scheduling 
    for(int i=0;i<2;i++)
    {
        a[i] = plist.addProcess(i);
    }
    if(main_pid == getpid()){
        int i=0;
        for(i=0;i<2;i++)
        {
            cout<<"Process Waiting to be scheduled with pid: "<<a[i]<<endl;
        }
     
        // Wait all the child process to get finished
        for (i = 0; i < 2; ++i) {
            int status;
            while (-1 == waitpid(a[i], &status, 0));
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                cerr << "Process " << i << " (pid " << a[i] << ") failed" << endl;
                exit(1);
            }
        }
    }


    
    // Shortest Job First
    for(int i=0;i<2;i++)
    {
        a[i] = plist.addProcess(i);
    }
    if(main_pid == getpid()){
        int i=0;
        for(i=0;i<2;i++)
        {
            cout<<"Process Waiting to be scheduled with pid: "<<a[i]<<endl;
        }


        // Wait all the child process to get finished
        for (i = 0; i < 2; ++i) {
            int status;
            while (-1 == waitpid(a[i], &status, 0));
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                cerr << "Process " << i << " (pid " << a[i] << ") failed" << endl;
                exit(1);
            }
        }
    }



    // Round Robin Scheduling
    for(int i=0;i<2;i++)
    {
        a[i] = plist.addProcess(i);
    }
    if(main_pid == getpid()){
        int i=0;
        for(i=0;i<2;i++)
        {
            cout<<"Process Waiting to be scheduled with pid: "<<a[i]<<endl;
        }


        // Wait all the child process to get finished
        for (i = 0; i < 2; ++i) {
            int status;
            while (-1 == waitpid(a[i], &status, 0));
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                cerr << "Process " << i << " (pid " << a[i] << ") failed" << endl;
                exit(1);
            }
        }
    }

    // Multilevel Feedback Queue
    for(int i=0;i<2;i++)
    {
        a[i] = plist.addProcess(i);
    }
    if(main_pid == getpid()){
        int i=0;
        for(i=0;i<2;i++)
        {
            cout<<"Process Waiting to be scheduled with pid: "<<a[i]<<endl;
        }


        // Wait all the child process to get finished
        for (i = 0; i < 2; ++i) {
            int status;
            while (-1 == waitpid(a[i], &status, 0));
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                cerr << "Process " << i << " (pid " << a[i] << ") failed" << endl;
                exit(1);
            }
        }
    }

    return 0;
}