// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

//-----------------------GLOBAL VARIABLES-------------------------------
char *shouts[5] = {"Ham Sandwich", "Turkey Sandwich", "Tuna Sandwich", 
					"Chicken Sandwich", "Roast Beef Sandwich"};
int numShouts;
int numPhils;
int numPhilsEntered = 0;
int numPhilsSat = 0;
int numMeals;
bool **chopsticks;
Semaphore *mutex = new Semaphore("Mutex semaphore for Task4", 1);
Semaphore **semChops;

int P = 0;
int S = 0;
int M = 0;
int AllM;
char *name;
int messageRead = 0;
int messageSend = 0;
const int size = 3;
Semaphore **boxOffice;
bool **postOffice;
bool hasMail = false;


//----------------------------------------------------------------------
// InputType
//		A simple enumeration type that is used to hold and say what type
//		of input the user has given.
enum InputType {INT, DEC, CHAR, NEGDEC, NEGINT, UNEXPECTED};

//----------------------------------------------------------------------
// philState
//		A simple enumeration type that is used to hold and say what state
//		each philosopher is currently in.
enum philState {HUNGRY, EATING, THINKING};

philState *state;

//----------------------------------------------------------------------
// Task1
//		Implementation of the task 1 problem.
//		Analyzes and validates input, telling the user which category
//		his or her input falls under.
//		Creates a thread that forks on the ValidateInput function.
//----------------------------------------------------------------------
void Task1();

//----------------------------------------------------------------------
// ValidateInput
//		Create variables to hold user input and inputType
//		Call the EvaluateInput function to figure out what type of input
//		the user has given.
//
//		"threadNum" is simply to identify the thread which called this,
//		and may or may not be used.
//----------------------------------------------------------------------
void ValidateInput(int threadNum);

//----------------------------------------------------------------------
// IsLetter
//		Checks the passed character against every number character, '-',
//		and '.' to see if it is a letter or keyboard character that
//		would prevent it from being a number.
//----------------------------------------------------------------------
bool IsChar(char a);

//----------------------------------------------------------------------
// EvaluateInput
//		Determines what type of input the user has given.
//		Iterates through the length of input with the following steps:
//			Check if char IsLetter.
//				Return CHAR if so
//			Check if char is '-'
//				If it's the first character, mark possible neg
//				Else, return CHAR.
//			Check if char is '.'
//				If it's the second character and it's not possible neg,
//				mark possible decimal.
//				If it's the third character and it's possible neg,
//				makr possible decimal.
//				Else, return CHAR.
//			If it's gotten here, it must be a number, so just continue.
//
//			
//----------------------------------------------------------------------
InputType EvaluateInput(char * input);

//----------------------------------------------------------------------
// Task2
//		Implementation of Task 2 problem.
//		Prompts the user for a number of threads and a number of shouts
//		per thread.
//		Creates that many threads and forks them to the Shout funtion.
//----------------------------------------------------------------------
void Task2();

//----------------------------------------------------------------------
// Shout
//		Loop a given number of times, each time shouting a random
//		word from the shouts array.
//----------------------------------------------------------------------
void Shout(int threadNum);

//----------------------------------------------------------------------
// SimpleThread
// 		Loop 5 times, yielding the CPU to another ready thread 
//		each iteration.
//
//		"which" is simply a number identifying the thread, for debugging
//		purposes.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Task3
//		Implementation of Task 3 problem.
//		Prompts the user for a number of philosophers and a number of meals.
//		Creates an array of chopsticks to use; one per philosopher

//		Creates a thread for each philosopher and forks them to the Dine function.
//		
//----------------------------------------------------------------------
// Edward Woods & Jason Woodworth worked on Tasks 1 & 2
// Evan Perry & Troy Stockman worked on Tasks 3 & 4


//ebw3559 & jww7675
void task34Input(int taskNum);

void Dine(int threadNum);

void think(int i);
void eat(int i);

void take_chopsticks_BW(int i);
void put_chopsticks_BW(int i);
void test_BW(int i);

