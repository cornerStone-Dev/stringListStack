/* stringListStack.h */

#ifndef STRINGLISTSTACK_HEADER
#define STRINGLISTSTACK_HEADER
#include <stdint.h>

/*******************************************************************************
 * Section Interface for Alterations
*******************************************************************************/
#define STRINGLISTSTACK_DEBUG

//#define STRINGLISTSTACK_ASSUME_SPACE

#ifndef STRINGLISTSTACK_CUSTOM_ALLOC
#include <stdlib.h>
#define STRINGLISTSTACK_FREE    free
#define STRINGLISTSTACK_MALLOC  malloc
#define STRINGLISTSTACK_CALLOC  calloc
#define STRINGLISTSTACK_REALLOC realloc
#endif

#ifdef STRINGLISTSTACK_STATIC_BUILD_IN
#define STRINGLISTSTACK_STATIC_BUILD static
#else
#define STRINGLISTSTACK_STATIC_BUILD
#endif

#define BASE_SIZE (4*1024)
#define STRINGLISTSTACK_SCOPE_STACK_SIZE (32)

/*******************************************************************************
 * Section Types
*******************************************************************************/

typedef struct{
#ifndef STRINGLISTSTACK_ASSUME_SPACE
	uint32_t endIndex;
	uint32_t size;
#endif
	uint32_t scopeIndex;
	uint32_t *indexStack;
	uint32_t *stringCountStack;
	uint8_t  *table;
} S_stringListStack;

// Main Function API error enumeration
enum {
	// errors
	stringListStack_errorNullParam1         = -1,
	stringListStack_errorNullParam2         = -2,
	stringListStack_errorNullParam3         = -3,
	stringListStack_errorNullParam4         = -4,
	stringListStack_errorMallocFailed       = -5,
	// worked as expected
	stringListStack_OK                      =  0,
	// not an error, but did not work as expected
	stringListStack_nothingFound            =  1,
};

/*******************************************************************************
 * Section Main Function API
 * Return values are of the enumeration in Types
*******************************************************************************/

STRINGLISTSTACK_STATIC_BUILD
int32_t
stringListStack_init(S_stringListStack **sls_p);


STRINGLISTSTACK_STATIC_BUILD
int32_t
stringListStack_insert(
	S_stringListStack *sls, // pointer memory holding address of stringListStack
	uint8_t  *string,  // pointer to c-string key(null not required)
	uint8_t  length,  // length of key in bytes(not including null termination)
	uint32_t *index); // optional pointer to memory to save index

STRINGLISTSTACK_STATIC_BUILD
int32_t
stringListStack_find(
	S_stringListStack *sls, // pointer memory holding address of stringListStack
	uint8_t  *string, // pointer to c-string key(null not required)
	uint8_t  length,  // length of key in bytes(not including null termination)
	uint32_t *index); // pointer to memory to save index

STRINGLISTSTACK_STATIC_BUILD	
int32_t
stringListStack_stringAt(
	S_stringListStack *sls, // pointer memory holding address of stringListStack
	uint32_t        index,  // index for string desired
	uint8_t         **output);  // pointer to pointer for writing string
								// will return pointer to one past end of string

/*******************************************************************************
 * Section Helper/Utility Function API
*******************************************************************************/

STRINGLISTSTACK_STATIC_BUILD 
void
enterScope(S_stringListStack *sls);

STRINGLISTSTACK_STATIC_BUILD
void
leaveScope(S_stringListStack *sls);

#ifdef STRINGLISTSTACK_DEBUG
STRINGLISTSTACK_STATIC_BUILD
uint8_t *
stringListStack_debugString(int32_t mainAPIReturnValue);
#endif


#endif
