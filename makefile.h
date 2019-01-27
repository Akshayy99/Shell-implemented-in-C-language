#define size 1024
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define clear() printf("\033[H\033[J")

pid_t shell_pgid;
char **ARR;
int PID;
typedef struct bg
{
  char name[1024];
  pid_t pid,pgid;
  struct bg *next;
}bg;
bg * background;
int shell;
pid_t shell_pgid;
int mainPid;
int childPid;

void pwd();
void echo(char **args);
void cd();
char *readline();
char **getArgs(char *input, char* delim);
char *cuserid(char *string); 
void pinfo(char **args, char *path);
void quit();
void about();
void init_shell();
void sigintHandler(int);
void print_perms(mode_t);
void ls(char*, int);
void ls_l(char*, int );
void insert(char*, pid_t);
void delete(pid_t);
void convertCWD(char*, char*);
void bgResponse(int);
void procSTOP(int) ;
void remindme(char**);
void clk(int, int);
void exe(char**);
int exec(char**, char*, char*);
int check1(char*);
int check(char*);
void set_env(char**);
void unset_env(char**);
void jobs();
void kjob(char**);
void overkill();
void fg(char**);
void Bg(char **);
