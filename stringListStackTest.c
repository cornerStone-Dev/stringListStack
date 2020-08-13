#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "stringListStack.h"

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

#define UPPER_LIMIT 50

int main(void)
{
	S_stringListStack *sls;
	u32 index;
	u8* buffp;
	u8 buff[128]={0};
	s32 returnCode;
	
	printf("Start of Test:\n");
	returnCode=stringListStack_init(&sls);
	if(returnCode){
		printf("stringListStack_init: %s\n",
		stringListStack_debugString(returnCode));
	}
	enterScope(sls);
	for (s64 x=1; x<=UPPER_LIMIT; x++){
		sprintf((char*)buff, "%ld", x);
		if ( stringListStack_insert(sls, (u8*)buff, strlen((char*)buff), 0) ){
			printf("Strange failure to insert %ld\n", x);
		}
		if((x%256)==0){
			leaveScope(sls);
			enterScope(sls);
		}
	}
	printf("ht->count is %d\n", sls->stringCountStack[sls->scopeIndex]);
	buffp = buff+20;
	for (s64 x=1; x<=UPPER_LIMIT; x++){
		sprintf((char*)buff, "%ld", x);
		returnCode = stringListStack_find(sls, (u8*)buff, strlen((char*)buff),
								&index);
		printf("[%d] ",index);
		if(returnCode){
			printf("Strange failure to insert %ld, code %d\n", x, returnCode);
		}
		
		stringListStack_stringAt(
			sls,
			index,
			&buffp);
		
		buffp = buff+20;
		printf("string is %s\n",buffp);
	}
	
	leaveScope(sls);
	printf("ht->count is %d\n", sls->stringCountStack[sls->scopeIndex]);

	return 0;
}

