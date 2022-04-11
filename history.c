#include<stdio.h>
#include<stdlib.h>
#include<string.h>


char* cmds[20];
int len = 0;

void history(char* cmd)
{
	if(len>=20)
	{
		len = (len)%20;
		free(cmds[len]);
	}
	if(len>0)
	{
		if(!strcmp(cmds[len-1],cmd))
		{
			return;
		}
	}
	cmds[len] = (char*)malloc(strlen(cmd));
	strcpy(cmds[len],cmd);
	len++;
}

void show_history()
{
	int j=(10-len>0)?0:len-10;
	for(int i=j;i<len;i++)
	{
		printf("%s",cmds[i]);
		j++;
	}
}

void show_history1(int num)
{
	if(num<=len)
	{
		for(int i=len-num;i<len;i++)
		{
			if(cmds[i]!=NULL)
			{
				printf("%s",cmds[i]);
			}
		}
	}
	else
	{
		for(int i=0;i<len;i++)
		{
			printf("%s",cmds[i]);
		}
	}
}

