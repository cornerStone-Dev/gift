// giftEvaluate.fl.c

u8$
giftEvaluate(S_Environment $e, u8 $cursor)
{
	T_hashTableNode $result;
	u8              $symbol;
	u8              $value;
	u8              $$procedure;
	u64             length;
	u64 stackCount = 0;

evaluateDispatch:
	switch($cursor)
	{
		case LIST_0:
		case LIST_INT1:
		case LIST_INT2:
		case LIST_INT3:
		case LIST_INT4:
		case LIST_INT5:
		case LIST_INT6:
		case LIST_INT7:
		case LIST_INT8:
		case LIST_FLOAT:  
		case LIST_TRUE:
		case LIST_FALSE:
		case LIST_NULL:
		case LIST_CHAR_LIT:
		case LIST_STRING_N:
		return cursor;
		
		case LIST_QUOTE:
		cursor+=1;
		return cursor;
		
		case LIST_START:
		cursor+=1;
		break;
		
		case LIST_SYMBOL:
		cursor+=1;
		result =
		hashTable_find_internal(
				e.hashTable,
				cursor,
				strlen(cursor) );
				
		// result is 0 if nothing is found
		if(result){
			return (u8$)result.value;
		} else {
			// return null to express it does not exist
			return @e.listNullValue;
		}
		
		default:
		printf("Syntax error on ");
		giftPrint(cursor);
		printf(" which is an invalid start to an expression.\n");
		return @e.undefinedValue;
	}
	
	switch($cursor)
	{
		// TODO pull out define and set! into function
		case LIST_DEFINE:{
		// define is a special form
		// creates a binding the global environment
		cursor+=1;
		// next expression must evalutate to a symbol
		if($cursor != LIST_SYMBOL)
		{
			symbol = giftEvaluate(e, cursor);
			if($symbol != LIST_SYMBOL)
			{
				printf("Error: attempt to define ");
				giftPrint(cursor);
				printf(", must be a symbol.\n");
				return @e.undefinedValue;
			}
		}
		symbol = cursor;
		// skip over symbol or expression resulting in a symbol
		cursor = skipItem(cursor);
		// evaluate expression that will become bound to the symbol
		value = giftEvaluate(e, cursor);
		// deal with special cases
		if($value == LIST_UNDEFINED){
			printf("Error: attempt to define ");
			giftPrint(symbol);
			printf(" as an expression resulting in an undefined value.\n");
			return value;
		}
		symbol+=1;
		u64 symbolLength = strlen(symbol);
		if($value == LIST_NULL){
			// delete the node if it exists
			if(hashTable_delete_internal(
				e.hashTable,
				symbol,
				symbolLength,
				(u64$)@value)==0)
			{
				free(value);
			}
			return @e.listTrueValue;
		}
		// insert value
		// search for existing symbol
		result =
		hashTable_find_internal(
				e.hashTable,
				symbol,
				symbolLength );
		
		// get size of value, cursor will point to the end
		// TODO this is not efficent is this is a large item, make better
		// make this a function call to getSize with special cases
		cursor = skipItem(value);
		length = cursor - value;
		// result is 0 if nothing is found
		if(result){
			// free old value
			free((u8$)result.value);
			// malloc fresh allocation
			result.value = (u64)malloc((length+7)/8*8);
			// copy value in
			memmove((u8$)result.value, value, length);
			return @e.listTrueValue;
		} else {
			// malloc fresh allocation
			cursor = malloc((length+7)/8*8);
			// copy value in
			memmove(cursor, value, length);
			// insert into has table
			HashTable_insert_internal(
				e.hashTable,
				symbol,
				symbolLength,
				(u64)cursor);
			return @e.listTrueValue;
		}}
		
		case LIST_SET:{
		// set! is a special form
		// updates the closest binding in the environment
		// this version updates the global environment
		cursor+=1;
		// next expression must evalutate to a symbol
		if($cursor != LIST_SYMBOL)
		{
			symbol = giftEvaluate(e, cursor);
			if($symbol != LIST_SYMBOL)
			{
				printf("Error: attempt to set! ");
				giftPrint(cursor);
				printf(", must be a symbol.\n");
				return @e.undefinedValue;
			}
		}
		symbol = cursor;
		// skip over symbol or expression resulting in a symbol
		cursor = skipItem(cursor);
		// evaluate expression that will become bound to the symbol
		value = giftEvaluate(e, cursor);
		// deal with special cases
		if($value == LIST_UNDEFINED){
			printf("Error: attempt to set! ");
			giftPrint(symbol);
			printf(" as an expression resulting in an undefined value.\n");
			return value;
		}
		symbol+=1;
		u64 symbolLength = strlen(symbol);
		if($value == LIST_NULL){
			// delete the node if it exists
			if(hashTable_delete_internal(
				e.hashTable,
				symbol,
				symbolLength,
				(u64$)@value)==0)
			{
				free(value);
			}
			return @e.listTrueValue;
		}
		// insert value
		// search for existing symbol
		result =
		hashTable_find_internal(
				e.hashTable,
				symbol,
				symbolLength );
		
		// get size of value, cursor will point to the end
		// TODO this is not efficent is this is a large item, make better
		// make this a function call to getSize with special cases
		cursor = skipItem(value);
		length = cursor - value;
		// result is 0 if nothing is found
		if(result){
			// free old value
			free((u8$)result.value);
			// malloc fresh allocation
			result.value = (u64)malloc((length+7)/8*8);
			// copy value in
			memmove((u8$)result.value, value, length);
			return @e.listTrueValue;
		} else {
			// symbol does not currently exist
			printf("Error: attempt to set! ");
			giftPrint(symbol);
			printf(" which does not exist.\n");
			return @e.listFalseValue;
		}}
		
		case LIST_IF:
		// if special form
		cursor+=1;
		if( ($(giftEvaluate(e, cursor))) != LIST_FALSE )
		{
			cursor = skipItem(cursor);
			goto evaluateDispatch;
		} else {
			cursor = skipItem(cursor);
			cursor = skipItem(cursor);
			goto evaluateDispatch;
		}
		
		// other procedure
		case LIST_START:
		case LIST_SYMBOL:
		value = getHeapCursor(e);
		do{
			// accumulate procedure and arguments
			$e.sp = giftEvaluate(e, cursor);
			e.sp+=1;
			stackCount+=1;
			cursor = skipItem(cursor);
		} while($cursor != LIST_END);
		break;
		
		// primitive procedure
		case LIST_PLUS:
		cursor+=1;
		// open a heap cursor
		value = getHeapCursor(e);
		// record start of args
		procedure = e.sp;
		while($cursor != LIST_END)
		{
			// accumulate arguments
			$e.sp = giftEvaluate(e, cursor);
			e.sp+=1;
			stackCount+=1;
			cursor = skipItem(cursor);
		}
		// e.sp-=stackCount;
		// return giftAddProcedure(e, value, procedure, stackCount);
		value = giftAddProcedure(e, value, procedure, stackCount);
		// pop stack
		e.sp-=stackCount;
		u8 $transformedSource=value-4;
		for(u32 x =0; x<20;x+=1)
		{
			printf("[%02X]",transformedSource[x]);
		}
		printf("\n");
		printf("heapIndex=%ld\n",e.heapIndex);
		printf("heapTop=%ld\n",e.heapTop);
		printf("heapBottom=%ld\n",e.heapBottom);
		return value;
		
		default:
		printf("Syntax error on ");
		giftPrint(cursor);
		printf(" which is not a valid procedure.\n");
		return @e.undefinedValue;
	}
	
	// get bottom of stack
	procedure = e.sp-stackCount;
	// apply
	switch($($procedure))
	{
		case LIST_PLUS:
		// skip first procedure
		procedure+=1;
		s64 total = 0;
		
		for(u64 x=0; x < stackCount; x+=1)
		{
			total += readListInt(
				(e.sp[x])+1,
				$(e.sp[x]));
		}
		
		cursor = listWriteInt(value, total);
		finalizeHeapCursor(e, value, cursor);
		// pop stack
		e.sp-=stackCount;
		u8 $transformedSource=value-4;
		for(u32 x =0; x<20;x+=1)
		{
			printf("[%02X]",transformedSource[x]);
		}
		printf("\n");
		printf("heapIndex=%ld\n",e.heapIndex);
		printf("heapTop=%ld\n",e.heapTop);
		printf("heapBottom=%ld\n",e.heapBottom);
		return value;
		
		//~ case LIST_SUBT:
		//~ printf("-");
		//~ value+=1;
		//~ goto loop;
		
		//~ case LIST_MULT:
		//~ printf("*");
		//~ value+=1;
		//~ goto loop;
		
		//~ case LIST_DIVI:
		//~ printf("/");
		//~ value+=1;
		//~ goto loop;
		
		//~ case LIST_REMA:
		//~ printf("%%");
		//~ value+=1;
		//~ goto loop;
		//~ default:
		//~ printf("unknown value");
		//~ value+=1;
		//~ goto loop;
		
	}
	
	
	return 0;
}

