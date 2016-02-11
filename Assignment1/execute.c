//
//  execute.c
//  Assignment1
//
//  Created by Evan Knox on 2016-02-07.
//  Copyright © 2016 NaliApplications. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include "linkedList.h"
#include <ctype.h>

#define NAME_MAX 100

struct historyElem{
    char data[64];
    int number;
};

static struct historyElem history[100];
int commandCount=1;

//Redirect output to a text file
int listToFile(char* file) {
    
    char* arg[] = {"ls", "-l", NULL};
    printf("list to a file \n");
    close(1);
    //Open the file for reading and writing
    int fd= open(file,O_RDWR| O_CREAT, S_IRUSR| S_IWUSR);
    //dup2(fd, 1);
    execvp(arg[0], arg);
    
    return 0;
}

//Change directory command
int cd(char *arg){
    if(chdir(arg)<0){
        printf("Could not change directory\n");
        return 1;
    }
    return 0;
}

//print current directory
int pwd(){
    char buffer[100];
    getcwd(buffer, NAME_MAX);
    printf("CWD is: %s\n",buffer);
    return 0;
}

//free
void freecmd(char *args[64]){
    free(args);
}

int addToHistory(char *args[64],int bg){

    char *ptr;
    long ret = strtol(*args, &ptr, 10);
    if(ret>0){
        //Don't add to history if the command is a number
        printf("Number!!\n");
        return 0;
    }else if(strcmp(*args, "history\n")==0){
        //Don't add the history command
        return 0;
    }else{
        struct historyElem currentElem;
        strcpy(currentElem.data, *args);
        currentElem.number = commandCount+1;
        //printf("Adding command #: %d with string: %s",commandCount,*args);
        history[commandCount] = currentElem;
        commandCount ++;
    }
    return 0;
}

//Will print the last 10 commands
int printHistory(){
    printf("\n-------Last 10 Commands-------\n");
    if(commandCount<10){
        for (int i=1; i<commandCount; i++) {
            printf("\t %d: %s",i,history[i].data);
        }
    }else{
        for (int i=commandCount-10; i<commandCount; i++) {
            printf("\t %d: %s",i,history[i].data);
        }
    }
    printf("\n");
    return 0;
}

int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
        return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

int getcmd(char *prompt, char *args[], int *background, int pt)
{
    int length, i = 0;
    char *token, *loc;
    char *line;
    size_t linecap = 0;
    
    //Sometimes we want to run command without prompt, implementing history!!
    if(pt==1){
        //Valid first time command
        printf("%s", prompt);
        length = (int)getline(&line, &linecap, stdin);
        addToHistory(&line, *background);
    }else{
        line = args[0];
    }
    
    if (length <= 0) {
        exit(-1);
    }
    
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;
    
    while ((token = strsep(&line, " \t\n")) != NULL) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            args[i++] = token;
    }
    return i;
}

//Run a command in a forked process, either bg or fg
int run(char *args[],int bg) {
    pid_t pid;
    int status;
    
    if(bg)
        printf("Running in the background\n");
    else
        printf("Running in the foreground\n");
    
    if((pid=fork())<0) {
        printf("Failed to work\n");
        exit(1);
    }else if (pid == 0){
        //Section that will be executed inside of the child process
        if(args[1]!=NULL &&(strcmp(args[0], "ls"))==0 && (strcmp(args[1], ">")==0) ) {
            listToFile(args[2]);
        }else if(strcmp(args[0], "pwd")==0) {
            pwd();
        }
        else if(strcmp(args[0], "cd")==0) {
            cd(args[1]);
        }
        else if(strcmp(args[0], "history")==0) {
            printHistory();
        }
        else{
            if((execvp(args[0], args)<0)){
                printf("Failed to execute\n");
                exit(1);
            }
        }
    }else if (!bg){ //wait for child process to terminate
        while (wait(&status)!= pid);
    }
    
    return 0;
}

long convertStringToInt(char * num){
    char *ptr;
    return strtol(num, &ptr, 10);
}

//Runs a command that was previously created
int runPreviousCmd(char *cmd){
    //convert the argument to an integer
    int numCommand = (int)convertStringToInt(cmd);
    char *commandNm = history[numCommand].data;
    printf("Running.. %s ok but not actually just yet\n",commandNm);
    return 0;
}

int main2()
{
    char *args[64];
    int bg;
    
    //Enter the main shell loop
    while(1) {
        //First get the users command and add it to history
        getcmd("Shell>>", args, &bg, 1);
        
        //See if if we need to run a previously entered command
        if (isNumeric(args[0])){
            runPreviousCmd(args[0]);
        }else{
            run(args, bg);
        }
        //then run the command in the fg or bg
    }
}