void take_chopsticks(int i);
void put_chopsticks(int i);
void test(int i);

//erp1067
void task56Input(int taskNum);
void Post1(int threadNum);
void readingMail(int personNum);
void composeMail(int personNum, bool hereAgain);
void Busy(int personNum, bool hereAgain);
void EnterPost(int threadNum, bool hereAgain);
void LeavePost(int threadNum, bool hereAgain);
//tas6329
void Post2(int threadNum);
void ReadMail(int num);
void ComposeMessage(int num);
void LeavingPost(int threadNum);
void Entering(int threadNum);

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
		printf("*** threads %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}


//----------------------------------------------------------------------
// ThreadTest
// 		Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");
	
	if (EvaluateInput(taskToDo) != INT) {
		printf("***Error, improper input or no -A command found\n");
		currentThread->Finish();
	}

	//Decide which task
	if (atoi(taskToDo) == 1)
		Task1();
	else if (atoi(taskToDo) == 2)
    	Task2();
	else if (atoi(taskToDo) == 3)
	task34Input(3);
	else if (atoi(taskToDo) == 4)
	task34Input(4);
	else if (atoi(taskToDo) == 5)
		task56Input(5);
	else if (atoi(taskToDo) == 6)
		task56Input(6);
    else {
    	printf("***Error, improper input or no -A command found\n");
    	currentThread->Finish();
    }
}

//-------------------------------------------------------------
//--------------------TASK 1-----------------------------------
//-------------------------------------------------------------
void
Task1()
{
	Thread *t = new Thread("Task 1 Thread");
	//Execute the new thread with the ValidateInput()
	t->Fork(ValidateInput, 1);
}

//-------------------------------------------------------------
//--------------------VALIDATE INPUT---------------------------
//-------------------------------------------------------------
void
ValidateInput(int threadNum)
{
	char * buffer = new char[256];
	InputType iType;
	
	//Inform the user how to enter input.
	printf("***Guidelines For Entering Information - \n");
	printf("***Enter any type of information to determine its type.\n");
	printf("***All leading and trailing whitespaces will be taken out.\n");
	printf("***Enter '/' to terminate the loop.\n");
	
	while (buffer[0] != '/') {
	//Get input and evaluate it
	printf("***Enter information ('/' to stop): ");
	fgets(buffer, 256, stdin);
	
	//Strip off leading whitespace to nip that in the bud.
	while (buffer[0] == ' ')
		buffer++;
	

	iType = EvaluateInput(buffer);
	
	
	
	printf("***In thread %d.  %s is of type: ", threadNum, buffer);
	switch(iType) {
		case INT:
			printf("INTEGER");
			break;
		case DEC:
			printf("DECIMAL");
			break;
		case CHAR:
			printf("CHARACTER");
			break;
		case NEGDEC:
			printf("NEGATIVE DECIMAL");
			break;
		case NEGINT:
			printf("NEGATIVE INTEGER");
			break;
		case UNEXPECTED:
			printf("YOU DIDN'T ENTER INPUT");
			break;
		default:
			printf("What did you do");
			break;
	}
	printf("\n\n");
	
	}
	
	currentThread->Finish();

}

