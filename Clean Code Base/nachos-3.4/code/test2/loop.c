#include "syscall.h"

int main()
{
	Exec("../test_orig/loop");
	Yield();
	Exec("../test_orig/loop");
	Yield();
	Exec("../test_orig/loop");
	Yield();
}