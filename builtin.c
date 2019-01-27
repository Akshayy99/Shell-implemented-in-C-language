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

/* about this shell*/
void about()
{
    // Source - http://ascii.co.uk/art/seashell
    char* descr = "           _.-''|''-._\n        .-'     |     `-.\n      .'\\       |       /`.\n    .'   \\      |      /   `.        Cshell.\n    \\     \\     |     /     /        Made by @Akshayy99\n     `\\    \\    |    /    /'\n       `\\   \\   |   /   /'\n         `\\  \\  |  /  /'\n        _.-`\\ \\ | / /'-._ \n       {_____`\\\\|//'_____}\n               `-'\n\n";

    printf(MAGENTA "%s" RESET,descr);
}

/* quit the shell */
void quit()
{
	printf(RED "EXITING SHELL.. Bye\n" RESET);
	exit(0);
}

/*print the present working directory */
void pwd()
{
	char cwd[size];
  	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf(YELLOW "%s\n" RESET, cwd );
	else
        perror("getcwd() error");
}

void echo(char **args)
{
  int i=1, j;
  while(args[i] != NULL)
  {
	for(j=0;j<strlen(args[i]);j++)
	{
	  if((args[i][j] != '\"' && args[i][j] != '\''))
		printf(RED "%c", args[i][j]);
	}
	printf(" ");
	i++;
  }
  printf("\n" RESET);
}

//function to change directory
void cd(char **tokens)
{
	char cwd[200]={0},cd[200];
	char *h="/home/";   
	int i=0, err;
	while(tokens[i]!=NULL)
    	i++;
	if(i == 1)
	{
		strcat(cwd, h);
		err = chdir(cwd);
	}
	// else if (tokens[1] == '~' || tokens[1] == '~/')
	else if(strcmp(tokens[1], "~")==0 || strcmp(tokens[1], "~/")==0)
	{
		strcat(cwd, h);
		err = chdir(cwd);
	}
	else if(tokens[1][0]=='.')
  	{
	    if(tokens[1][1]=='.')
	    {
	      int i,j;
	      getcwd(cwd,sizeof(cwd));
	      for(i=strlen(cwd)-1;i>=0;i--)
	      {
	        if(cwd[i]=='/')
	          break;
	      }
	      for(j=0;j<i;j++)
	      {
	        cd[j]=cwd[j];
	      }
	      strcat(cd,tokens[1]+2);
	      err=chdir(cd);
	      if(err!=0)
	        fprintf(stderr,"No directory present!\n");
	    }
	    else
	    {
	      getcwd(cwd,sizeof(cwd));
	      strcat(cwd,"/");
	      strcat(cwd,tokens[1]);
	      err=chdir(cwd);
	      if(err!=0)
	        fprintf(stderr,"No directory present!\n");
	    }
	  }
	  else if(tokens[1][0] != '/')
	  {
	      getcwd(cwd,sizeof(cwd));
	      strcat(cwd,"/");
	      strcat(cwd,tokens[1]);
	      err=chdir(cwd);
	      if(err!=0)
	        fprintf(stderr,"No directory present!\n");
	  }
	  else
	  {
	    err=chdir(tokens[1]);
	    if(err!=0)
	      fprintf(stderr,"No directory present!\n");
	  }
}

