#include "dpsim.h"
#include <signal.h> //pthread_kill()

static const unsigned int NUM_PHILOSPHERS = 5;
static const unsigned int NUM_CHOPSTICKS = 5;

static int chopsticks[5]; 
static pthread_mutex_t mutex[5];
static pthread_t philosphers[5];


void* th_main( void* th_main_args ) {

	// ---------------------------------------
	// TODO: you add your implementation here

	int tempCnt[5];
	int i;

	for(i = 0; i < NUM_CHOPSTICKS; i++)
	{
		chopsticks[i] = -1;
	}	

	tempCnt[0] = 0;
	
	for(int x = 1; x < NUM_PHILOSPHERS; x++)
	{
		tempCnt[x] = tempCnt[x - 1] + 1;
	}

	for(int j = 0; j < NUM_PHILOSPHERS; j++)
	{
		if(pthread_create(&philosphers[j], NULL, (void *) th_phil, &tempCnt[j]) != 0)
		{
			perror("Creation Error!\n");
			exit(1);
		}
	}

	int running = TRUE;	
	
	while(running)
	{
		int temp[5];
		memcpy(temp, chopsticks, 5 * sizeof(int));
		int noDeadlock = 0;
		int philDiners = 0;

		for(int k = 0; k < NUM_CHOPSTICKS; k++)
		{
			if(temp[k] == -1)
				noDeadlock++;

			else if(temp[(k + 1) % 5] == temp[k])
			{
				philDiners++;
				noDeadlock++;
			}
		}

		if(noDeadlock == 0)
		{
			printf("Deadlock condition (0, 1, 2, 3, 4) ... terminating");
			running = 0;
		}

		else if(philDiners > 0)
		{
			printf("Philospher(s) ");

			for(int l = 0; l < NUM_CHOPSTICKS; l++)
			{
				if ((temp[l] == temp[(l + 1) % 5]) && (temp[l] != -1))
				{
					printf("%d ", l);
					if (philDiners > 1)
						philDiners--;
				}
			}

			printf("are eating!\n");
		}

		delay(10000);
	}	


	for(i = 0; i < NUM_PHILOSPHERS; i++)
	{
		pthread_kill(philosphers[i], 0);
	}

	pthread_exit(0);

} // end th_main function


void* th_phil( void* th_phil_args ) {

	// ---------------------------------------
	// TODO: you add your implementation here

	int philospherID = *(int*) (th_phil_args);
	
	while(1)
	{
		delay(10000);
		eat(philospherID);
	}

} // end th_phil function


void delay( long nanosec ) {

	struct timespec t_spec;

	t_spec.tv_sec = 0;
	t_spec.tv_nsec = nanosec;

	nanosleep( &t_spec, NULL );

} // end think function


void eat( int phil_id ) {

	// ---------------------------------------
	// TODO: you add your implementation here

	//pick up right chopstick
	pthread_mutex_lock(&mutex[phil_id]);
	chopsticks[phil_id] = phil_id;

	//pick up left chopstick
	pthread_mutex_lock(&mutex[(phil_id + 1) % 5]);
	chopsticks[(phil_id + 1) % 5] = phil_id;

	delay(10000);

	//put down left chopstick
	pthread_mutex_unlock(&mutex[(phil_id + 1) % 5]);
	chopsticks[(phil_id + 1) % 5] = -1;

	//put down right chopstick
	pthread_mutex_unlock(&mutex[phil_id]);
	chopsticks[phil_id] = -1;

} // end eat function
