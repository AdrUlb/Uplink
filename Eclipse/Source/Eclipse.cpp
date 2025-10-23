#include "Eclipse/Eclipse.hpp"

#include <sys/time.h>

double EclGetAccurateTime()
{
	static bool initted = false;
	static timeval startTime;

	if (initted)
	{
		timeval thisTime { };
		gettimeofday(&thisTime, nullptr);

		const auto sec = thisTime.tv_sec - startTime.tv_sec;
		const auto usec = thisTime.tv_usec - startTime.tv_usec;
		return (sec * 1000.0) + (usec / 1000.0);
	}

	initted = true;
	gettimeofday(&startTime, nullptr);
	startTime.tv_usec = 0;
	return 0;
}
