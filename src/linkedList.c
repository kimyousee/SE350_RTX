#include "linkedList.h";
#include "k_rtx.h";

LinkedList* initLinkedList(U8 *pointer) {
	LinkedList *l = (LinkedList *)pointer;
	l->head = (Node *)(pointer+128);
	l->tail = (Node *)(pointer+128);
	l->head->next = NULL;
	l->tail->next = NULL; 
	return l;
}

Node* popLinkedList(LinkedList *l) {
	Node *n = l->head;
	if (l->head == NULL) {
		return NULL;
	}
	
	l->head = l->head->next;
	return n;
}

void pushLinkedList(LinkedList *l, Node *n) {
	l->tail->next = n;
	l->tail = l->tail->next;
	l->tail->next = NULL;
}
