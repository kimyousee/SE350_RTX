#include "linkedList.h"
#include "k_rtx.h"

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
		n->next = NULL;
		l->head = n;
		l->tail = n;
	}
	l->tail->next = n;
	l->tail = l->tail->next;
	l->tail->next = NULL;
}

void sortPushLinkedList(LinkedList *l, Node *n) {
	Node *cur = l->head;
	if (cur == NULL) {
		pushLinkedList(l, n);
		return;
	}
	if (n->value < cur->value) {
		n->next = cur;
		l->head = n;
		return;
	}
	while(cur->next != NULL && cur->next->value < n->value) {
		cur = cur->next;
	}
	
	if (cur->next == NULL) {
		l->tail = n;
	}
	n->next = cur->next;
	cur->next = n;
}