//-------------------------------------------------------------
//-------------------EVALUATE INPUT----------------------------
//-------------------------------------------------------------
InputType EvaluateInput(char * input) {
	//Bool types to check possibilities
	bool maybeDecimal = false;
	bool maybeNegative = false;
	
	
	//Strip off trailing newLines or spaces, if any.
	while (input[strlen(input) - 1]  == '\n' || input[strlen(input) - 1] == ' ') {
		input[strlen(input) - 1] = '\0';
	}
	
	
	//First handle if there was just no input
	//Since we've just trimmed it, the size would be 0
	if (strlen(input) == 0)
		return UNEXPECTED;
	
	//Begin evaluating and iterating through the input
	for (int i = 0; (int)i < (int)strlen(input); i++) {
		//Check if the current char is a character
		//If at any time we find a character, it's a string
		if (IsChar(input[i]))
			return CHAR;
		//Check if current char is a - sign
		//If at the first position, it might be negative.
		//If at anywhere else, it just makes it a string
		else if (input[i] == '-') {
			if (i == 0 && strlen(input) > 1)
				maybeNegative = true;
			else
				return CHAR;
		}
		//Check if current char is a .
		//If it already may be a decimal, then we've already hit
		// a ., thus there are 2 and we know it's a character
		//If the . is at either the first or last position, we
		// know it is a character
		//If the . is in the second position, but we know it may
		// be negative ("-.xxxx"), we know it is a character.
		//Else, we can assume the . is in the middle somewhere,
		// and can set maybeDecimal to true.
		else if (input[i] == '.') {
			//Already hit a .
			if (maybeDecimal)
				return CHAR;
			//. is at first or last position
			else if (i == 0 || i == (int)strlen(input) - 1)
				return CHAR;
			//"-.xxxxx"
			else if (i == 1 && maybeNegative)
				return CHAR;
			//It's assumed in the middle and first occurrence
			else
				maybeDecimal = true;
		}
		
		//If we've reached this part of the code, we are sure this
		//character is a number.  Thus we can continue.
	}
	//If we've reached this point, we have eliminated all chance
	//of it being a char.  So we must figure out what kind of number.
	if (maybeNegative && maybeDecimal)
		return NEGDEC;
	else if (maybeNegative)
		return NEGINT;
	else if (maybeDecimal)
		return DEC;
	else
		return INT;
		
	//If we've gotten to this point, then something weird is up.
	return UNEXPECTED;
}

//-------------------------------------------------------------
//--------------------IS CHAR----------------------------------
//-------------------------------------------------------------
bool IsChar(char a) {
	//Check a against every single number, and - and .
	if (a != '0' && a != '1' && a != '2' && a != '3' &&
		a != '4' && a != '5' && a != '6' && a != '7' &&
		a != '8' && a != '9' && a != '-' && a != '.')
		return true;
	else
		return false;

}

//-------------------------------------------------------------
//--------------------TASK 2-----------------------------------
//-------------------------------------------------------------
void Task2() {
	char * buffer = new char[256];
	int numThreads = 0;
	
	//Collect Input
	printf("***Enter desired number of threads: ");
	fgets(buffer, 256, stdin);
	
	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	}
	
	
	//Convert Number of Threads
	numThreads = atoi(buffer);
	
	//Collect Input
	printf("***Enter desired number of shouts per thread: ");
	fgets(buffer, 256, stdin);
	
	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	} 
	
	
	//Convert Number of Shouts
	numShouts = atoi(buffer);
	
	//Set up our loop to fork numThreads
	Thread * t;
	for (int i = 0; i < numThreads; i++) {
		t = new Thread("Task 2 Thread");
		t->Fork(Shout, i);
	}
	
	
	currentThread->Finish();
}

void Shout(int threadNum) {
	int shout;
	int waitUntil;
	int waitCounter;
	for (int i = 0; i < numShouts; i++) {
		shout = Random() % 5; //Random b/w 0-4
		waitUntil = (Random() % 4) + 2; //Random b/w 2-5
		waitCounter = 0;
		
		printf("***Thread %d shouting: %s \n\n", threadNum, shouts[shout]);
	
		//Busy waiting loop
		//Keep yielding until the waitCounter hits the waitUntil
		while (waitCounter < waitUntil) {
			currentThread->Yield();
			waitCounter++;
		}
		
	}
	
	currentThread->Finish();
}

