//命令行解释器
/*
解析逻辑：
parsecmd() 以pipeline的链式调用进行命令解析
顺序为 parsecmd() -> parseline() -> parsepipe() -> parseexec() -> parseblock() -> parseredirs()
分别对不同类型的命令进行处理，从左往右不断使用peek()函数判断当前的符号
使用gettoken()获取空格分割的参数，构造树状命令结构。

解析完成后，调用nulterminate方法进行递归的参数截取。
*/

#include "../kernel/user/user.h"
#include "../kernel/user/string.h"

void start_bash();

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);
struct cmd *nulterminate(struct cmd*);

const char whitespace[] = " \t\r\n\v";
const char symbols[] = "<|>&;()";

int main(int argc, char* argv) {
    char *str = (char*)malloc(20);
    safestrcpy(str, "bash start\n", 20);
    printf("%s\n", str);
    start_bash();
    return 0;
}

int
getcmd(char *buf, int nbuf) {
    char cwd[128];
    pwd(cwd, 128);

    char name[32];
    uname(name, 32);

    printf("%s:%s# ", name, cwd);
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if(buf[0] == 0) // EOF
      return -1;
    return 0;
}




/**************************构造命令*************************/
struct cmd*
execcmd(void)
{
  struct execcmd *cmd;
  // disheap();
  printf("execcmd\n");

  cmd = (struct execcmd *)malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXEC;
  printf("end execcmd cmd: %x\n", cmd);
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
backcmd(struct cmd *subcmd)
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  return (struct cmd*)cmd;
}
/**************************构造命令*************************/

//取出剩余串首字符
//q:第一个非空白字符位置 eq:字符截取结束位置(被截取的下一个)
int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '(':
  case ')':
  case ';':
  case '&':
  case '<':
    s++;
    break;
  case '>':
    s++;
    if(*s == '>'){
      // >> 
      ret = '+';
      s++;
    }
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

//ps 命令字符数组 es 结束位置 toks检测符号
//移动ps指针 若检测到目标符号则返回真 否则返回0(遇到'\0'同样)
int
peek(char **ps, char *es, const char *toks)
{
  char *s;  //s:命令指针
  
  s = *ps;

  //忽略空白符
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

//解析重定向
struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    //检测到 <>   tok:获取
    tok = gettoken(ps, es, 0, 0);
    
    //获取重定向文件（q -> eq）
    if(gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch(tok){
    case '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, STDIN);  //STDIN存疑
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, STDOUT);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, STDOUT);
      break;
    }
  }
  return cmd;
}

//解析命令块
struct cmd*
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  if(!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  if(!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  return cmd;
}

//ps 起始位置指针 es 结束位置
struct cmd*
parseexec(char **ps, char *es) {
  printf("parse exec\n");
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
      return parseblock(ps, es);

  //没有子块 =>构造exec命令(仅开辟空间)
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  //若没有重定向 则不会发生改变
  ret = parseredirs(ret, ps, es);
  printf("after parseredirs\n");

  //未检测到"|)&;"
  while(!peek(ps, es, "|)&;")){
    //无参数 退出
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
        break;
    if(tok != 'a')
        panic("syntax");
    
    // printf("parse argv: %s\n", cmd->argv[argc]);
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if(argc >= MAXARGS)
        panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argc = argc;
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  printf("parseexec end, ret:%x\n", ret);
  return ret;
}

//ps 起始位置指针 es 结束位置
struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  //检测到管道
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}


//ps 起始位置指针 es 结束位置
struct cmd*
parseline(char **ps, char *es)
{
  printf("start to parse line\n");
  struct cmd *cmd;
  
  cmd = parsepipe(ps, es);

  while(peek(ps, es, "&")){
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  printf("parseline end cmd: %x\n", cmd);
  return cmd;
}

extern void pause();

struct cmd*
parsecmd(char *s)
{
  printf("start to parse cmd : %s\n", s);
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);  //cmd结束位置
  cmd = parseline(&s, es);
  
  peek(&s, es, ""); //移动指针
  if(s != es){
    printf("leftovers: %s\n", s);
    panic("syntax");
  }
  //获取参数
  nulterminate(cmd);
  //printf("end parse , argv[0]:%s\n", ((struct execcmd*)cmd)->argv[0]);
  printf("end parse  cmd addr:%x\n", cmd);
  return cmd;
}

struct cmd*
nulterminate(struct cmd *cmd)
{
  int i;
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    return 0;
  
  switch(cmd->type){
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    //直接在原串上打上结束符
    for(i=0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    break;
    
  case LIST:
    lcmd = (struct listcmd*)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}


// Execute cmd
void
runcmd(struct cmd *cmd)
{
  printf("start to run cmd : %x\n", cmd);
  int p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit();
  
  switch(cmd->type){
  default:
    panic("runcmd");

  case EXEC:
    printf("runcmd type EXEC  argv[0]: %x\n", ecmd->argv[0]);
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit();
    exec(ecmd->argv[0], ecmd->argc, ecmd->argv);

    printf("exec %s failed\n", ecmd->argv[0]);
    break;
  /*
  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if(open(rcmd->file, rcmd->mode) < 0){
      printf("open %s failed\n", rcmd->file);
      exit();
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if(fork() == 0)
      runcmd(lcmd->left);
    wait();
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
    if(fork() == 0){
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork() == 0){
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait();
    wait();
    break;
    
  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork() == 0)
      runcmd(bcmd->cmd);
    break;
  */
  }
  exit();
}


void start_bash() {
    static char buf[100];
//   Read and run input commands.
    while(getcmd(buf, sizeof(buf)) >= 0){
        if (buf[0] == '\0' || buf[0] == '\n' || buf[0] == '\r') continue;
        if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
            //切换目录
            buf[strlen(buf)-1] = 0;
            if(chdir(buf+3) != 0) printf("cannot cd %s\n", buf+3);
            continue;
        }
        if(buf[0] == 'p' && buf[1] == 'w' && buf[2] == 'd' &&
            (buf[3] == '\n' || buf[3] == ' ' || buf[3] == '\0' || buf[3] == '\r')){
            char cwd[128];
            pwd(cwd, 128);
            printf("%s\n", cwd);
            continue;
        }
        if(fork() == 0)
            runcmd(parsecmd(buf));
        wait();
    }

    printf("fail to get cmd\n");
    while (1);
}