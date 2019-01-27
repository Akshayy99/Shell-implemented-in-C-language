#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <termios.h>
#include <sys/prctl.h>
#include <makefile.h>

void sigintHandler(int sig_num)
{
  printf("\n");
	signal(SIGINT, sigintHandler);
	fflush(stdout);
}

void sigtstpHandler(int sig_num)
{
  printf("\n");
  signal(SIGTSTP, sigtstpHandler);
}
void init_shell()
{
  clear();
  char* welcomestr = "\n                           ./+o+-\n                  yyyyy- -yyyyyy+\n               ://+//////-yyyyyyo\n           .++ .:/++++++/-.+sss/`\n         .:++o:  /++++++++/:--:/-\n        o:+o+:++.`..```.-/oo+++++/\n       .:+o:+o/.          `+sssoo+/\n  .++/+:+oo+o:`             /sssooo.\n /+++//+:`oo+o               /::--:.\n \\+/+o+++`o++o               ++////.\n  .++.o+++oo+:`             /dddhhh.\n       .+.o+oo:.          `oddhhhh+\n        \\+.++o+o``-````.:ohdhhhhh+\n         `:o+++ `ohhhhhhhhyo++os:\n           .o:`.syhhhhhhh/.oo++o`\n               /osyyyyyyo++ooo+++/\n                   ````` +oo+++o\\:    CShell\n                          `oo++.    Made by Akshay Kharbanda\n\n";
    printf(MAGENTA "%s" RESET, welcomestr);
  sleep(1.4);
  clear();

}



/* insert a new process in the background */
void insert(char *process, pid_t pid)
{
  bg *new = (bg*)malloc(sizeof(bg));
  strcpy(new->name,process);
  new->pid = new->pgid = pid;
  new->next = NULL;
  if(background==NULL)
    background=new;
  else
  {
    bg *temp=background;
    while(temp->next!=NULL)
      temp=temp->next;
    temp->next=new; 
  }
}

void delete(pid_t pid)
{
  if(background!=NULL)
  {
    bg *temp=background;
    if(background->pid == pid)
    {
      background = background->next;
      free(temp);
    } 
    else
    {
      bg *temp2;
      while(temp!=NULL && temp->pid!=pid)
      {
        temp2=temp;
        temp=temp->next;
      }
      if(temp!=NULL)
      {
        temp2->next=temp->next;
        free(temp);
      }
    
    }
  }
}


void bgResponse(int signal)
{
  pid_t pid;
  int status;
  pid_t processid;
  while((processid=waitpid(-1,&status,WNOHANG))>0)
  {
    if(processid!=-1 && processid!=0)
    {
      bg *temp=background;
      while(temp!=NULL && temp->pid!=processid)
        temp=temp->next;   
      if(WIFEXITED(status))
      {
        if(temp!=NULL)
        {
          fprintf(stderr,"%s with pid %d exited normally. Press ENTER to continue..\n",temp->name,processid);
          delete(processid); 
        }
      }
    }
  }
 }

void procSTOP(int signal) 
{
	int status=0;
	printf("\nProcess %d has been stopped and sent to bg\n", PID);
	pid_t pid = PID;
	insert(ARR[0], PID); 
	//printf("%d\n",pid);
	if(kill(pid, SIGSTOP) < 0)
		perror("Error in putting it the process to bg : ");
	waitpid(pid, &status, WUNTRACED);
	printf("\n");

	if(WIFSTOPPED(status))
		fprintf(stderr, "\n[%d]+ stopped %s\n", pid, ARR[0] );
	tcsetpgrp(shell, shell_pgid);
}

void exe(char **args)
{
   int te = -1;
   int status = 0, i;
   PID = fork();
   childPid = PID;
   bg *temp;
   int backg = 0;
   i = 0;
   ARR = args;
   signal(SIGTSTP, procSTOP);
   while(args[i] != NULL)
     i++;
   if(i != 1)
   {
      if(strcmp(args[i-1], "&") == 0)
      {
          backg = 1;
          args[i-1] = NULL;
      }
    }
    if (PID == 0)
    {
         execvp(*args, args);
         signal(SIGTSTP, procSTOP);
         perror(*args);
         exit(1);
    }
    else
    {
        if(backg)
        {
             printf("starting background job %d\n", PID);
             insert(args[0], PID);
             signal(SIGCHLD, bgResponse);
        }
        else
          while(wait(&status) != PID);
        if(status != 0)
            fprintf(stderr, "error: %s exited with status code %d\n", args[0], status);
    }
}