//-------------------------------------------------------------
//--------------------Input for Tasks 3 & 4--------------------
//-------------------------------------------------------------
void task34Input(int taskNum){
	char * buffer = new char[256];

	//Collect Input
	printf("***Enter desired number of philosophers: ");
	fgets(buffer, 256, stdin);

	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	}


	//Convert Number of Philosophers
	numPhils = atoi(buffer);
	state = new philState[numPhils];

	//Collect Input
	printf("***Enter desired number of meals to eat: ");
	fgets(buffer, 256, stdin);

	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	} 


	//Convert Number of Meals
	numMeals = atoi(buffer);

	printf("\n\n");

	//Set up our loop to fork numPhils
	if(taskNum == 3){
		//Create an array of chopsticks with one per philosopher
		chopsticks = new bool*[numPhils];
		for(int i = 0; i < numPhils; i++)
		{
			chopsticks[i] = new bool[numPhils];
		} 

		Thread * t;
		for (int i = 0; i < numPhils; i++) {
			t = new Thread("Task 3 Thread");
			t->Fork(Dine, i);
		}
	}
	else
	{	
		//Create an array of chopsticks with one per philosopher
		semChops = new Semaphore*[numPhils];
		for(int i = 0; i < numPhils; i++)
		{
			semChops[i] = new Semaphore("New Semaphore for a chopstick", 0);
		} 

		Thread * t;
		for (int i = 0; i < numPhils; i++) {
			t = new Thread("Task 4 Thread");
			t->Fork(Dine, i);
		}

	}


	currentThread->Finish();
}


//-------------------------------------------------------------
//--------------------DINE for Task3 & Task4-------------------
//-------------------------------------------------------------
void Dine(int which)
{
	//Current Philosopher enters the room
	numPhilsEntered++;
	printf("***Philosopher %i has entered the room. \n", which);
	while(numPhilsEntered < numPhils)
		currentThread->Yield();

	//Current Philosopher sits at the table
	numPhilsSat++;
	printf("***Philosopher %i has sat down \n", which);
	while(numPhilsSat < numPhils)
		currentThread->Yield();

	while(true){
		if(numMeals > 0){
			//Checks which task to do
			if(atoi(taskToDo) == 3)
				take_chopsticks_BW(which);
			else if(atoi(taskToDo) == 4)
			{
				take_chopsticks(which);
				eat(which);
				put_chopsticks(which);
				think(which);	
			}
		}
		else{
			numPhilsSat--;
			printf("***Philosopher %i is ready to go. \n", which);
			while(numPhilsSat > 0)
				currentThread->Yield();

			printf("***Philosopher %i has left. \n", which);
			currentThread->Finish();
		}
	}

}

void think(int i)
{
	int waitUntil;
	int waitCounter;
	//Thinking for 2-5 cycles
	waitUntil = (Random() % 4) + 2; //Random b/w 2-5
	waitCounter = 0;

	state[i] = THINKING;
	printf("***Philosopher %i has started thinking. \n", i);

	//Busy waiting loop
	//Keep yielding until the waitCounter hits the waitUntil
	while (waitCounter < waitUntil) 
	{
		currentThread->Yield();
		waitCounter++;
	}

}

void eat(int i)
{
	int waitUntil;
	int waitCounter;
	//Eating for 2-5 cycles
	waitUntil = (Random() % 4) + 2; //Random b/w 2-5
	waitCounter = 0;

	//Busy waiting loop
	//Keep yielding until the waitCounter hits the waitUntil
	while (waitCounter < waitUntil) 
	{
		currentThread->Yield();
		waitCounter++;
	}

	printf("***Philosopher %i has satiated his tum. \n", i);

}

void take_chopsticks_BW(int i)
{
	state[i] = HUNGRY;
	test_BW(i);
	put_chopsticks_BW(i);
}

void put_chopsticks_BW(int i)
{
	if(chopsticks[i][(i + 1) % numPhils] == true){
		//Current Philosopher puts down both chopsticks
		chopsticks[i][(i + 1) % numPhils] = false;
		printf("***Philosopher %i has put down Left Chopstick. \n", i);
		printf("***Philosopher %i has put down Right Chopstick. \n", i);
		think(i);
	}

	test_BW((i + numPhils - 1) % numPhils);
	test_BW((i + 1) % numPhils);

}

