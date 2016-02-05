#include "linkedList.h";
#include "k_rtx.h";

LinkedList* initLinkedList(U8 *pointer) {
	LinkedList *l = (LinkedList *)pointer;
	l->head = (Node *)(pointer+MEMORY_BLOCK_SIZE);
	l->tail = (Node *)(pointer+MEMORY_BLOCK_SIZE);
	l->head->next = NULL;
	l->tail->next = NULL; 
	return l;
}

int linkedListHasNext(LinkedList *l) {
	if (l->head == NULL) {
		return 0;
	}
	return 1;
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
	if (l->head == NULL) {
		l->head = n;
	}
	l->tail->next = n;
	l->tail = l->tail->next;
	l->tail->next = NULL;
}
