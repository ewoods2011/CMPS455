#include "syscall.h"

void
forkTo(){
};

int
main()
{
	int i = 0;
	Fork(forkTo);
	Write("Past the Fork!",16,1);
	Exit(0);
}