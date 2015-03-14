#include "syscall.h"

int
main()
{
    SpaceId aSpaceId = Exec("../test2/blank");
	SpaceId aSpaceId2 = Exec("../test2/blank");

	if (aSpaceId2 == 0) /* the id passed back should be between 0 and 3 */
		Write("SpaceId2 = 0\n", 13, ConsoleOutput);
	else if (aSpaceId2 == 1)
		Write("SpaceId2 = 1\n", 13, ConsoleOutput);
	else if (aSpaceId2 == 2)
		Write("SpaceId2 = 2\n", 13, ConsoleOutput);
	else if (aSpaceId2 == 3)
		Write("SpaceId2 = 3\n", 13, ConsoleOutput);

	Exit(0);
}