u8$
giftAddProcedure(S_Environment $e, u8 $cursor, u8 $$args, u64 numArgs)
{
	u8 $end;
	u8 $currentArg;
	U_Data arg1, arg2;
	u8 type1, type2, typeComp;
	s64 total = 0;
	if(numArgs<2){
		printf("ERROR: minimum of 2 parameters for '+' procedure.\n");
	}
	currentArg = $args;
	if($currentArg == LIST_FLOAT) // its a float
	{
		currentArg+=1;
		arg1.d = readListFloat(currentArg);
		type1 = 2;
	} else if ($currentArg <= LIST_INT8)
	{ // its an int
		currentArg+=1;
		arg1.i = readListInt(currentArg, ($(currentArg-1)));
		type1 = 1;
	} else {
		printf("ERROR: Math is for only floats and ints.\n");
		return @e.undefinedValue;
	}
	numArgs-=1;
	
	do{
		args+=1;
		currentArg = $args;
		if($currentArg == LIST_FLOAT) // its a float
		{
			currentArg+=1;
			arg2.d = readListFloat(currentArg);
			type2 = 2;
		} else if ($currentArg <= LIST_INT8)
		{ // its an int
			currentArg+=1;
			arg2.i = readListInt(currentArg, ($(currentArg-1)));
			type2 = 1;
		} else {
			printf("ERROR: Math is for only floats and ints.\n");
			return @e.undefinedValue;
		}
		
		typeComp = type1|type2;
		if (typeComp==1)
		{
			arg1.i = arg1.i+arg2.i;
		} else if (typeComp==2) {
			arg1.d = arg1.d+arg2.d;
		} else if (type1==2) {
			arg1.d = arg1.d+arg2.i;
		} else {
			arg1.d = arg1.i+arg2.d;
			type1 = 2;
		}
		numArgs-=1;
	} while(numArgs);
	
	// we have finished the computation
	if(type1 == 1){
		end = listWriteInt(cursor, arg1.i);
	} else {
		end = listWriteFloat(cursor, arg1.d);
	}
	finalizeHeapCursor(e, cursor, end);
	return cursor; 
}

