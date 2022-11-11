#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock;

struct Passenger
{
  int id;
  int from_floor;    // 1 to 7
  int to_floor;      // 1 to 7
  int in_elevator;   // 1 (in elevator_pick) or 0 (out elevator_pick)
  int type_elevator; // 1 or 2
} requests[10];

// function to create a passenger request
void *createPassenger(void *arg)
{ // passenger request
  pthread_mutex_lock(&lock);
  int i = *(int *)arg;
  int toFloor = (rand_r(arg) % 8) + 1;   // Floor they are going to
  int fromFloor = (rand_r(arg) % 8) + 1; // Floor they are coming from
  int randElevator = (rand() % 2) + 1;   // Which elevator they chose (1 or 2)
  struct Passenger *p = &requests[i];
  p->from_floor = fromFloor;
  p->to_floor = toFloor;
  p->type_elevator = randElevator;
  p->id = i;
  p->in_elevator = 0; // 0 (not in elevator_pick)  1 (in elevator_pick)
  requests[i] = *p;
  free(arg);
  pthread_mutex_unlock(&lock);
  return 0;
}

struct Passenger *sort_from_destination(struct Passenger p[], int n);
struct Passenger *sortDescending(struct Passenger p[], int n);
void elevator_pick(struct Passenger arr[], int n, int currentFloor, struct Passenger people_in_elevator[]);

int main()
{
  // fork the program into 2 process to simulate 2 elevators
  int id = fork();
  srand(time(NULL));
  struct Passenger people_in_elevator[5];
  int currentFloor = 1;             // starting floor
  int numThread = (rand() % 5) + 1; // random number of thread that will create passengers
  pthread_t threads[numThread];
  int i;
  for (i = 0; i < numThread; i++)
  {
    int *memoI = malloc(sizeof(int));
    *memoI = i;
    pthread_create(&threads[i], NULL, createPassenger, memoI);
  }
  for (int i = 0; i < numThread; i++)
  {
    pthread_join(threads[i], NULL);
  }
  if (id == 0) // child process to simulator on elevator
  {            // child process ELEVATOR = 1
    // printf("CHILD PROCESS - - \n");
    printf("ELEVATOR 1 - - \n");
    struct Passenger elevator1Req[5];
    int index = 0;
    /*
    iterate through the request array to get all passenger requests that belong to elevator 1
    */
    for (int i = 0; i < sizeof(requests) / sizeof(requests[0]); i++)
    {
      if (requests[i].type_elevator == 1)
      {
        elevator1Req[index] = requests[i];
        index++;
      }
    }
    printf("Number Of Passengers Requesting Elevator 1 - - - %d\n", index);
    /*
    sort the array so that the elevator will handle the passenger requests
    starting from the one with at the lowest floor going up
    */
    struct Passenger *orderedElevator = sort_from_destination(elevator1Req, index); // array itself is a pointer in c

    /*
    we pass the sorted array into the elevator function, that will handle all the passengers request
    */
    elevator_pick(orderedElevator, index, currentFloor, people_in_elevator);
  }
  else if (id != 0) // parent process that will simulator 2nd elevator
  {                 // parent process ELEVATOR = 2
    wait(NULL);
    // printf("IN PARENT PROCESS - - \n");
    printf("ELEVATOR 2 - - \n");
    struct Passenger elevator2Req[5];
    int index = 0;
    /*
    iterate through the request array to get all passenger requests that belong to elevator 2
    */
    for (int i = 0; i < sizeof(requests) / sizeof(requests[0]); i++)
    {
      if (requests[i].type_elevator == 2)
      {
        elevator2Req[index] = requests[i];
        index++;
      }
    }
    printf("Number Of Passengers Requesting Elevator 2 - - - %d\n", index);

    /*
    sort the array so that the elevator will handle the passenger requests
    starting from the one with at the lowest floor going up
    */
    struct Passenger *orderedElevator = sort_from_destination(elevator2Req, index); // array itself is a pointer in c

    /*
    we pass the sorted array into the elevator function, that will handle all the passengers request
    */
    elevator_pick(orderedElevator, index, currentFloor, people_in_elevator);
  }

  return 0;
}

/*
function that sorts the passenger array according to the destination floor they are at
*/
struct Passenger *sort_from_destination(struct Passenger p[], int n)
{
  struct Passenger temp;
  struct Passenger *ordered = malloc(sizeof(struct Passenger) * n);
  for (int i = 0; i < n - 1; i++)
  {
    for (int j = 0; j < n - i - 1; j++)
    {
      if (p[j].from_floor > p[j + 1].from_floor)
      {
        temp = p[j];
        p[j] = p[j + 1];
        p[j + 1] = temp;
      }
    }
  }
  ordered = p;

  return ordered;
}

/*
elevator function that handles all the passengers requests
*/
void elevator_pick(struct Passenger arr[], int n, int currentFloor, struct Passenger people_in_elevator[])
{
  printf("Number of REQUEST - - >  %d\n", n);
  int passenger = 0;
  for (int i = 0; i < n; i++)
  {
    printf("USER AT FLOOR %d\n", arr[i].from_floor);
    if (arr[i].from_floor == currentFloor)
    {
      printf("Floor %d Passenger - %d \n", currentFloor, i);

      sleep(1);
      printf("DOOR OPENED <-|  |-> \n");
      sleep(1);
      printf("PASSENGER GOT IN < - - -\n");
      sleep(1);
      printf("DOOR CLOSED ->||<- Passenger - %d\n", i + 1);
      arr[i].in_elevator = 1; // person in elevator
      currentFloor++;
      passenger++;
    }
    else
    {
      while ((arr[i].from_floor != currentFloor) && (currentFloor <= 8))
      {
        printf("Floor %d Passenger - %d \n", currentFloor, i);
        sleep(1);
        currentFloor++;
      }
      printf("Floor %d Passenger - %d \n", currentFloor, i);
      sleep(1);
      printf("DOOR OPENED <-|  |-> \n");
      sleep(1);
      printf("PASSENGER GOT IN < - - -\n");
      sleep(1);
      printf("DOOR CLOSED ->||<- Passenger - %d\n", i + 1);
      arr[i].in_elevator = 1; // person in elevator
      passenger++;
    }
  }
  printf("ELEVATOR STOP - - - \n");
  printf("\n");
}