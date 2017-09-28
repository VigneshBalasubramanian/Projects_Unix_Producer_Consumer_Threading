// CS590 FINAL PROGRAMMING ASSIGNMENT //
// VIGNESH BALASUBRAMANIAN //
// PRODUCER CONSUMER PROBLEM //
// Reference: 
// [1] Advanced Programming in the UNIX Environment, 3rd Edition By  W. Richard Stevens and  Stephen A. Rago //
// [2] http://www.cs.utsa.edu/~wagner/CS3343/rec1/queue.html
// [3] http://stackoverflow.com/questions/20619234/circular-queue-implementation //
// [4] https://macboypro.wordpress.com/2009/05/25/producer-consumer-problem-using-cpthreadsbounded-buffer/ //	      
// [5] http://www.mathcs.emory.edu/~cheung/Courses/171/Syllabus/8-List/array-queue2.html/ //
// [6] http://apiexamples.com/c/stdlib/rand_r.html //

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <time.h>

#define Q_max 100
#define Q_min 0

// Initializing mutex variables//
pthread_mutex_t lock_thread = PTHREAD_MUTEX_INITIALIZER;

// Initializing condition variable for producer and consumer//
pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_cond = PTHREAD_COND_INITIALIZER;

// Function declaration for consumer and producer threads //
void *producer_thread(void *prod_var);
void *consumer_thread(void *cons_var);

// Declaration of array for the queue //
int Q_Array[Q_max];

// Initialization of the flag variables to check the queue's limit for Producer and Consumer//
int queue_head = -1;
int queue_tail = -1;

// Declaration of the random value generator variable //
unsigned int rValue;

// Initialization of the variable to track the slot for Producer and Consumer//
int queue_tracker = 0;

// Function definition of Producer segment //

void *producer_thread(void *prod_var)
{	
	// Producer adds entry to the queue //

	for(;;)
	{

		// For random value generation //

		srand(rValue);		
		int Q_R_Entry = rand_r(&rValue) % 1000;
		int Q_Entry = Q_R_Entry + 1;
		rValue *= 2;

		// Re-randomizing the random value generator variable to make it a bit more random //
		rValue = (unsigned int)time(NULL);

		// Declaration of thread IDs for producer//
		pthread_t tidp_prod;

		// Locking the mutex //
		pthread_mutex_lock(&lock_thread);


		// Checking if the queue is full before queuing;  100 being the given queue capacity in the requirement  //

		if(queue_tracker == Q_max)
		{
			printf("\n The Queue is full. Cannot add an entry now.\n");
			pthread_cond_wait(&p_cond, &lock_thread); // Putting producer to wait since producer shouldn't add to a full queue //
		}
		else
		{
			queue_tracker++;				   // Advancing the count by 1 //
			queue_tail = (queue_tail+1)%Q_max; // Incrementing to next slot after adding an entry to the queue //
			Q_Array[queue_tail] = Q_Entry;	   // Enqueueing //
			printf("\nProducer %lu added %d to slot %d\n",(unsigned long)&tidp_prod, Q_Entry, queue_tail);  // unsigned long because the tid is of unsigned long type //
		}

		// Unlocking the mutex //	
		pthread_mutex_unlock(&lock_thread);
		// Signalling the consumer //
		pthread_cond_signal(&c_cond);

		// Random value generation for sleep between 1 to 3 seconds //
		int random_Value = rand()%4;
		sleep(random_Value);
	}

	//Thread exit //
	pthread_exit(0);

}

// Function definition of Consumer segment //

void *consumer_thread(void *cons_var)
{


	if(queue_head == -1)
	{
		queue_head = 0;

	}
	// Consumer picks out entry from the queue //

	for(;;)
	{

		// Locking the mutex //
		pthread_mutex_lock(&lock_thread);


		// Declaration of thread IDs for consumer //
		pthread_t tidp_cons;


		if (queue_head == Q_max)
		{
			queue_head = 0;
		}

		// Checking if the queue is empty before dequeuing //
		if(queue_tracker == 0)
		{
			printf("\n The Queue is empty. Cannot pick up entry from an empty queue.\n");
			pthread_cond_wait(&c_cond, &lock_thread);   // Putting consumer to wait since consumer shouldn't add to a full queue //
		}

		else
		{
			queue_tracker--;				 // Decrementing the count by 1 //
			printf("\nConsumer %lu removed %d from slot %d\n",(unsigned long)&tidp_cons, Q_Array[queue_head], queue_head);	 // unsigned long because the tid is of unsigned long type; also printing the headmost value i.e., in a way dequeueing //
			Q_Array[queue_head] = 0;
			queue_head++;   // Incrementing after picking out an entry from the queue //		
		}

		// Unlocking the mutex //	
		pthread_mutex_unlock(&lock_thread);
		// Signalling the producer //
		pthread_cond_signal(&p_cond);

		// Random value generation for sleep between 1 to 3 seconds //		
		int random_Value = rand()%4;
		sleep(random_Value);
	}

	//Thread exit //

	pthread_exit(0);
}


// Main function //

int main()
{

	// Declaration of producer thread count, consumer thread count and loop variable //
	int pno, cno, i;

	// Declaration of producer and consumer thread return pointers //
	void *pro;
	void *con;

	// Initialization of the random value generator variable //
	rValue = (unsigned int)time(NULL);

	// Inputting the number of producer and consumer threads from the user //
	printf("\nEnter the number of producer threads to be created:    ");
	scanf("%d",&pno);
	printf("\nEnter the number of consumer threads to be created:    ");
	scanf("%d",&cno);

	// Variable delaration of thread creation //
	pthread_t tidp_1[pno]; // Producer thread ID creation //
	pthread_t tidp_2[cno]; // Consumer thread ID creation //

	// Calling the producer function for user entered number of times //
	for(i=0; i < pno; i++)
	{
		pthread_create(&tidp_1[i], 0, producer_thread, NULL);  // The attribute pthread_attr_t *attr = 0  //
	}

	// Calling the consumer function for user entered number of times //

	for(i=0; i < cno; i++)
	{
		pthread_create(&tidp_2[i], 0, consumer_thread, NULL);  // The attribute pthread_attr_t *attr = 0  //
	}

	// Cleaning the mutex using Join functions//

	for(i=0; i < pno; i++)
	{
		pthread_join(tidp_1[i], &pro);
	}

	for(i=0; i < cno; i++)
	{
		pthread_join(tidp_2[i], &con);
	}

	// Destroying the mutex and conditional variables //
	pthread_mutex_destroy(&lock_thread);
	pthread_cond_destroy(&c_cond);
	pthread_cond_destroy(&p_cond);

}
