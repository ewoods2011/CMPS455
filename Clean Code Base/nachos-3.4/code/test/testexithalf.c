#include <syscall.h>
int main(){
	Exec("../test/test7");
	Yield();
	Exec("../test/test7");
	Yield();
	Exit(0);
	}
