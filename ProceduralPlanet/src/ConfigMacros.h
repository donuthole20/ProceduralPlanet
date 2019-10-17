#pragma once

#define THREADED 1

#define ENABLE_KEY_INPUT 0
#ifdef RELEASE
	#define ENABLE_LOG 0
#elif defined(DEBUG)
	#define ENABLE_LOG 1
#endif 
