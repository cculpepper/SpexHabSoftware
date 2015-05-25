struct QRecStruct {
	char *inPtr;
	char *outPtr;
	char *start;
	char *end;
	int capacity;
	int stored;
};

int enqueue (struct QRecStruct*, char);
int dequeue (struct QRecStruct*, char);
struct QRecStruct* initQueue(int);
int deInitQueue(struct QRecStruct*);