void test_BW(int i)
{
	if(state[i] == HUNGRY && 
		state[((i + numPhils - 1) % numPhils)] != EATING &&
		state[((i + 1) % numPhils)] != EATING){
			if(chopsticks[i][(i + 1) % numPhils] == false && 
				chopsticks[((i + numPhils - 1) % numPhils)][i] == false && 
				chopsticks[(i + 1) % numPhils][(i + 2) % numPhils] == false)
			{
				chopsticks[i][(i + 1) % numPhils] = true;

				printf("***Philosopher %i has picked up Left Chopstick. \n", i);
				printf("***Philosopher %i has picked up Right Chopstick. \n", i);
				printf("***Philosopher %i has started eating \n", i);
				numMeals--;
				printf("***%i meals left \n", numMeals);
				eat(i);
			}
			else{
				while(chopsticks[i][(i + 1) % numPhils] == true)
					currentThread->Yield();
			}
	}
}

void take_chopsticks(int i)
{
	mutex->P();
	state[i] = HUNGRY;
	test(i);
	mutex->V();
	semChops[i]->P();

}

void put_chopsticks(int i)
{
	printf("***Philosopher %i has put down Left Chopstick. \n", i);
	printf("***Philosopher %i has put down Right Chopstick. \n", i);
	mutex->P();
	state[i] = THINKING;
	test((i + numPhils - 1) % numPhils);
	test((i + 1) % numPhils);
	mutex->V();


}

void test(int i)
{
	if(state[i] == HUNGRY && 
		state[((i + numPhils - 1) % numPhils)] != EATING &&
		state[((i + 1) % numPhils)] != EATING && 
		numMeals > 0)
	{

		state[i] = EATING;
		semChops[i]->V();
		printf("***Philosopher %i has picked up Left Chopstick. \n", i);
		printf("***Philosopher %i has picked up Right Chopstick. \n", i);
		printf("***Philosopher %i has started eating. \n", i);
		numMeals--;
		printf("***%i meals left \n", numMeals);

	}

}


//-------------------------------------------------------------
//--------------------Input for Tasks 5 & 6--------------------
//-------------------------------------------------------------

void task56Input(int taskNum)
{
	//P= number of people
	//S= number of messages mailbox can hold
	//M= number of messages to be sent

	char * buffer = new char[256];
	
	//Collect Input
	printf("***Enter desired number of participating people: ");
	fgets(buffer, 256, stdin);
	
	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	}
	// Convert input to number of people
	P = atoi(buffer);
	
	//Collect Input
	printf("***Enter desired number of messages in a person's mailbox: ");
	fgets(buffer, 256, stdin);
	
	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	}
	// Convert input to number of messages mailbox can hold
	S = atoi(buffer);

	//Collect Input
	printf("***Enter desired number of messages to be sent: ");
	fgets(buffer, 256, stdin);
	
	if (EvaluateInput(buffer) != INT) {
		printf("***Input must be in integer format.\n\n");
		currentThread->Finish();
	}
	// Convert input to number messages to be sent
	M = atoi(buffer);

	printf("\n\n");
	
	//Set up our loop to fork Number of People in Simulation
	if(taskNum == 5){
		//Create an array 
		postOffice = new bool*[P];
		for(int i = 0; i < P; i++)
		{
			postOffice[i] = new bool[S];
		} 
		messageSend = M - 1;
		Thread * t;
		for (int i = 0; i < P; i++) {
			t = new Thread("Task 5 Thread");
			t->Fork(Post1, i);
		}
	}
	else
	{	
		boxOffice = new Semaphore*[P];
		for(int j = 0; j < P; j++)
		{
			boxOffice[j] = new Semaphore(name, S);
		}
		
		messageSend = M;

		Thread * t;
		for (int i = 0; i < P; i++) {
			t = new Thread("Task 6 Thread");
			t->Fork(Post2, i);
		}
	}
	
	currentThread->Finish();
}

//-------------------------------------------------------------
//--------------------POST1 for Task 5-------------------------
//-------------------------------------------------------------