int exec(char **tokens, char *path, char *home)
{ 
  if(strcmp(tokens[0], "clear") == 0)
  {
    clear();
  }
  if(strcmp(tokens[0], "cd") == 0)
  {
    cd(tokens);
    return 1;
  }
  else if(strcmp(tokens[0], "pwd") == 0)
  {
    pwd();
    printf("\n");
    return 1;
  }
  else if(strcmp(tokens[0], "echo") == 0)
  {
      echo(tokens);
      return 1;
  }

  else if((strcmp(tokens[0], "quit") == 0) || (strcmp(tokens[0], "exit") == 0))
  {
    quit();
    return 1;
  }
  else if(strcmp(tokens[0], "ls") == 0)
  {
    if(tokens[1] == NULL)
    {
      ls(path, 0);
      return 1;
    }
    else if((strcmp(tokens[1], "-l") == 0) && tokens[2] == NULL)
    {
      ls_l(path, 0);
      return 1;
    }
    else if((strcmp(tokens[1], "-a") == 0) && tokens[2] == NULL)
    {
      ls(path, 1);
      return 1;
    }
    else if((strcmp(tokens[1], "-la") == 0) || (strcmp(tokens[1], "-al") == 0))
    {
      ls_l(path, 1);
      return 1;
    }
    else if(((strcmp(tokens[1], "-l") == 0) && (strcmp(tokens[2], "-a") == 0)) || ((strcmp(tokens[1], "-a") == 0) && (strcmp(tokens[2], "-l") == 0)))
    {
      ls_l(path, 1);
      return 1;
    }
    else
    {
      printf("%s: Command not found:\nFlag options:\n-l\n-a\n-la\n", *tokens);
      return 1;
    }
  }
  else if(strcmp(tokens[0], "pinfo") == 0)
  {
    pinfo(tokens, home);
    return 1;
  }
  else if(strcmp(tokens[0], "about") == 0)
  {
    about();
    return 1;
  }
  else if(strcmp(tokens[0], "remindme") == 0)
  {
    if(tokens[1] == NULL || tokens[2] == NULL)
    {
      fprintf(stderr, "remindme: incorrect syntax\n");
      return 1;
    }
    remindme(tokens);
    return 1;
  }
  else if(strcmp(tokens[0], "clock") == 0)
  {
    if(tokens[1] == NULL || tokens[2] == NULL || tokens[3] == NULL || tokens[4] == NULL)
    {
      fprintf(stderr, "clk: invalid syntax / format\n");
      return 1;
    }
    if(strcmp(tokens[1], "-t") != 0 || strcmp(tokens[3], "-n") != 0)
    {
      fprintf(stderr, "clk: invalid syntax / format\n");
        return 1;
    }
     
    clk(atoi(tokens[2]), atoi(tokens[4]));
    return 1;
  }
  else if(strcmp(tokens[0], "jobs") == 0)
  {
  	jobs();
  	return 1;
  }
  else if(strcmp(tokens[0], "kjob") == 0)
  {
  	kjob(tokens);
  	return 1;  
  }
  else if(strcmp(tokens[0], "overkill") == 0)
  {
    overkill();
    return 1;
  }
  else if(strcmp(tokens[0], "fg") == 0)
  {
    fg(tokens);
    return 1;
  }
  else if(strcmp(tokens[0], "bg") == 0)
  {
    Bg(tokens);
    return 1;
  }
  else if(strcmp(tokens[0], "setenv") == 0)
  {
  	set_env(tokens);
  	return 1;
  }
  else if(strcmp(tokens[0], "unsetenv") == 0)
  {
  	unset_env(tokens);
  	return 1;
  }
  else
  {
    exe(tokens); 
    return 1;
  }
  return 0;
}


void redir_to_output(char *token, char* command, char *path, char *home)
{
    int status = 0, pid, r;
    char **k, *filename;
    filename = strtok(token, " \t\a\n");
    FILE *test = fopen(filename, "w");
    int fd = fileno(test); 
    k = getArgs(command, " \t\a\n");
    // pid = fork(); 
    if((pid = fork()) == 0) 
    { 
      dup2(fd, 1);
      r = exec(k, path, home); 
      exit(0);
    }
    else
      while(wait(&status) != pid);

}

void redir_to_input(char *token, char *command, char *path, char *home)
{
    int status=0, pid, r;
    char **k, *filename;
    filename = strtok(token, " \t\a\n");
    FILE *test = fopen(filename, "r");
    int fd = fileno(test); 
    k = getArgs(command, " \t\a\n");
    pid = fork(); 
    if(pid == 0) 
    { 
      dup2(fd, 0);
      r = exec(k, path, home); 
      exit(0);
    }
    else
      while(wait(&status) != pid);

}

void redir_append(char *token, char *command, char *path, char *home)
{
    int status=0, pid, r;
    char **k, *filename;
    filename = strtok(token, " \t\a\n");
    FILE *test = fopen(filename, "a");
    int fd = fileno(test); 
    k = getArgs(command, " \t\a\n");
    pid = fork(); 
    if(pid == 0) 
    { 
      dup2(fd, 1);
      r = exec(k, path, home); 
      exit(0);
    }
    else
      while(wait(&status)!=pid);

}

