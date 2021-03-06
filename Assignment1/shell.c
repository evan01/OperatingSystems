//
//  shell.c
//  Assignment1
//
//  Created by Evan Knox (Worked with Omar BaMashmos!)
//  Student Number : 260502120
//

#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include<signal.h>
#define NAME_MAX 100;

size_t MAXLINE = 64;

//Linked list implementation
struct Command *head;
struct Command *tail;
struct Command{
    struct Command *next;//Pointer to the next command
    int num;//Represents the command number
    int bg; //Whether this is a bg command or not
    char args[10][64]; //The actual arguments passed by the user
    int argCount;
    int error; //When the command runs, whether it failed or not
    int pid; //For keeping track of background jobs (for fg and jobs)
    int badHistoryCmd; //So that we don't run any history commands (makes output nicer)
};

/*
 Initialize our linked list, simple malloc, perhaps change this later...
 */
int initializeCmdList(){
    struct Command *HeadOfList = (struct Command*)malloc(sizeof(struct Command));
    HeadOfList->next = NULL;
    head = HeadOfList;
    tail = HeadOfList;
    head->num = 0;
    return 0;
}

/*
 This will print the entire command on the same line
 */
int printCommand(struct Command *cmd){
    int i=0;
    while (strcmp(cmd->args[i],"")!= 0) {
        printf("%s ",cmd->args[i]);
        i++;
    }
    return 0;
}

/*
 Given a number representing a command number, return pointer to struct
 that represents that command
 */
struct Command * getCommandFromNum(int number){
    struct Command *current = head;
    
    while (current != tail) {
        if (current->num == number) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/*
 Lists all the currently running jobs in the bg
 */
struct Command * listJobs(){
    struct Command *current = head;
    
    while (current != tail) {
        if (current->bg == 1) {
            if(kill(current->pid, 0)==0) {
                printf("Process ID: %d \t",current->pid);
                printCommand(current);
            }
        }
        current = current->next;
    }
    printf("\n");
    return NULL;
}

/*
 Will tell us whether a string is numeric or not
 */
int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
        return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

/*
 Converts a string to a number
 */
long convertStringToInt(char * num){
    char *ptr;
    return strtol(num, &ptr, 10);
}

/*
 Important to note that this won't add the command to history
 This function just parses a possible command and handles the case
 where the user enters a number in search of another command
 */
struct Command * getCmd(){
    char *token, *loc;
    //Create a struct representing the command, allocate memory for it
    struct Command *cmd = (struct Command*)malloc(sizeof(struct Command));
    for (int i=0; i<10; i++) {
        strcpy(cmd->args[i], "");
    }
    
    //Prompt the user to enter a command and store it as a line -->The ONLY USER INTERACTION
    printf("Shell>>");
    char *line = (char *) malloc (MAXLINE+1);
    int length = (int)getline(&line, &MAXLINE, stdin);
    if (length <= 0) {
        exit(-1);
    }
    
    // Check if background is specified..Update the cmd struct
    if ((loc = index(line, '&')) != NULL) {
        cmd->bg = 1;
        *loc = ' ';
    } else
        cmd->bg = 0;
    
    //Now it's time to parse the line that user entered
    int argCount = 0;
    while ((token = strsep(&line, " \t\n")) != NULL && argCount < 10) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0){
            strcpy(cmd->args[argCount], token);
            argCount++;
        }
        
    }
    
    //If the line is just a number then look up that number in history,
    //Update the new struct accordingly to that commands struct
    if (isNumeric(cmd->args[0])) {
        //First get the number
        int num = (int)convertStringToInt(cmd->args[0]);
        
        //Then find the coresponding command associated with this in
        struct Command *AssociatedCmd = getCommandFromNum(num);
        if (AssociatedCmd != NULL) {
            //Update the current cmd struct with the args of cmd
            for (int l=0; l<10; l++) {
                strcpy(cmd->args[l],AssociatedCmd->args[l]);
            }
            cmd->bg = AssociatedCmd->bg;
            cmd->argCount = AssociatedCmd->argCount;
            cmd->error = AssociatedCmd->error;
            cmd->num = head->num+1;
        }else{
            cmd->badHistoryCmd = 1; //Use this to basically not run the command later...
        }
        
    }else{
        //Not a numeric command!
        cmd->argCount = argCount;
        cmd->num = head->num+1;
    }
    
    //return the struct that represents the command we're about to run
    return cmd;
}

int addToHistory(struct Command *cmd){
    //We want to the tail of cmd to point to the head of our list
    cmd->next = head;
    head = cmd; //Then update the head pointer
    
    return 0;
}

/*
 This frees the struct representing a particular command
 */
int freeCmd(struct Command *cmd){
    free(cmd);
    return 0;
}

int exitShell(){
    exit(0);
}


//print current directory
int pwd(){
    char buffer[100];
    size_t bufsize = 100;
    getcwd(buffer, bufsize);
    printf("CWD is: %s\n",buffer);
    return 0;
}
int isRedirect(struct Command *cmd) {
    for(int i=0; i< 64;i++){
        if((strcmp(cmd->args[i],">"))==0){
            return i;
        }
        
    }
    return -1; // did not find redirection sign >
    
}
//Redirect output to a text file
int listToFile(struct Command *cmd, int loc) {
    printf("list to a file \n");
    char *command = (char *)malloc(64);
    char *argv[64] = {NULL};
    for(int i=0;i<(cmd->argCount);i++){
        argv[i] = malloc(sizeof(command));
        strcpy(argv[i],cmd->args[i]); //Can't do this because argv[i] isn't initialized
    }
    
    if(fork()==0) {
        close(1);
        //Open the file for reading and writing
        int fd= open(argv[loc+1],O_RDWR| O_CREAT, S_IRUSR| S_IWUSR);
        argv[loc]=NULL;
        argv[loc+1]=NULL;
        if((strcmp(argv[0], "pwd"))==0) {
            pwd();
        }
        else{
            execvp(argv[0], argv);
        }
    }
    return 0;
}

