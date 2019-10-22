#pragma once

#define THREADED 1
#define GL_ENABLE_ERROR_CHECK 0//Note: Enabling this is slow
#define ENABLE_INPUT 0
#ifdef RELEASE
	#define ENABLE_LOG 0
#elif defined(DEBUG)
	#define ENABLE_LOG 1
#endif 