//self defined function to print the process related info of the shell program or program with a given pid
void pinfo(char ** params, char * basePath)
{
    // Create path string: /proc/pid
    char procPath[1000];
    strcpy(procPath, "/proc/");

    // If pid given, use that, else get info for shell process
    if(params[1]) strcat(procPath, params[1]);
    else strcat(procPath, "self");

    // read process id and status
    // Create stat path string
    char statPath[100];
    strcpy(statPath, procPath); strcat(statPath, "/stat");

    int errno = 0;
    FILE * stat = fopen(statPath, "r");
    if(errno) {
        fprintf(stderr, "Error reading %s: %s\n", statPath, strerror(errno));
        return ;
    }

    int pid; 
    char status; 
    char name[20];
    fscanf(stat, "%d", &pid); 
    fscanf(stat, "%s", name); 
    fscanf(stat, " %c", &status);

    fprintf(stdout, "pid -- %d\n", pid);
    fprintf(stdout, "Process Status -- %c\n", status);
    fclose(stat);
    
    // read number of virtual memory pages
    errno = 0;
    strcpy(statPath, procPath); 
    strcat(statPath, "/statm");
    FILE * mem = fopen(statPath, "r");
    if(errno)     {
	    fprintf(stderr, "Error reading %s: %s\n", statPath, strerror(errno));
        return ;
    }

    int memSize; 
    fscanf(mem, "%d", &memSize);
    fprintf(stdout, "Memory -- %d\n", memSize);
    fclose(mem);
    
    // get path to executable
    char exePath[1000];
    strcpy(statPath, procPath); 
    strcat(statPath, "/exe");
    errno = 0;
    // WARNING: READLINK DOES NOT ADD NULL TERMINATING BYTE
    readlink(statPath, exePath, sizeof(exePath));
    if(errno) {
        fprintf(stderr, "Error reading symbolic link %s: %s\n", statPath, strerror(errno));
        return;
    }

    // Substtute ~ for homepath
    int sameChars = 0, baseL = strlen(basePath);
    for(sameChars = 0; sameChars < baseL; sameChars++)
        if(basePath[sameChars] != exePath[sameChars]) break;;
    
    char relPath[1000];
    if(sameChars == baseL) {// First part of path is common
        relPath[0] = '~'; relPath[1] = '\0';
        strcat(relPath, (const char *)&exePath[baseL]);
    }
    else {
        strcpy(relPath, exePath);
        relPath[strlen(exePath)] = '\0';
    }
    
    // Clear exePath
    int i = 0;
    while(exePath[i]) exePath[i++] = '\0';
    
    fprintf(stdout, "Executable Path -- %s\n", relPath);
}

void print_perms(mode_t st) {
    char perms[11];
    if(st && S_ISREG(st)) perms[0]='-';
    else if(st && S_ISDIR(st)) perms[0]='d';
    else if(st && S_ISFIFO(st)) perms[0]='|';
    else if(st && S_ISSOCK(st)) perms[0]='s';
    else if(st && S_ISCHR(st)) perms[0]='c';
    else if(st && S_ISBLK(st)) perms[0]='b';
    else perms[0]='l';  // S_ISLNK
    perms[1] = (st & S_IRUSR) ? 'r':'-';
    perms[2] = (st & S_IWUSR) ? 'w':'-';
    perms[3] = (st & S_IXUSR) ? 'x':'-';
    perms[4] = (st & S_IRGRP) ? 'r':'-';
    perms[5] = (st & S_IWGRP) ? 'w':'-';
    perms[6] = (st & S_IXGRP) ? 'x':'-';
    perms[7] = (st & S_IROTH) ? 'r':'-';
    perms[8] = (st & S_IWOTH) ? 'w':'-';
    perms[9] = (st & S_IXOTH) ? 'x':'-';
    perms[10] = '\0';
    printf(YELLOW "%s" RESET, perms);
}

void ls(char path[], int flag) {
    DIR * dir; 
    struct dirent * file;
    dir = opendir(path);
    while(file=readdir(dir)) {
        if(file->d_name[0] != '.' || flag) {
            printf(YELLOW "%s   ", file->d_name);
        }
    }
    printf("\n" RESET);
    free(file);
    free(dir);
}

