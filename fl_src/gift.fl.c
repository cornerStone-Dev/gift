// gift.fl.c

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <math.h>
#include <termios.h>

#include "../fl_src/giftList.fl.h"

#define  HASHTABLE_STATIC_BUILD_IN
#include "../hashTable/hashTable.h"
#include "../hashTable/hashTable.c"

#include "../tool_output/giftLex.c"

pub union U_Data {
	u8   $s;
	s64  i;
	f64  d;
	s32  fd[2];
};

pub #define TIMESPEC struct timespec

pub struct S_Environment{
	u8  $heap;
	u8  $cache;
	u64 heapIndex;
	u64 heapBottom;
	u64 heapTop;
	u64 generationSize;
	u32 generationCount;
	// hash table stuff
	T_HashTable $hashTable;
	// parameter stack
	u8  $$stack;
	u8  $$sp;
	u8            $tokens;
	u8 $          yycur;
	u8 $          buff;
	u8 $          buff_start;
	u8 $          out;
	T_TIMESPEC time;
	u32           line_num;
	u8            is_def;
	u8            in_list;
	u8            printed_error;
	u8 listNullValue;
	u8 listTrueValue;
	u8 listFalseValue;
	u8 undefinedValue;
};

pub s32
giftCommandLine(S_Environment $e, s32 argc, u8 $$argv)
{
	u8 buff[128];
	u8 $sourceCode, $transformedSource;
	u8 $cursor;
	u8 $readEnd;
	if (argc <= 1)
	{
		printf("gift supplied with no arguments, try --help.\n");
		return 0;
	}
	for (s32 i=1; i<argc; i+=1)
	{
		u32 x = giftLexOptions(argv[i]);
		switch(x){
			case 0:
			return 0;
			case 1:
			printHelp();
			break;
			case 2:
			return 0;
			case 3:
			printf("target file: %s\n",argv[i]);
			sprintf(buff, "%s", argv[i]);
			sourceCode = loadFile(buff, 0);
			// pass the code to the reader
			do{
				cursor = getHeapCursor(e);
				readEnd = giftRead(@sourceCode, cursor, 0);
				if(readEnd == 0){
					break;
				}
				$readEnd = 0xFF;
				transformedSource = cursor;
				finalizeHeapCursor(e, cursor, readEnd);
				transformedSource-=4;
				for(u32 x =0; x<20;x+=1)
				{
					printf("[%02X]",transformedSource[x]);
				}
				printf("\n");
				printf("heapIndex=%ld\n",e.heapIndex);
				printf("heapTop=%ld\n",e.heapTop);
				printf("heapBottom=%ld\n",e.heapBottom);
				cursor = giftEvaluate(e, cursor);
				giftPrint(cursor);
				printf("\n");
				//giftEvaluate(e, cursor);
			
			} while(1);
			
			break;
		}
	}
		
	return 0;
}

#define HELP_MESSAGE \
	"gift help:\n" \
	"--repl for read-eval-print-loop.\n" \
	"yourFileNameHere.gift to run a file.\n"

void
printHelp(void)
{
	printf(HELP_MESSAGE);
}

#define FILE_TYPE FILE

u8$
loadFile(u8 $file_name, u8 as_function)
{
	T_FILE_TYPE $pFile;
	u8 $buffer;
	u64 fileSize, result;
	
	pFile = fopen(file_name, "rb");
	if (pFile==NULL) {fputs ("File error, cannot open source file\n",stderr); exit (1);}
	// obtain file size:
	fseek(pFile , 0 , SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);
	// allocate memory to contain the whole file:
	buffer = malloc(fileSize+2);
	if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}
	// copy the file into the buffer:
	result = fread (buffer,1,fileSize,pFile);
	if (result != fileSize) {fputs ("Reading error\n",stderr); exit (3);}
	/* 0x03 terminate buffer, leave return in sub file */
	if (as_function){
		buffer[fileSize]=';';
		buffer[fileSize+1]=0;
	} else {
		buffer[fileSize]=0;
	}
	fclose (pFile);
	// TODO update to maybe not use garbage collector
	//save_variable(file_name, strlen((const char *)file_name), (s64)buffer);
	return buffer;
}


pub s32
giftInit(S_Environment $$e)
{
	s32 returnCode;
	if(e==0){
		return -1;
	}
	S_Environment $env = calloc(1,sizeof(S_Environment));
	if(env ==0){
		return -2;
	}

	// heap set up
	env.heap = malloc(256*1024); // 256 kb = size of L2 cache(CoffeeLake)
	env.heapTop = (256*1024)-1-16;

	// symbol table set up
	returnCode = hashTable_init(@env.hashTable);
	if(returnCode){
		return returnCode;
	}
	
	// stack set up
	env.stack = malloc(128*8);
	env.sp = env.stack;

	// constants
	env.listNullValue = LIST_NULL;
	env.listTrueValue = LIST_TRUE;
	env.listFalseValue = LIST_FALSE;
	env.undefinedValue = LIST_UNDEFINED;
	
	$e = env;
	return 0;
}





