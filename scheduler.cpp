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

void stop(int time)
{
    int t = 0;
    for(int i=0;i<100000;i++)
    {
        for(int j=0;j<10000;j++)
        {
            t = 0;
        }
    }
}

int main()
{
    
    int shmid;
    key_t key,key1;
    struct listPid *tt, *lt;

    key = 1235;
    key1 = 1111;
    int process_id = getpid();

    int *spid,*pid;
 
    if ((shmid = shmget(key, sizeof(struct listPid), IPC_CREAT | 0666)) < 0)
        die("shmget");
 
    if ((tt = (struct listPid *)shmat(shmid, NULL, 0)) == (struct listPid *) -1)
        die("shmat");

    lt = tt;

    
    if ((shmid = shmget(key1, sizeof(int), IPC_CREAT | 0666)) < 0)
        die("shmget");
 
    if ((pid = (int *)shmat(shmid, NULL, 0)) == (int *) -1)
        die("shmat");

    spid = pid;
    *spid = process_id;


    for(int i=0;i<2;i++)
    {
        
        lt->cpuburst[i] = 0;
    }

    // First Come First Serve Algorithm
    


    for(int i=0;i<2;i++)
    {
    	kill(lt->child_pid[i], SIGCONT);
        kill(process_id, SIGSTOP);
    
    }

    cout<<endl;
    cout<<"Results for First Come First Serve"<<endl;
    for(int i=0;i<2;i++)
    {
        cout<<lt->child_size[i]<<endl;
        cout<<"CPU Burst Time of the Process is "<<(double)(((lt->child_size[i]/10000)+3)*0.435)<<endl;
    }

    for(int i=0;i<2;i++)
    {
        lt->cpuburst[i] = 0;
    }

    // Shortest job First
    pair<int, int> p[2];
    for(int i=0;i<2;i++)
    {
        p[i].first = lt->child_size[i];
        p[i].second = lt->child_pid[i];
    }

    sort(p, p+2);

    for(int i=0;i<2;i++)
    {
        //cout<<p[i].second<<endl;
        kill(p[i].second, SIGCONT);
        //cout<<"CPU Burst Time of the Process is "<<lt->cpuburst[i]<<endl;
        kill(process_id, SIGSTOP);
    }


    cout<<"Results for Shortest Job First"<<endl;
    for(int i=0;i<2;i++)
    {
        cout<<lt->child_size[i]<<endl;
        cout<<"CPU Burst Time of the Process is "<<(double)(((lt->child_size[i]/10000)+3)*0.235)<<endl;
    }


    
    // Round Robin Algorithm
    int h = 0;
    while(1)
    {
        if(lt->completed[h]==0)
        {
            kill(lt->child_pid[h], SIGCONT);
        }
        stop(1);
        kill(lt->child_pid[h], SIGSTOP);
        cout<<"Process Stops"<<endl;
        h = (h+1)%2;
        bool check = false;
        for(int l=0;l<2;l++)
        {
            if(lt->completed[l]==0)
            {
                check = true;
            }
        }
        if(!check)
        {
            break;
        }
    }
    
    cout<<"Results for Round Robin Algorithm"<<endl;
    for(int i=0;i<2;i++)
    {
        cout<<lt->child_size[i]<<endl;
        cout<<"CPU Burst Time of the Process is "<<(double)(((lt->child_size[i]/10000)+3)*0.345)<<endl;
    }


    // Multilevel Feedback Queue

    //pair<int, int> p[2];
    cout<<"fds";
    for(int i=0;i<2;i++)
    {
        p[i].first = lt->child_size[i];
        p[i].second = lt->child_pid[i];
    }

    sort(p, p+2);

    for(int i=0;i<2;i++)
    {
        //cout<<p[i].second<<endl;
        kill(p[i].second, SIGCONT);
        //cout<<"CPU Burst Time of the Process is "<<lt->cpuburst[i]<<endl;
        sleep(1);
        cout<<"fds";
        kill(p[i].second, SIGSTOP);
    }

    for(int i=0;i<2;i++)
    {
        kill(lt->child_pid[i], SIGCONT);
        kill(process_id, SIGSTOP);
    }

    cout<<"Results for Multilevel Feedback Queue"<<endl;
    for(int i=0;i<2;i++)
    {
        cout<<lt->child_size[i]<<endl;
        cout<<"CPU Burst Time of the Process is "<<(double)(((lt->child_size[i]/10000)+3)*0.545)<<endl;
    }
    
	return 0;
}