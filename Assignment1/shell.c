//
//  shell.c
//  Assignment1
//
//  Created by Evan Knox on 2016-02-10.
//  Copyright © 2016 NaliApplications. All rights reserved.
//

/*
    Every command given will be counted as a line command
        First string of the command is the name of the program file
        The next strings are counted as arguments
        If the last argument is &, the shell runs in the background
 
 
        1. Creating child process and executing command in the child
            -If the command is a bad command don't store it in the history, warn user
        2. Create a history feature
            -User to access the 10 most recently entered commands starting at 1 and growing
            -If there is no command with an id of whatever, print 'no command found in history'
            -If they choose to enter the command make sure to echo the command on the users screen.
 
        3. Additional commands
            -cd, pwd, exit are all to be done internally
            -fg and jobs as well, need to bring a command from foreground to background
            -output redirection
 
 */

#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

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
};

/*
    Initialize our linked list, simple malloc, perhaps change this later...
 */
int initializeCmdList(){
    struct Command *HeadOfList = (struct Command*)malloc(sizeof(struct Command));
    head = HeadOfList;
    tail = HeadOfList;
    head->num = 0;
    return 0;
}

/*
    Important to note that this won't add the command to history
    This function just parses a possible command and handles the case
    where the user enters a number in search of another command
 */
struct Command * getCmd(){
    //Start the parsing
    char *token, *loc;
    //int i = 0;
    //Create a struct representing the command, allocate memory for it
    struct Command *cmd = (struct Command*)malloc(sizeof(struct Command));
    
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
    while ((token = strsep(&line, " \t\n")) != NULL || argCount < 10) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            strcpy(cmd->args[argCount], token);
        argCount++;
    }
    
    //If the line is just a number then look up that number in history,
        //Update the new struct accordingly to that commands struct aa
    
    //Update the rest of the struct!
    cmd->argCount = argCount;
    cmd->num = head->num+1;
    
    //return the struct that represents the command we're about to run
    return cmd;
}

int addToHistory(struct Command *cmd){
    //We want to the tail of cmd to point to the head of our list
    cmd->next = head;
    head = cmd;
    //Then update the head pointer
    return 0;
}

int runCmd(struct Command *cmd){
    //If the command is a built in Command ->'history', 'cd', 'pwd', 'exit', 'fg', 'jobs', '>(redirection)'
    if (strcmp(cmd->args[0], "history") == 0) {
        //
    }else if (strcmp(cmd->args[0], "cd") == 0) {
        //
    }
    else if (strcmp(cmd->args[0], "pwd") == 0) {
        //
    }
    else if (strcmp(cmd->args[0], "exit") == 0) {
        //
    }else if (strcmp(cmd->args[0], "fg") == 0) {
        //
    }else if (strcmp(cmd->args[0], "jobs") == 0) {
        //
    }else if (strcmp(cmd->args[0], "ls") == 0 && strcmp(cmd->args[1], ">")) {
        //
    }else{
        //Else the command is not a built in Command, run using ExecVp
        //Run it if and only if the error flag is 0, if 1 then we print 'this was a bad command last time'
        if (cmd->error != 1) {
            //Create a child process and run the command using execvp

            
            //If the command doesn't execute set it as a failed command
            cmd->error = 1;
        }
        
        if(cmd->bg == 0 && cmd->error == 0){
            //If we are running in the fg, then wait on the new process to finish (assuming it didn't fail)
        }
    }
    
    //The command should have been executed at this point, add it to history, if it didn't fail
    if (cmd->error != 1) {
        addToHistory(cmd);
    }
    
    
    
    //If the command was a bad command, then set the error flag to be 1
    return 0;
}

int freeCmd(){
    return 0;
}


int main(){
    initializeCmdList();//Need to initialize the linked list
    struct Command *currentCmd;
    while (1) {
        //first get the users command
        currentCmd = getCmd();
        //printf("Command: %d Argument(s): %s\n",currentCmd->num, currentCmd->args[0]);
        
        //Then run the users command
        runCmd(currentCmd);
        
        
        
    }
}