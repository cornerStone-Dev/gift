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

#define  HASHTABLE_STATIC_BUILD_IN
#include "../hashTable/hashTable.h"
#include "../hashTable/hashTable.c"

#include "../tool_output/giftLex.c"

pub s32 
openGift(u8 $card)
{
	return (($(card)=='Y')&&(($(card+1))=='E')&&(($(card+2))=='S'));
}
