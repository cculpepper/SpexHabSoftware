#include <stdlib.h>
struct QRecStruct {
	char *inPtr;
	char *outPtr;
	char *start;
	char *end;
	int capacity;
	int stored;
};

int enqueue (struct QRecStruct* q, char c){
	if (q->stored < q->capacity){
		*(q->inPtr) = c;
		if (q->inPtr == q->end){
			q->inPtr = q->start;
		} else {
			q->inPtr++;
		}
		q->stored++;
		return 0;
	} else {
		return -1;
	}
}

char dequeue (struct QRecStruct* q){
	char c;
	if (q->stored == 0){
		return 0;
	} else {
		c = *(q->outPtr);
		if (q->outPtr == q->end){
			q->outPtr = q->start;
		} else {
			q->outPtr++;
		} 
		q->stored--;
		return c;
	}
}
struct QRecStruct* initQueue(int len){
	struct QRecStruct *q;
	q = malloc(sizeof(struct QRecStruct));
	q->start = malloc(len * sizeof(char));
	q->capacity = len;
	q->outPtr = q->start;
	q->inPtr = q->start;
	q->stored = 0;
	q->end = q->start + len - 1;
	return q;
}



int deInitQueue(struct QRecStruct* q ){
	free( q->start);
	free(q);
	return 0;
}