void Post1(int which)
{
	bool hereAgain = false;
	EnterPost(which, hereAgain);
	LeavePost(which, hereAgain);
	
	
}

void readingMail(int personNum)
{

	for (int i = 0; i < S; i++)
	{
		if (postOffice[personNum][i] == true)
		{
			printf("Person %d read message from Person %i.\n", personNum, i);
			postOffice[personNum][i] = false;
			currentThread->Yield();
			messageRead++;
		}
		
	}
	hasMail = false;
}

void composeMail(int personNum, bool hereAgain)
{
	int receiver = Random() % P; // Random recipient of the message
		// Find a new recipient if the Random recipient happens to be the sender
	while (receiver == personNum)
		receiver = Random() % P;
	
	int mailPosition = 0;
	if(hereAgain == false)
	{
		while (postOffice[receiver][mailPosition] == true && mailPosition < S)
		{
				mailPosition++;
		}
	}
	
	if (postOffice[receiver][mailPosition] == false)
	{
		// message has been sent to recipient
		postOffice[receiver][mailPosition] = true;
		printf("Person %d compose message to Person %i.\n", personNum, receiver);

		hasMail = true;

		messageSend = messageSend - 1;
	}
	else
	{
		
		if(hereAgain == false)
		{   
			// Call BusyWaiting Loop
			Busy(personNum, hereAgain);
		}
		else
		{
			readingMail(personNum);
			hereAgain = false;
		}
	}
		
}

void Busy(int personNum, bool hereAgain)
{
	int waiting = 0;
	while(waiting < 3){
		currentThread->Yield();
		waiting++;
	}
	hereAgain = true;
	composeMail(personNum, hereAgain);

}

void EnterPost(int threadNum, bool hereAgain)
{
	printf("Person %i entered the Post Office.\n", threadNum);
	if (hasMail == true)
	{
		readingMail(threadNum);
	}
	else
	{
		if(messageSend > 0)
			{composeMail(threadNum, hereAgain);}
	}
	
	LeavePost(threadNum, hereAgain);

}

void LeavePost(int threadNum, bool hereAgain)
{
	int waitUntil, waitCounter;
	printf("Person %i left the Post Office.\n", threadNum);
	waitUntil = (Random() % 4) + 2;
	waitCounter = 0;
	while (waitCounter < waitUntil)
	{
		currentThread->Yield();
		waitCounter++;
	}
	if(messageSend == 0)
	{
		currentThread->Finish();
	}

		
		EnterPost(threadNum, hereAgain);
}

//-------------------------------------------------------------
//--------------------POST2 for Task 6-------------------------
//-------------------------------------------------------------

void 
ReadMail(int num)
{
	if(boxOffice[num]->getValue() <  S)
	{	
		printf("Person %d read a mail from his mail box\n", num);
		boxOffice[num]->V();
		currentThread->Yield();
		messageRead++;
		ReadMail(num);
	}
}

void
ComposeMessage(int num)
{	
	if ((messageSend-1) != -1)
	{
		int random = Random() % P;
		while(random == num)
		{
			random = Random() % P;
		}
	
		boxOffice[random]->P();
		printf("Person %d compose a message for Person %d\n", num, random);
		
		messageSend = messageSend - 1;
	}
	
}

void
LeavingPost(int threadNum)
{
	printf("Person %d has left the post office\n", threadNum);
	int random = (Random() % 4) + 2;
	while(random != 0)
	{
		currentThread->Yield();
		random--;
	}
	
	if(messageSend == 0 && messageRead == M)
	{
		currentThread->Finish();
	}

	if((messageRead < M) || (messageSend > 0))
	{	
		Entering(threadNum);
	}
}

void
Entering(int threadNum)
{
	printf("Person %d has enter the post office.\n", threadNum);
	
	if(boxOffice[threadNum]->getValue() < S)
	{
		ReadMail(threadNum);
	}
	else
	{
		ComposeMessage(threadNum);
	}
	
	LeavingPost(threadNum);
}

void
Post2(int threadNum)
{
	Entering(threadNum);
}

