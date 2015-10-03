#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE 80


void splitString(char *str, char * args[],int& arg_num,bool& amp)
{
    int len = strlen(str);
    int i,l,r;
    l = 0;
    arg_num = 0;
    for(i=0;i<len-1;i++){
        if(str[i+1] == ' '){
            args[arg_num] = (char*)malloc(MAX_LINE);
            strncpy(args[arg_num],str+l,i-l+1);
            arg_num++;
            l = i+2;
        }
    }

    args[arg_num] = (char*)malloc(MAX_LINE);
    strncpy(args[arg_num],str+l,len-l);
    arg_num++;

    //args[arg_num] = (char*)malloc(MAX_LINE);
    args[arg_num] = NULL;

    amp = false;
    int last = arg_num - 1;
    if(strcmp(args[last],"&") == 0){
        amp = true;
        free(args[last]);
        args[last] = NULL;
        arg_num--;
    }
}

int main()
{
    char *args[MAX_LINE/2 +1];
    char * history[MAX_LINE];
    int should_run = 1;
    int count_cmd = 0;
    while(should_run){
        char pp[MAX_LINE];
        getcwd(pp,MAX_LINE);
        printf("%s$ ",pp);
        //printf("");
        fflush(stdout);
        //input process
        char str[MAX_LINE];
        gets(str);

        //process !
        if(str[0] == '!' && strlen(str) > 1){
            if(str[1] == '!'){
                if(count_cmd==0){
                    printf("No such command in history.\n");
                    continue;
                }else{
                    strcpy(str,history[count_cmd-1]);
                }
            }else{
                int num = 0;
                int len = strlen(str);
                for(int i=1;i<len;i++){
                    num = num*10 + str[i] - '0';
                }
                if(num>count_cmd){
                    printf("No such command in history.\n");
                    continue;
                }else{
                    strcpy(str,history[num-1]);
                }
            }
        }
        //end !

        int arg_num; bool amp;
        splitString(str,args,arg_num,amp);

        //process 'cd'
        if(strcmp("cd",args[0])==0){
            if(arg_num==1) continue;
            char path[MAX_LINE];
            strncpy(path,str+3,strlen(str));
            chdir(path);
            continue;
        }
        //end 'cd'

        history[count_cmd] = (char*)malloc(MAX_LINE);
        strcpy(history[count_cmd],str);
        count_cmd++;

        if(strcmp(args[0],"exit") == 0) return 0;

        //fork

        pid_t pid = fork();
        if(pid < 0){
            printf("fork failed!\n");
        }else if(pid == 0){
            if(strcmp(args[0],"history") == 0){
                int pcount=count_cmd,i=0;
                while(pcount&&i<10){
                    pcount--;
                    i++;
                    printf("%d %s\n",pcount+1,history[pcount]);
                }
            }else{
                execvp(args[0],args);
            }
        }else{
            int status;
            if(!amp) waitpid(pid,&status,NULL);
        }

    }
    return 0;
}
