/* stringListStack.c */

#include "stringListStack.h"

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

/*******************************************************************************
 * Section Local Prototypes
 ******************************************************************************/

#ifndef STRINGLISTSTACK_ASSUME_SPACE
static inline s32
increaseSize(S_stringListStack *sls, u8 **table, u8 **cursor, u32 index);
#endif

/*******************************************************************************
 * Section Init
*******************************************************************************/

STRINGLISTSTACK_STATIC_BUILD
s32
stringListStack_init(S_stringListStack **sls_p)
{
	if(sls_p==0){
		return stringListStack_errorNullParam1;
	}
	S_stringListStack *sls = STRINGLISTSTACK_MALLOC(sizeof(S_stringListStack));
	if(sls==0){
		return stringListStack_errorMallocFailed;
	}

#ifndef STRINGLISTSTACK_ASSUME_SPACE
	sls->endIndex = STRINGLISTSTACK_BASE_SIZE-1;
	sls->size = 1;
#endif
	sls->scopeIndex = 0;
	sls->indexStack =
	STRINGLISTSTACK_MALLOC(sizeof(u32)*STRINGLISTSTACK_SCOPE_STACK_SIZE*2);
	sls->stringCountStack = sls->indexStack + STRINGLISTSTACK_SCOPE_STACK_SIZE;
	sls->table = STRINGLISTSTACK_CALLOC(1, STRINGLISTSTACK_BASE_SIZE);
	
	if(((u64)sls->indexStack&(u64)sls->stringCountStack&(u64)sls->table)==0){
		return stringListStack_errorMallocFailed;
	}
	
	*sls_p = sls;
	return stringListStack_OK;
}

/*******************************************************************************
 * Section Insertion
 ******************************************************************************/

static s32
stringListStack_insert_internal(
	S_stringListStack *sls,
	u8                *string,
	u8                length,
	u32               *index);

STRINGLISTSTACK_STATIC_BUILD
s32
stringListStack_insert(
	S_stringListStack *sls,
	u8                *string,
	u8                length,
	u32               *index)
{
	if(sls==0){
		return stringListStack_errorNullParam1;
	}
	if(string==0){
		return stringListStack_errorNullParam2;
	}
	if(length==0){
		return stringListStack_errorNullParam3;
	}
	return stringListStack_insert_internal(sls, string, length, index);
}

static s32
stringListStack_insert_internal(
	S_stringListStack *sls,
	u8                *string,
	u8                length,
	u32               *index)
{
	u8  *table     = sls->table;
#ifndef STRINGLISTSTACK_ASSUME_SPACE
	u8  *end       = table + sls->endIndex;
	s32 returnCode;
#endif
	u32 scopeIndex = sls->scopeIndex;
	u32 i          = 0;
	u8  *cursor    = table + sls->indexStack[scopeIndex];
	
#ifndef STRINGLISTSTACK_ASSUME_SPACE
	// check if room for the sting
	if ( (end - cursor) < length )
	{
		returnCode  = increaseSize(
						sls,
						&table,
						&cursor,
						sls->indexStack[scopeIndex]);
		if(returnCode){
			return returnCode;
		}
	}
#endif
	
	/* copy string to type_table */
	do{
		*cursor = *string;
		cursor++;
		string++;
		i++;
	} while(i < length);
	
	/* store length of string */
	*cursor = length;
	cursor++;
	
	// return index if address given
	if(index){
		*index = sls->stringCountStack[scopeIndex];
	}
	
	// store index and increment string count
	sls->indexStack[scopeIndex] = cursor - table;
	sls->stringCountStack[scopeIndex]+=1;
	
	return stringListStack_OK;
}

/*******************************************************************************
 * Section Find
*******************************************************************************/

static s32
stringListStack_find_internal(
	S_stringListStack *sls,
	u8                *string,
	u8                length,
	u32               *index);

STRINGLISTSTACK_STATIC_BUILD
s32
stringListStack_find(
	S_stringListStack *sls,
	u8                *string,
	u8                length,
	u32               *index)
{
	if(sls==0){
		return stringListStack_errorNullParam1;
	}
	if(string==0){
		return stringListStack_errorNullParam2;
	}
	if(index==0){
		return stringListStack_errorNullParam4;
	}
	
	return stringListStack_find_internal(sls, string, length, index);
}

/* search type table for IDENT */
static s32
stringListStack_find_internal(
	S_stringListStack *sls,
	u8                *string,
	u8                length,
	u32               *index)
{
	u8  *table     = sls->table;
	u32 scopeIndex = sls->scopeIndex;
	u32 i;
	u8  *cursor    = table + sls->indexStack[scopeIndex] - 1;
	u8  *cursorAtLength;
	u32 idx = sls->stringCountStack[scopeIndex];
	u8  storedLength;
	
	
	/* search backwards, view as a stack */
	while (1)
	{
		start:
		if(cursor <= table){
			break;
		}
		storedLength = (*cursor);
		cursorAtLength = cursor;
		idx--;
		if ( length != storedLength )
		{
			/* skip to next one */
			cursor = cursorAtLength-storedLength-1;
			goto start;
		}
		cursor-=storedLength;
		/* matching length */
		i=0;
		do {
			if ( cursor[i] != string[i] )
			{
				/* skip to next string */
				cursor = cursorAtLength-storedLength-1;
				goto start;
			}
			i++;
		} while (i < length);
		/* success */
		*index = idx;
		return stringListStack_OK;
	}
	/* we have found nothing */
	return stringListStack_nothingFound;
}

