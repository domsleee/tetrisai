#pragma once

#ifdef MY_DEBUG
#define DEBUG_WEIGHTING(...) printf(__VA_ARGS__)
#else
#define DEBUG_WEIGHTING(...) 
#endif