#ifndef _LINKED_LIST_
#define _LINKED_LIST_

typedef struct NodeStruct {
	struct NodeStruct *next;
	int value;
	void *message;
} Node;

typedef struct LinkedListStruct {
	struct NodeStruct *head;
	struct NodeStruct *tail;
} LinkedList;

Node* popLinkedList(LinkedList *l);
void pushLinkedList(LinkedList *l, Node *n);
void sortPushLinkedList(LinkedList *l, Node *n);
int linkedListHasNext(LinkedList *l);

#endif
