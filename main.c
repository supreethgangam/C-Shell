#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include"token.h"
#include"ls.h"
#include"process.h"
#include"history.h"
#include<ctype.h>

char home[1024];
char display[1024];
char bgcmd[30];
char prev[200];

void change_display(char* address)
{
  register struct passwd *pw;
  register uid_t uid;
  char user[200];
  uid = geteuid();
  pw = getpwuid(uid);
  if(pw)
  {
    strcpy(user,pw->pw_name);
  }
  char host[200];
  host[199]='\0';
  gethostname(host,199);
  strcpy(display,"<");
  strcat(display,user);
  strcat(display,"@");
  strcat(display,host);
  strcat(display,":~");
  strcat(display,address);
  strcat(display,">");
}


void checkbgproc()
{
  int status;
  pid_t w;
  w = waitpid(-1, &status, WNOHANG);
  if(w>0)
  {
    if(WIFEXITED(status))
    {
      fprintf(stderr,"%s with pid %d exited normally\n",bgcmd,w);
    }
    else
    {
      fprintf(stderr,"%s with pid %d exited abnormally\n",bgcmd,w);
    }
  }
}


void pinfo(char* argv[],int len)
{
  pid_t pid;  
  char* file = (char*)malloc(100);
  if(len == 1)
  {
    pid = getpid();
    sprintf(file,"/proc/%d/stat",pid);
  }
  else 
  {
    sprintf(file,"/proc/%s/stat",argv[1]); 
  }
  FILE* fp = fopen(file,"r");
  if(!fp)
  {
    perror("Status File cant be acccessed");
    exit(-1);
  }
  char* path = (char*)malloc(1000);
  char st;
  char* word = (char*)malloc(1000);
  int vm;

  char status;
  int pgrp;
  int tpgid;
  int mem;
  fscanf(fp,"%d %*s %c %*s %d %*s %*s %d ",&pid, &status, &pgrp, &tpgid);
  fclose(fp);
  if(len == 1)
  {
    pid = getpid();
    sprintf(file,"/proc/%d/status",pid);
  }
  else 
  {
    sprintf(file,"/proc/%s/status",argv[1]); 
  }
  fp = fopen(file,"r");

  while (fscanf(fp, " %s", word) == 1) 
  {
        if (strcmp(word, "VmSize:") == 0) 
        {
            fscanf(fp, "%d", &mem);
        }
  }
  if(len == 1)
  {
    pid = getpid();
    sprintf(file,"/proc/%d/exe",pid);
  }
  else 
  {
    sprintf(file,"/proc/%s/exe",argv[1]); 
  }

  char* path1 = (char*)malloc(1000);
  int i = readlink(file,path1,200);
  if(i!=-1)
  {
    path1[i]='\0';
  }
  else
  {
    strcat(path1,path);
  }

  free(file);
  free(word);
  fclose(fp);

  printf("pid -- %d\n",pid);
  printf("Process Status -- %c",status);    
  if(pgrp == tpgid)
  {
    printf("+");
  }
  printf("\n");
  printf("memory -- %d KB\n",mem);
  printf("Executable Path -- %s\n",path1);
    
  free(path);  
  free(path1);
}

void execute(char* line)
{
  checkbgproc();
  char cmd[100];
  char* token = (char*)malloc(1024);
  char* args[100];
  int len = 0;
  token = strtok(line," \t\n");
  strcpy(cmd,token);

  while(token != NULL && strcmp(token,""))
  {
    args[len++] = token;
    token = strtok(NULL," \t\n");  
  }

  char* address = args[1];

  if(!strcmp(cmd,"cd"))
  {
    char cwd3[100];
    char temp[100];
    if(!strcmp(address,"~") || len == 1)
    {
      strcpy(address,home);
    }
    
    getcwd(cwd3,100);
    strcpy(temp,cwd3);

    if(!strcmp(address,"-"))
    {
      strcpy(address,prev);
      printf("%s\n",address);
      chdir(home);
      char cwd2[100];
    }

    strcpy(prev,temp);
    

    if(access(address,F_OK) == 0)
    {
      chdir(address);
      char cwd1[100];
      getcwd(cwd1,100);
      change_display(cwd1);
    }

    else
    {
      perror("Cannot access the directory");
      return;
    }

  }

  else if(!strcmp(cmd,"pwd"))
  {
    char cwd[100];
    getcwd(cwd,100);
    puts(cwd);
  }

  else if (!strcmp(cmd,"echo"))
  {
    char* temp1 = (char*)malloc(10000);
    strcpy(temp1,args[1]);
    for(int i=2;i<len;i++)
    {
      strcat(temp1,args[i]);
    }
    printf("%s\n",temp1);
  }

  else if(!strcmp(cmd,"ls"))
  {
    ls_function(len,args);
  }

  else if(!strcmp(cmd,"pinfo"))
  {
    pinfo(args,len);
  }

  else if(!strcmp(cmd,"repeat"))
  {
    int num = atoi(args[1]);
    char* temp = (char*)malloc(100);
    strcpy(temp,args[2]);
    if(len<3)
    {
      printf("Error: Enter proper repeat command");
      return;
    }
    for(int i=3;i<len;i++)
    {
      strcat(temp," ");
      strcat(temp,args[i]);
    }
    while(num--)
    {
      execute(temp);
    }
  }

  else if(!strcmp(cmd,"history"))
  {
    if(len == 1)
    {
      show_history();
    }
    else if(len == 2)
    {
      int num;
      if(strlen(args[1])==2)
      {
        num = args[1][0]-'0';
        num *= 10;
        num += args[1][1]-'0';
        if(num>20 || num<0)
        {
          printf("Enter proper command");
        }
        show_history1(num);  
      }
      else
      {
        show_history1(args[1][0]-'0');
      }
    }
    else
    {
      printf("Enter proper command");
    }
  }

  else
  {
    if(strcmp(args[len-1],"&"))
    {
      args[len] = NULL;
      process(args,0);
    }
    else
    {
      args[--len] = NULL;
      strcpy(bgcmd,args[0]);
      process(args,1);
    }
  }

}

void killbg() 
{
  pid_t pid;
  pid = getpid();
  kill(pid,SIGKILL);
}

int main()
{
  
  getcwd(home,1024);
  change_display(home);

  while(1)
  {
    printf("%s",display);
    int j;
    char* cmd = (char*)malloc(1024);
    fgets(cmd,1024,stdin);
    history(cmd);
    int asc = (int)cmd[0];
    if(!strcmp(cmd,"exit\n"))
    {
      killbg();
      break;
    }
    if(asc!=10)
    {
      char* tokens[100];
      int num_tokens;
      num_tokens = tokenize(cmd,tokens);
      int flag_bg = 0;
      for(int i=0;i<num_tokens;i++)
      {
        execute(tokens[i]);
      }
    }
  }
  return 0;
}