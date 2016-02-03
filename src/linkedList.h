#include "k_rtx.h";

typedef struct NodeStruct {
	struct NodeStruct *next;
} Node;

typedef struct LinkedListStruct {
	struct NodeStruct *head;
	struct NodeStruct *tail;
} LinkedList;

LinkedList* initLinkedList(U8 *pointer);
Node* popLinkedList(LinkedList *l);
void pushLinkedList(LinkedList *l, Node *n);