/*******************************************************************************
 * Section String at Index
*******************************************************************************/

static u8 *
stringListStack_stringAt_internal(
	S_stringListStack *sls,
	u32               index,
	u8                *output);

STRINGLISTSTACK_STATIC_BUILD	
s32
stringListStack_stringAt(
	S_stringListStack *sls,
	u32               index,
	u8                **output)
{
	u8 *endOfOutput;
	u8 *startOfOutput;
	if(sls==0){
		return stringListStack_errorNullParam1;
	}
	if(output==0){
		return stringListStack_errorNullParam3;
	}
	startOfOutput = *output;
	endOfOutput = stringListStack_stringAt_internal(sls, index, startOfOutput);
	
	if(startOfOutput == endOfOutput){
		return stringListStack_nothingFound;
	}
	
	*output = endOfOutput;
	
	return stringListStack_OK;
}


/* search type table at index */
static u8 *
stringListStack_stringAt_internal(
	S_stringListStack *sls,
	u32               index,
	u8                *output)
{
	u8  *table       = sls->table;
	u32 scopeIndex   = sls->scopeIndex;
	u32 i            = 0;
	u8  *cursor      = table + sls->indexStack[scopeIndex] - 1;
	u32 idx          = sls->stringCountStack[scopeIndex] - 1;
	u32 storedLength = *cursor;
	
	// index is out of bounds
	if (index > idx ) {
		return output;
	}
	
	/* look for index */
	while ( idx != index )
	{
		idx--;
		/* skip to next one */
		cursor = cursor-storedLength-1;
		storedLength = *cursor;
	}
	cursor-=storedLength;
	/* copy out string */
	do{
		*output = *cursor;
		cursor++;
		output++;
		i++;
	} while (i < storedLength);
	
	return output;
}

/*******************************************************************************
 * Section Scope Change
*******************************************************************************/

STRINGLISTSTACK_STATIC_BUILD 
void
stringListStack_enterScope(S_stringListStack *sls)
{
	u32 index = sls->scopeIndex;
	/* copy end of previous scope to new scope */
	sls->stringCountStack[index+1] = sls->stringCountStack[index];
	sls->indexStack[index+1] = sls->indexStack[index];
	sls->scopeIndex=index+1;
}

STRINGLISTSTACK_STATIC_BUILD
void
stringListStack_leaveScope(S_stringListStack *sls)
{
	sls->scopeIndex--;
}

/*******************************************************************************
 * Section Helper Functions
*******************************************************************************/

#ifndef STRINGLISTSTACK_ASSUME_SPACE
static inline s32
increaseSize(S_stringListStack *sls, u8 **table, u8 **cursor, u32 index)
{
	u8 *tmp;
	
	sls->size++;
	tmp=STRINGLISTSTACK_REALLOC(sls->table,sls->size*STRINGLISTSTACK_BASE_SIZE);
	if(tmp == 0){
		// no ability to realloc, return error keep same table
		sls->size--;
		return stringListStack_errorMallocFailed;
	}
	
	sls->table = tmp;
	*table = tmp;
	*cursor = tmp + index;
	
	return stringListStack_OK; 
}
#endif

/*******************************************************************************
 * Section Debug
*******************************************************************************/

#ifdef STRINGLISTSTACK_DEBUG
STRINGLISTSTACK_STATIC_BUILD
u8 *
stringListStack_debugString(s32 mainAPIReturnValue)
{
	s32 x = mainAPIReturnValue;
	switch(x){
		case stringListStack_errorNullParam1:
		return (u8*)"stringListStack Error: First parameter provided is NULL(0).\n";
		case stringListStack_errorNullParam2:
		return (u8*)"stringListStack Error: Second parameter provided is NULL(0).\n";
		case stringListStack_errorNullParam3:
		return (u8*)"stringListStack Error: Third parameter provided is NULL(0).\n";
		case stringListStack_errorNullParam4:
		return (u8*)"stringListStack Error: Third parameter provided is NULL(0).\n";
		case stringListStack_errorMallocFailed:
		return (u8*)"stringListStack Error: "
					"Malloc was called and returned NULL(0).\n";
		case stringListStack_OK:
		return (u8*)"stringListStack OK: Everything worked as intended.\n";
		case stringListStack_nothingFound:
		return (u8*)"stringListStack Status: "
					"Nothing Found. Search for string terminated"
					" with nothing in find or delete.\n";
		default:
		return (u8*)"stringListStack Default: This value is not enumerated."
		       " Debug has no information for you.\n";
	}
}
#endif