//Change directory command
int cd(char *arg){
    if(chdir(arg)<0){
        printf("Failed change directory\n");
        return 1;
    }
    return 0;
}

/*
 Print the last 10 commands the
 */
int printHistory(){
    struct Command *node = head;
    
    printf("\n\t-----HISTORY-----\n");
    //We want to print the 10 most recent commands
    for (int i = 0; i<10; i++) {
        //Handle the case where we have less than 10 commands!
        if(node->next == NULL)
            break;
        //First print the command number
        printf("%d\t",node->num);
        
        //Then print the users command
        printCommand(node);
        
        printf("\n");
        //Then update the node
        node=node->next;
    }
    
    return 0;
}

/*
 This function will take in a command struct and run the command args in a child process
 */
int runChildProcess(struct Command *cmd){
    int pid,status;
    
    if ((pid = fork())<0) {
        printf("Failed to fork a child process, shell exiting\n");
        exitShell();
    }else if (pid == 0){
        char *command = (char *)malloc(64);
        //Format args to be passed into the exec vp function
        char *argv[64] = {NULL};
        for(int i=0;i<(cmd->argCount);i++){
            argv[i] = malloc(sizeof(command));
            strcpy(argv[i],cmd->args[i]); //Can't do this because argv[i] isn't initialized
        }
        if ((execvp(argv[0], argv)) < 0) {
            printf("Invalid command, failed to run\n");
        }
        exit(0); // Make sure to terminate the child process! Or else things get wierd...
    }else{
        /* In PARENT PROCESS */
        if(cmd->bg == 0){
            //We have to wait for the process to finish
            while (wait(&status) != pid); // SURE THIS IS RIGHT??
        }else{
            //We store the pid of the child process, for later
            cmd->pid = pid;
        }
    }
    
    return 0;
}

struct Command * fg(int processID){
    struct Command *current = head;
    
    while (current != tail) {
        if (current->bg == 1) {
            if(current->pid==processID) {
//                current->bg=0;
//                runChildProcess(current);
                current->bg = 0;
                signal(SIGTTOU, SIG_IGN);
                pid_t pid= (pid_t)processID;
                tcsetpgrp(STDIN_FILENO, pid);
                current = tail;
                return NULL;
            }
        }
        current = current->next;
    }
    printf("No such process is running in the background");
    return NULL;
}


/*
 This function will take in a command struct and run the command
 based off of the contents of the struct
 */
int runCmd(struct Command *cmd){
    //If the command is a built in Command ->'history', 'cd', 'pwd', 'exit', 'fg', 'jobs', '>(redirection)'
    //Check if we are doing a redirect command
    int location=isRedirect(cmd);
    
    if (strcmp(cmd->args[0], "history") == 0) {
        printHistory();
    }else if (strcmp(cmd->args[0], "cd") == 0) {
        cd(cmd->args[1]);
    }
    else if (strcmp(cmd->args[0], "pwd") == 0 && location <0) {
        pwd();
    }
    else if (strcmp(cmd->args[0], "exit") == 0) {
        exitShell();
    }else if (strcmp(cmd->args[0], "fg") == 0) {
        if(isNumeric(cmd->args[1])==1){
            fg((int)convertStringToInt(cmd->args[1]));
        }else{
            printf("Invalid number entry");
        }
    }else if (strcmp(cmd->args[0], "jobs") == 0) {
        listJobs();
    }else if (location>=0) {
        listToFile(cmd, location); // IS THIS JUST JUST for the ls command??
    }else{
        //Else the command is not a built in Command, run using ExecVp in CHILD PROCESS
        if (cmd->error != 1 && cmd->badHistoryCmd != 1) {
            printf("Running the cmd ");
            for (int i=0; i<cmd->argCount; i++) {
                printf("%s ",cmd->args[i]);
            }
            printf("\n");
            
            //Run the Command!
            runChildProcess(cmd);
            
        }else{
            //Last time we executed the command it failed, warn the user
            if(cmd->badHistoryCmd !=1){
                printf("The command: ");
                printCommand(cmd);
                printf(" failed to execute last time, not entering it into history");
                
            }else{
                //The command was a bad history command
                printf("No command found in history\n");
            }
            
        }
        
    }
    
    //The command should have executed by this point, add it to history if it didn't fail
    if (cmd->error != 1 && strcmp(cmd->args[0],"history")!= 0 && cmd->badHistoryCmd !=1) {
        addToHistory(cmd);
    }else{
        //If was a bad command or we don't want to add the command to history, free the cmd
        freeCmd(cmd);
    }
    
    return 0;
}

int main(){
    initializeCmdList();//Need to initialize the linked list
    struct Command *currentCmd;
    while (1) {
        //first get the users command
        currentCmd = getCmd();
        //printf("Command: %d Argument(s): %s\n",currentCmd->num, currentCmd->args[0]);
        
        //Then run the users command, also adds to history
        runCmd(currentCmd);

        
    }
}