void redir_ior(char *token, char *command, char *token2, char *path, char *home)
{
    int status=0, pid, r;
    char **k,*filename,*file2;
    filename = strtok(token, " \t\a\n");
    file2 = strtok(token2, " \t\a\n");
    FILE *test = fopen(filename, "r");
    FILE *test1 = fopen(file2, "w");
    int fd = fileno(test); 
    int fd1 = fileno(test1);
    k = getArgs(command, " \t\a\n");
    pid = fork(); 
    if(pid == 0) 
    { 
      dup2(fd, 0);
      dup2(fd1, 1);
      r = exec(k, path, home);
      exit(0);
    }
    else
      while(wait(&status) != pid);
}

void redir(int flag, char **l, char *path, char *home)
{
    if(flag == 0)
        redir_to_output(l[1], l[0], path, home);

    else if(flag == 1)
        redir_to_input(l[1], l[0], path, home);

    else if(flag == 2)
        redir_append(l[1], l[0], path, home);

    else if(flag == 3)
        redir_ior(l[1], l[0], l[2], path, home);
}

int piping(char **args, char *path, char *home) 
{
	int status = 0, pipe_cnt = -1, flag = -1, len;
	pid_t pid, wpid;
	char **k, **l;
	int i, j, total, r;
	for(i=0 ; args[i] != NULL ; i++)
		pipe_cnt++;

	int pipes[2 * pipe_cnt];
	for(i = 0 ; i < (2 * pipe_cnt) ; i += 2) 
	{
		pipe(pipes + i);
	}
    for(i = -2 ; i < (2 * pipe_cnt) ; i += 2) 
    {
        if((pid = fork()) == 0) 
        {
            if((i + 3) < (2 * pipe_cnt))
                dup2(pipes[i + 3], 1);
            if(i != -2)
                dup2(pipes[i], 0);
            for(j = 0 ; j < (2 * pipe_cnt) ; j++)
                close(pipes[j]);
            r = i/2 + 1;
            len = check1(args[(i+2)/2]);
            if(len == 2)
            {
                flag = check(args[(i+2)/2]);
                if(flag != 2)
                    flag = 3;
            }
            else
                flag = check(args[(i+2)/2]);

            l = getArgs(args[(i+2)/2], "><");
            if(l[1] != '\0')
            { 
                redir(flag, l, path, home);
                r = 1;
            }
            else
            {
                k = getArgs	(args[(i+2)/2], " \t\a\n");
                r = exec(k, path, home);
            }
            exit(EXIT_FAILURE);
		}
    }
    for(i = 0 ; i < (2 * pipe_cnt) ; i++)
        close(pipes[i]);

    while ((wpid = wait(&status)) > 0);

    return 0;
}


int main()
{
  signal(SIGINT, sigintHandler);
  signal(SIGTSTP, sigtstpHandler);
  char *in_com;
  char cwd[size];
  char path[size];
  char user[size], hostname[size], displayCWD[size];
  char HOME[size];
  int bgcount;
  int cnt;
  int res, r, c, flag, len, length;
  int status, i = 0;
  int bgflag, j;
  init_shell();
  // mainPid = getpid();
  char *input, **args, **comm, **k,**l,*inp,**p;
  do 
  {
    if(getcwd(path, sizeof(path)) != NULL);
    else
      perror("getcwd() error");
    getcwd(cwd, sizeof(cwd));
      if(i == 0)
      {
        strcpy(HOME, cwd);
        i = 1;
        bgcount = 0;
      }
      cuserid(user);
      hostname[size-1] = '\0';
      gethostname(hostname, sizeof(hostname)-1);
      if(strcmp(cwd, HOME)==0)
        strcpy(displayCWD, "~");
      else 
      {
        strcpy(displayCWD, cwd);
        convertCWD(displayCWD, HOME);
      }
    printf(MAGENTA "<%s" RESET, user);
    printf(YELLOW "@" RESET);
    printf(CYAN "%s:", hostname);
    printf(BLUE "%s> " RED, displayCWD);
    printf("%c%c%c"RESET, '\xE2', '\x99', '\xA5'); 
    printf(" ");
    /* get user input */
    input = readline();
    /* parse the input to get the semicolon separated arguments */
    comm = getArgs(input, ";");
    childPid = -1;
    for(i = 0; comm[i] != NULL; i++) 
    {
        len=check1(comm[i]);

      if(len == 2)
        {
          flag = check(comm[i]);
          if(flag != 2)
            flag = 3;
        }
        else
          flag = check(comm[i]);
        if(comm[i][0] == '\0')
          continue; 
        p=getArgs(comm[i], "|");
        if(p[1] == '\0')
        { 
          l = getArgs(comm[i], "><");
          if(l[1] == '\0')
          {
            k = getArgs(comm[i], " \t\r\n\a");
            r = exec(k, path, HOME);
            free(k);
          }
          else
          {
            redir(flag, l, path, HOME);
            r = 1;
          }
        }
        else
        {
          r = piping(p, path, HOME);
          r = 1;
        }
    }
    free(input);

	} while(r);


  return 0;
}