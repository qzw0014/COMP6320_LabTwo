// C program for array implementation of queue 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <limits.h>
#include <pthread.h> 
#include <time.h>

int finish = 0;     //Packets Assignment Finish Flag

struct ThreadOneParams
{
    int select; 
    double param;
    struct Queue* q1;
    struct Queue* q2;
};

struct ThreadOneParams* newOneParams (int select, double param, struct Queue *q1, struct Queue *q2)
{
    struct ThreadOneParams *temp = (struct ThreadOneParams*)malloc(sizeof(struct ThreadOneParams));
    temp->select = select;
    temp->param = param;
    temp->q1 = q1;
    temp->q2 = q2;
}

struct ThreadTwoParams
{
    struct Queue *q;
    double u;
};

struct ThreadTwoParams* newTwoParams (double u, struct Queue *q)
{
    struct ThreadTwoParams *temp = (struct ThreadTwoParams*)malloc(sizeof(struct ThreadTwoParams));
    temp->u = u;
    temp->q = q;
}

struct Packet
{
    int key;
    time_t start_time;
    time_t service_time;
    struct Packet *next;
};

// A utility function to create a new packet. 
struct Packet* newPacket(int k) 
{ 
    struct Packet *temp = (struct Packet*)malloc(sizeof(struct Packet)); 
    temp->key = k; 
    temp->next = NULL; 
    return temp;  
} 

struct Queue 
{ 
    struct Packet *front, *rear;
    int size; 
};
  
// A utility function to create an empty queue 
struct Queue *createQueue() 
{ 
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL;
    q->size = 0; 
    return q; 
} 
  
// The function to add a key k to q 
int enQueue(struct Queue *q, int k) 
{ 
    if (q->size < 10) 
    {
        struct Packet *temp = newPacket(k);
        temp->start_time = time(NULL);

        // If queue is empty, then new node is front and rear both 
        if (q->rear == NULL) 
        { 
            q->front = q->rear = temp;
            q->size = q->size + 1; 
            return 0; 
        } 
    
        // Add the new node at the end of queue and change rear 
        q->rear->next = temp; 
        q->rear = temp;
        q->size++;
        return 0;
    }
    else 
    {
        return 1;
    } 
} 

// Function to remove a key from given queue q 
struct Packet *deQueue(struct Queue *q) 
{ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
       return NULL; 
    // Store previous front and move front one node ahead 
    struct Packet *temp = q->front; 
    q->front = q->front->next; 
    q->size = q->size - 1; 
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
       q->rear = NULL;
    // Set service time
    time_t end_time = time(NULL);
    temp->service_time = end_time - temp->start_time;
    return temp; 
} 

// Function to generate exponential distribution number
double exponGenerator(double myLamda)
{
    double pv = 0.0;
	while (1)
	{
		pv = (double)rand() / (double)RAND_MAX;
		if (pv != 1)
			break;
	}
	pv = (-1.0 / myLamda) * log(1 - pv);
	return pv;
}

void *packetAssignment(struct ThreadOneParams *paramOne)
{
    double blockCounter = 0.0;
    double blockPro = 0.0;
    double queueLenCounter = 0.0;
    double averageQueueLen = 0.0;
    double waitingTimeCounter = 0.0;
    double averageWaiting = 0.0;
    int i = 0;
    if (paramOne->select == 0) 
    {
        for (i = 0; i < 10000; i++)
        {
            double interval = exponGenerator(paramOne->param);
            usleep(interval * 1000000);
            int res = randomSelection(paramOne->q1, paramOne->q2);
            if (res == 1) 
            {
                blockCounter++;
            }
        }
    }
    else if (paramOne->select == 1)
    {
        for (i = 0; i < 10000; i++)
        {
            double interval = exponGenerator(paramOne->param);
            sleep(interval);
            int res = minQueue(paramOne->q1, paramOne->q2);
            if (res == 1) 
            {
                blockCounter++;
            }
        }
    }
    blockPro = blockCounter/10000;
    finish = 1;
    return;
}

void *startServer(struct ThreadTwoParams *paramTwo) 
{
    while(1)
    {
        if (finish == 1) 
        {
            break;
        }
        else
        {
            double interval = exponGenerator(paramTwo->u);
            usleep(interval * 1000000);
            struct Packet *k = deQueue(paramTwo->q);
        }
    }
}

// Main function
int main(int argc, char * argv[]) 
{
    int i = 0;
    if (argc != 3)
    {
        printf("Incorrect parameters!\n");
        return 0;
    }
    double param1 = atof(argv[1]);
    double param2 = atof(argv[2]);
    for (i = 0; i < 1; i++)
    {
        struct Queue *q1 = createQueue();
        struct Queue *q2 = createQueue();
        struct ThreadOneParams *oneparams = newOneParams(0, param1, q1, q2);
        struct ThreadTwoParams *twoparams = newTwoParams(param2, q1);
        struct ThreadTwoParams *threeparams = newTwoParams(param2, q2);
        pthread_t thread1, thread2, thread3;
        pthread_create(&thread2, NULL, startServer, twoparams);
        pthread_create(&thread3, NULL, startServer, threeparams);
        pthread_create(&thread1, NULL, packetAssignment, oneparams); 
    }
}