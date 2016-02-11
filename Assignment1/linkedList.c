/*
Author: Omar Ba mashmos
ID#: 260617216
File: Implementing pust, find and delete on a linked list
Source: Comp 310-ECSE 427 tutorial  

*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include "linkedList.h"

Node* head=NULL;
Node* tail=NULL;

int push(char *s[64], int number, int bg) {
	
	if(head==NULL) { //If head isn't initialized, create it in memory
		head=(Node*) malloc(sizeof(Node));
		tail=head;
	}else { //If head exists,
		tail->next=(Node*) malloc(sizeof(Node));
		tail=tail->next; //Update tail pointer to be the new Node that we've created
    }
    //Now update the tail node that we're adding
    tail->bg = bg;
    tail->number = number;
    //Copy the sentence to the data array
    for (int i=0; i<64; i++) {
        strcpy(tail->data[i], *s[i]);
    }

	
    return 0;
}

int find(char* s, int len) {
	Node* it=head;
	int i=0;
	if(it==NULL) {
		//List is empty
		return -1;
	}
	
	while(it!=NULL) {
		if(strcpy(s, it->data)==0){
			return i;
		}
		i++;
		it=it->next;
		
	}
	//could not find 
	return -1;
	
	
}