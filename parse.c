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

char *readline()
{
	int x = size;
	char *command = malloc(sizeof(char) * x);
	char c;
	int position = 0;
	while(1)
	{
		c = getchar();
		if(c == EOF){
			printf("\n");
			exit(0);
		}   
		if(c == '\n')
		{
			command[position] = '\0';
			position++;
			return command;
		}
		else{
			command[position] = c;
			position++;
		}
	}
	if(position >= x)
	{
		x += size;
		command = realloc(command, x);
	}
}

char **getArgs(char *input, char* delim)
{
  int i = 0, tsize = size;
  char **tokens = malloc(tsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "hash: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(input, delim);
  while (token != NULL) 
  {
    tokens[i++] = token;

    if (i >= size) 
    {
      tsize += size;
      tokens = realloc(tokens, size * sizeof(char*));
      if (!tokens) 
      {
        fprintf(stderr, "hash: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, delim);
  }
  tokens[i] = NULL;
  return tokens;
}


int check1(char *inp)
{
	int len = 0, i;
	for(i = 0 ; i < strlen(inp) ; i++)
	{
		if(inp[i] == '>' || inp[i] == '<')
		len++;
	}
	return len;
}

int check(char *inp)
{
	int i, flag = -1;
	for(i = 0 ; i < strlen(inp) ; i++)
	{
		if(inp[i] == '>')
		{
			if(inp[i+1] == '>')
			{ 
				flag = 2;
				break;
			}
			else
			{  
				flag = 0;
				break;
			}
		}
		else if(inp[i] == '<')
		{
			flag = 1;
			break;
		}
	}
	return flag;
}