void ls_l(char path[], int flag) {
    DIR * dir; 
    DIR * dir2; 
    struct dirent * file; 
    struct dirent * file2; 
    struct stat sbuf;
    struct stat sbuf2;
    char buf[128];
    struct passwd pwent, *pwentp;
    struct group grp, * grpt;
    char datestring[256];
    char path2[size];
    struct tm time;
    strcpy(path2, path);
    dir = opendir(path);
    dir2 = opendir(path2);
    long long int sum_size = 0;
    while(file2=readdir(dir2)) 
    {
    	if(flag) 
    	{
	        stat(file2->d_name, &sbuf2);
    		sum_size += (int)sbuf2.st_size;   	
    	}
	    else if(!flag)
	    {
	        stat(file2->d_name, &sbuf2);
	    	if(file2->d_name[0] != '.')	sum_size += (int)sbuf2.st_size;
    	}
	}
	printf("total %lld\n", sum_size);
    while(file=readdir(dir)) 
    {
    	if(flag)
    	{	
    		// printf("total %lld\n", sum_size);
	        stat(file->d_name, &sbuf);
	        print_perms(sbuf.st_mode);
	        printf(" %3d", (int)sbuf.st_nlink);
	        if (!getpwuid_r(sbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
	            printf(RED " %8s" RESET, pwent.pw_name);
	        else
	            printf(" %d", sbuf.st_uid);

	        if (!getgrgid_r (sbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
	            printf(RED " %8s" RESET, grp.gr_name);
	        else
	            printf(" %d", sbuf.st_gid);
	        printf(MAGENTA " %8d" RESET, (int)sbuf.st_size);
	        
	        localtime_r(&sbuf.st_mtime, &time);
	        /* Get localized date string. */
	        strftime(datestring, sizeof(datestring), "%b %d %R", &time);

	        printf(GREEN " %s " RESET, datestring);
	        printf("%s\n", file->d_name);
	    }
	    else if(!flag)
	    {
	    	if(file->d_name[0] != '.')
	    	{
	    		// printf("total %lld\n", sum_size);
	    		stat(file->d_name, &sbuf);
		        print_perms(sbuf.st_mode);
		        printf(" %3d", (int)sbuf.st_nlink);
		        if (!getpwuid_r(sbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
		            printf(RED " %8s" RESET, pwent.pw_name);
		        else
		            printf(" %d", sbuf.st_uid);

		        if (!getgrgid_r (sbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
		            printf(RED " %8s" RESET, grp.gr_name);
		        else
		            printf(" %d", sbuf.st_gid);
		        printf(MAGENTA " %8d" RESET, (int)sbuf.st_size);
		        
		        localtime_r(&sbuf.st_mtime, &time);
		        /* Get localized date string. */
		        strftime(datestring, sizeof(datestring), "%b %d %R", &time);

		        printf(GREEN " %s " RESET, datestring);
		        printf("%s\n", file->d_name);
	    	}
    	}
	}
}

void convertCWD(char *cwd, char *HOME)
{
  int i, flag=0, len = strlen(cwd), j;
  for(i=0; HOME[i]!='\0'; i++) {
    if(cwd[i]!=HOME[i]){
      flag = 1;
      break;
    }
  }
  if(flag==1){
    return;
  } else {
    char newCWD[len-i+3];
    newCWD[0]='~';
    for(j=1; i<len; i++, j++){
      newCWD[j] = cwd[i];
    }
    strcpy(cwd, newCWD);
  }
}

void remindme(char **msg)
{
  pid_t pid;
  pid = fork();
  int i = 2;
  if(pid == 0)
  {
    sleep(atoi(msg[1]));
    while(msg[i] != NULL)
    {
      printf(MAGENTA "%s " RESET, msg[i]);
      i++;
    }
    exit(1);
  }
  if(pid < 0)
  {
    fprintf(stderr, "Forking error\n");
  }
}

void clk(int time, int dur)
{
  int n = dur/time;
  char path[100];
  char buffer[size];
  char buffer1[size];
  long length;
  strcpy(path, "/proc/driver/rtc");
  int errno = 0;
  while(n--)
  {
    FILE * f = fopen(path, "r");
    if(errno) 
    {
        fprintf(stderr, "Error reading %s: %s\n", path, strerror(errno));
        return ;
    }
    if(f)
    {
      fscanf(f, "%*s %*s %s", buffer);
      fscanf(f, "%*s %*s %s", buffer1);
      printf(GREEN "%s ", buffer1);
      printf("%s \n" RESET, buffer);
      sleep(time);
    }
    fclose(f);
  }
}

void set_env(char **args)
{
	if(args[1] == NULL)
	{
		printf("setenv:\t--usage setenv var [value]\n");
		return;
	}

	if(args[2] == NULL)
	{
		setenv(args[1], "", 1);
		return;
	}
	if(getenv(args[1]) == NULL)
		setenv(args[1],"",1);

	else 
	setenv(args[1],args[2],1);
}

void unset_env(char **args)
{
	if(args[1] == NULL)
	{
		printf("unsetenv:\t--usage unsetenv var \n");
		return;
	}
	if(getenv(args[1]) == NULL)
		printf("%s: No such enviorment variable\n", args[1]);

	else if(unsetenv(args[1]) != 0)
		printf("Unidentified error\n");
}

void jobs()
{
  int sno;
  bg *temp = background;
  sno = 1;
  while(temp != NULL)
  {
    int Pid = temp->pid;
    char statPath[100];
    sprintf(statPath, "/proc/%d/stat", Pid);
    int errno = 0;
    char status; 
    FILE * stat = fopen(statPath, "r");
    if(errno) {
        fprintf(stderr, "Error reading %s: %s\n", statPath, strerror(errno));
        return ;
    }
    int temp213;
    char * ch;
    fscanf(stat, "%d %s %c",&temp213, &ch, &status);
    char status1[100];
    if (status == 'T' )
      strcpy(status1, "Stopped");
    else
      strcpy(status1, "Running");
    fprintf(stdout, "[%d] %s %s [%d]\n", sno, status1, temp->name, temp->pid );
    temp = temp->next; 
    sno++;
  }
}

void kjob(char **tokens)
{ 
  int i = 0;
  while(tokens[i] != NULL) i++;
  if(i > 3)
    printf(RED "Too many arguments\nUsage: kjob <jobNumber> <signalNumber>\n" RESET);
  else if(i < 3)
    printf(RED "Too few arguments\nUsage: kjob <jobNumber> <signalNumber>\n" RESET);
  else
  {
    bg *temp = background;
    int num = atoi(tokens[1]);
    int sig = atoi(tokens[2]);
    pid_t pid;
    i = num;
    while(temp != NULL && i > 1)
    {
      temp = temp->next;
      i--;
    }

    pid = (temp != NULL) ? temp->pid : -1;

    if(pid == -1)
      fprintf(stderr, "No such job number\n");
    else
    {
      if(kill(pid,sig) < 0)
        perror("Unable to kill process");
      // else
      // {
      //   printf("KILLED %s with pid [%d] and job number %d\n", temp->name, pid, num);
        // delete(pid);
      // }
    }
  }
}

void overkill()
{
  bg *temp = background;
  int flag = 0;
  while(temp != NULL)
  {
    if(kill(temp->pid, 9) < 0)
    {
      flag = 1;
      printf("Unexpected error\n");
    }
    temp = temp->next;
  }
  if(flag == 0)
    printf("Successfully overkilled all bg procs\n");
  else
    printf("Unexpected error\n");

  background = temp;
}

void fg(char **tokens)
{
  int status;
  pid_t pid, pgroupid, child_pid;
  bg *temp = background;
  int num = atoi(tokens[1]), i;
  for(i = 0; tokens[i] != NULL; i++);
	if(i > 2)
			fprintf(stderr,RED "fg: Too many arguments\nUsage: fg <jobNumber>\n" RESET);
	else if(i < 2)
			fprintf(stderr,RED "fg: Too few arguments\nUsage: fg <jobNumber>\n" RESET);
  else
  {
    i = num;
    while(temp !=NULL && i > 1)
    {
      temp = temp->next;
      i--;
    }

    pid = (temp != NULL) ? temp->pid : -1;
    if(pid >= 0)
    {
      printf("%s\n", temp->name);
      pgroupid = getpgid(pid);
      tcsetpgrp(shell, pgroupid);
      if(killpg(pgroupid, SIGCONT) < 0)
        perror("Unexpected error");
      waitpid(pid, &status, WUNTRACED);
      if(WIFSTOPPED(status))
        fprintf(stderr, "[%d]+ stopped %s\n", child_pid, temp->name);
      delete(pid);
      tcsetpgrp(shell, shell_pgid);
    }
    else
			fprintf(stderr,RED "fg: No such pid exists\n" RESET);
  }
}

void Bg(char **tokens)
{
  int i = 0;
  while(tokens[i] != NULL) i++;
  if(i > 2)
    printf(RED "Too many arguments\nUsage: bg <jobNumber>\n" RESET);
  else if(i < 2)
    printf(RED "Too few arguments\nUsage: bg <jobNumber>\n" RESET);
  else
  {
      bg *temp = background;
      int num = atoi(tokens[1]);
      pid_t pid;
      i = num;
      while(temp != NULL && i > 1)
      {
          temp = temp->next;
          i--;
      }
      pid = (temp != NULL) ? temp->pid : -1;

    if(pid == -1)
      fprintf(stderr, "No such job number\n");
    else
    {
      if(kill(pid, SIGCONT) < 0)
        perror("Unexpected Error..");
    }
      
  }
}

