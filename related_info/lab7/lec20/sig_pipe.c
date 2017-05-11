/******************************
signal有三种处理方式，捕获，忽略，屏蔽。实现信号的捕获需要实现对应信号的handler函数，并在程序中用signal函数
注册这个handler，当收到对应信号时，就可以实现捕获和处理；实现信号的忽略需要调用signal函数，并在第二个参数处
放上SIG_IGN，当收到对应信号时，信号会被丢弃；实现信号的屏蔽需要首先定义屏蔽集合，并用sigprocmask函数设置屏蔽，
当收到对应信号时，该信号会加入等待队列被挂起，直到用sigprocmask函数解除屏蔽，程序会继续处理对应信号。

pipe的实现可以分为命名的pipe和非命名的pipe两种方式，如果用fork创建子进程，可以通过非命名pipe方式实现，调用
pipe函数即可创建一个pipe，用write和read函数即可向pipe中写或从pipe中读内容。

下面的程序分别在三个子进程中用直接捕获，忽略和屏蔽的方式实现了对signal的处理，并通过pipe从子进程向父进程发送
字符串，运行此程序的一个示例输出是：
parent_pid = 32178
In child process (pid = 32179)
Child 32179 sending string
Child 32179 registered SIGUSR1 handler
In child process (pid = 32180)
Child 32180 sending string
Parent received string: Hello, world!
Parent send SIGUSR1 to child 32179
Child 32180 ignored SIG_INT
Child 32180 sleeping
Parent received string: Hello, world!
Parent send SIGINT to child 32180
Caught signal 30
In child process (pid = 32181)
Child 32181 sleeping
Exit status of 32179 was 0
Child 32180 sleeping
Child 32181 sleeping
Child 32180 sleeping
Child 32181 unblocked SIGUSR1
Caught signal 30
Exit status of 32181 was 0
Exit status of 32180 was 0
******************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

// Define the function to be called when SIGUSR1 signal is sent to process
void
sigusr1_handler(int signum)
{
   printf("Caught signal %d\n",signum);

   // Terminate program
   exit(0);
}


int main(void) {
    pid_t cpid, wpid, child_pid[3];
    int   status = 0;
    int   i, j;

    int     fd[3][2], nbytes;
    char    string[] = "Hello, world!\n";
    char    readbuffer[80];

    for (i = 0; i < 3; i++)
        pipe(fd[i]);

    printf("parent_pid = %d\n", getpid());

    for (i = 0; i < 3; i++) {
        if ((cpid = fork()) == -1) {
            perror("Unable to fork");
            exit(1);
        }

        if (cpid == 0) {
            printf("In child process (pid = %d)\n", getpid());
            /* Child process closes up input side of pipe */
            close(fd[i][0]);
            /* Send "string" through the output side of pipe */
            printf("Child %d sending string\n", getpid());
            write(fd[i][1], string, (strlen(string)+1));

            if (i == 0) {
                printf("Child %d registered SIGUSR1 handler\n", getpid());
                signal(SIGUSR1, sigusr1_handler);
                while(1) {
                    sleep(1);   // wait for signal
                }
            } else if (i == 1) {
                signal(SIGINT, SIG_IGN);    // ignore SIG_INT
                printf("Child %d ignored SIG_INT\n", getpid());
                for (j = 0; j < 3; j++) {
                    printf("Child %d sleeping\n", getpid());
                    sleep(1);
                }
                exit(0);
            } else {
                printf("Child %d registered SIGUSR1 handler\n", getpid());
                signal(SIGUSR1, sigusr1_handler);
                printf("Child %d blocked SIGUSR1\n", getpid());
                sigset_t set;
                sigemptyset(&set);
                sigaddset(&set, SIGUSR1);
                sigprocmask(SIG_BLOCK, &set, NULL);
                for (j = 0; j < 2; j++) {
                    printf("Child %d sleeping\n", getpid());
                    sleep(1);
                }
                printf("Child %d unblocked SIGUSR1\n", getpid());
                sigprocmask(SIG_UNBLOCK, &set, NULL);
                exit(0);
            }
        } else {
            child_pid[i] = cpid;
        }
    }

    for (i = 0; i < 3; i++) {
        /* Parent process closes up output side of pipe */
        close(fd[i][1]);
        /* Read in a string from the pipe */
        nbytes = read(fd[i][0], readbuffer, sizeof(readbuffer));
        printf("Parent received string: %s", readbuffer);

        if (i == 0) {
            printf("Parent send SIGUSR1 to child %d\n", child_pid[0]);
            kill(child_pid[0], SIGUSR1);    // send SIGUSR1 signal
        } else if (i == 1) {
            printf("Parent send SIGINT to child %d\n", child_pid[1]);
            kill(child_pid[1], SIGINT);     // send SIGINT signal
        } else {
            printf("Parent send SIGUSR1 to child %d\n", child_pid[2]);
            kill(child_pid[2], SIGUSR1);
        }
    }

    while ((wpid = wait(&status)) > 0) {
        printf("Exit status of %d was %d\n", (int)wpid, status);
    }
    return 0;
}
