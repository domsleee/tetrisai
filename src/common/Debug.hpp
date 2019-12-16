#pragma once

#ifdef MY_DEBUG
#define DEBUG_WEIGHTING(__VA_ARGS__) printf(__VA_ARGS__)
#else
#define DEBUG_WEIGHTING(__VA_ARGS__) 
#endif