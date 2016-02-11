/*
Author: Omar Ba mashmos
ID#:260617216
Linked List header file
Source: Comp 310- ECSE 427 tutorial One slides
*/

typedef struct _Node{
    char *data[64];
    int bg;
    int number;
    struct _Node* next;
} Node;

int push(char *s[64], int number, int bg);