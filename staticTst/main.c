#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define DBG 1
/* in my pc unsigned int is 32 bits*/
typedef unsigned int uint32_t;
#define u32_bound ((unsigned int)pow(2,sizeof(uint32_t)*8)-1)
static void _tstZero(uint32_t* _prevWU,uint32_t future){
	unsigned int  must_sleep=1;
    uint32_t now=u32_bound+21;
#if DBG
	printf(" [ DBG ] now value %u \n",now);
#endif
    if(now<*_prevWU){
        if(future>now && *_prevWU>future )
            must_sleep=1;
        else
            must_sleep=0;
    }
    if(must_sleep)
        printf(" [ RESULT ] It must sleep  for %u \n \n \n",future-now);
    else
        printf(" [ RESULT ] Time exceeded \n \n \n");
} 
int main(void){
    /* Sleeping task*/
	uint32_t prev=u32_bound-10;
	uint32_t next=u32_bound+40;
	printf(" \n \n [ TST_1 ] Last WU value %u, Next WU %u \n",prev,next);
	_tstZero(&prev,next);
    /*Not sleeping*/
    next=u32_bound+10;
    printf(" [ TST_2 ] Last WU  %u, Next  WU  %u \n",prev,next);
    _tstZero(&prev,next);
    /*Not sleeping*/
    next=u32_bound-5;
    printf(" [ TST_3 ]  LAST WU  %u, Next WU  %u \n",prev,next);
    _tstZero(&prev,next);

	return 0;
}
