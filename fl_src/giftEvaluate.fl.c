// giftEvaluate.fl.c

u8$
giftEvaluate(S_Environment $e, u8 $cursor, u8 $$sp)
{
	T_hashTableNode $result;
	u8              $value;
	u8              $nextArgument;
	u8              $heapCursor=0;
	u8              $$procedure;
	u8              $$arguments = 0;
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
		u32 symbolLength = $cursor;
		cursor+=1;
		result =
		hashTable_find_internal(
				e.hashTable,
				cursor,
				symbolLength );
				
		// result is 0 if nothing is found
		if(result){
			return (u8$)result.value;
		} else {
			// return null to express it does not exist
			return @e.listNullValue;
		}
		
		case LIST_REG0:
		case LIST_REG1:
		case LIST_REG2:
		case LIST_REG3:
		case LIST_REG4:
		case LIST_REG5:
		case LIST_REG6:
		case LIST_REG7:
		return $(e.arguments+($cursor-LIST_REG0));
		
		default:
		printf("Syntax error on ");
		giftPrint(cursor);
		printf(" which is an invalid start to an expression.\n");
		return @e.undefinedValue;
	}
	
	switch($cursor)
	{
		// TODO pull out define and set! into function
		case LIST_DEFINE:
		// define is a special form
		// creates a binding the global environment
		return evalDefineExpr(e, cursor, 0, sp);
		
		case LIST_SET:
		// set! is a special form
		// updates the closest binding in the environment
		// this version updates the global environment
		return evalDefineExpr(e, cursor, 1, sp);
		
		case LIST_IF:
		// if special form
		cursor+=1;
		if( ($(giftEvaluate(e, cursor, sp))) != LIST_FALSE )
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
		// for procedure open a heap cursor before evaluating arguments
		if(heapCursor == 0){
			heapCursor = getHeapCursor(e);
		}
		nextArgument = cursor;
		procedure = sp;
		// accumulate procedure and arguments by evaluating them
		// this is done in the context of the previous arguments(if there)
		do{
			cursor = nextArgument;
			// skip past item
			nextArgument = skipItem(cursor);
			// store item in stack
			$sp = giftEvaluate(e, cursor, sp);
			sp+=1;
			// keep track of how many things we have added to the stack
			stackCount+=1;
		} while($nextArgument != LIST_END);
		
		// check if we are in a tailcall context
		if (arguments != 0)
		{
			u64 x = 0;
			// set arguments to previous procedure location
			arguments = arguments-1;
			// copy down new procedure and arguments so stack doesn't grow
			for(; x < stackCount; x+=1)
			{
				arguments[x] = procedure[x];
			}
			// fix up stack
			sp = @arguments[x];
			// fix up procedure
			procedure = arguments;
		}
		
		// change context into new function
		arguments = procedure+1;
		
		// sudo apply here
		u8 $proc = $procedure;
		if($proc != LIST_PROCEDURE){
			printf("Error: expected procedure\n");
		}
		proc+=1;
		if ($proc != stackCount-1 ){
			printf("Error: expected %d arguments,  there is %ld\n",$proc, stackCount-1);
		}
		proc+=1;
		// we are now looking at the body
		// assume single body
		cursor = proc;

		// pop the stack
		//e.sp-=stackCount;
		stackCount = 0;
		// set arguments to what they were for evals
		e.arguments = arguments;
		goto evaluateDispatch;
		
		
		case LIST_LET:
		cursor+=1;
		nextArgument = cursor;
		// accumulate bindings
		// this is done in the context of the previous arguments(if there)
		do{
			cursor = nextArgument;
			// skip past item
			nextArgument = skipItem(cursor);
			// move into list
			cursor+=1;
			// store item in stack
			$sp = giftEvaluate(e, cursor, sp);
			sp+=1;
		} while($nextArgument != LIST_END);
		
		cursor = nextArgument +1;
		
		
		goto evaluateDispatch;
		
		// primitive procedure
		case LIST_PLUS:
		case LIST_SUBT:
		case LIST_MULT:
		case LIST_DIVI:
		case LIST_REMA:
		cursor+=1;
		// TODO Do I need to open a heap cursor?
		// open a heap cursor
		if(heapCursor == 0){
			heapCursor = getHeapCursor(e);
		}
		value = evalMathExpr(e, cursor, heapCursor, ($(cursor-1)-LIST_PLUS),sp);
		return value;

		case LIST_EQUALS:
		case LIST_GREATER_THAN:
		case LIST_LESS_THAN:
		case LIST_LESS_THAN_OR_EQUAL:
		case LIST_GREATER_THAN_OR_EQUAL:
		// these procedures always produce #t or #f
		return evalCompareExpr(e, cursor+1, (($cursor)-LIST_EQUALS),sp);
		
		case LIST_NOT:
		// produces #t if #f otherwise #f
		return evalLogicalNotExpr(e, cursor,sp);
		
		case LIST_LAMBDA:
		// the result of a lambda is a procedure
		// we must compile the procedure from the inputs
		return giftLambda(e, cursor);
		
		default:
		printf("Syntax error on ");
		giftPrint(cursor);
		printf(" which is not a valid procedure.\n");
		return @e.undefinedValue;
	}
	
	printf("No mans land.\n");
	return @e.undefinedValue;
	
	
	return 0;
}

u8$
giftAddProcedure(S_Environment $e, u8 $cursor, u8 $$args, u64 numArgs)
{
	u8 $end;
	u8 $currentArg;
	U_Data arg1, arg2;
	u8 type1, type2, typeComp;
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

u8$
evalMathExpr(S_Environment $e, u8 $t, u8 $out, u8 op, u8 $$sp)
{
	U_Data arg[2];
	f64 arg3;
	u8 $next, $end;
	u8 type[2];
	u8 typeComp;
	u8 typeIdx=0;
	again:
	next = t;
	t = giftEvaluate(e, t, sp);
	if($t == LIST_FLOAT) // its a float
	{
		t+=1;
		arg[typeIdx].d = readListFloat(t);
		type[typeIdx] = 2;
		//next+=8;
	} else if ($t <= LIST_INT8)
	{ // its an int
		t+=1;
		arg[typeIdx].i = readListInt(t, ($(t-1)));
		type[typeIdx] = 1;
		//next+=($(t-1));
	} else {
		printf("ERROR: Math is for only floats and ints.\n");
		return @e.undefinedValue;
	}
	next = skipItem(next);
	t = next;
	if(typeIdx==0){
		typeIdx=1;
		if ($t==LIST_END)
		{
			// only one parameter
			// minus is only valid situation
			if(op ==1){
				if (type[0]!=1)
				{
					end = listWriteFloat(out, -(arg[0].d));
				} else {
					end = listWriteInt(out, -(arg[0].i));
				}
				finalizeHeapCursor(e, out, end);
				return out;
			} else {
				printf("ERROR: minimum of 2 parameters for +,*,/,%%.\n");
				return @e.undefinedValue;
			}
		}
		goto again;
	}
	// now that everything is recorded deal with types
	typeComp = type[0]|type[1];
	switch(op){
		case 0:
		if (typeComp==1)
		{
			arg[0].i = arg[0].i+arg[1].i;
		} else if (typeComp==2) {
			arg[0].d = arg[0].d+arg[1].d;
		} else if (type[0]==2) {
			arg[0].d = arg[0].d+arg[1].i;
		} else {
			arg[0].d = arg[0].i+arg[1].d;
			type[0] = 2;
		}
		break;
		case 1:
		if (typeComp==1)
		{
			arg[0].i = arg[0].i-arg[1].i;
		} else if (typeComp==2) {
			arg[0].d = arg[0].d-arg[1].d;
		} else if (type[0]==2) {
			arg[0].d = arg[0].d-arg[1].i;
		} else {
			arg[0].d = arg[0].i-arg[1].d;
			type[0] = 2;
		}
		break;
		case 2:
		if (typeComp==1)
		{
			arg[0].i = arg[0].i*arg[1].i;
		} else if (typeComp==2) {
			arg[0].d = arg[0].d*arg[1].d;
		} else if (type[0]==2) {
			arg[0].d = arg[0].d*arg[1].i;
		} else {
			arg[0].d = arg[0].i*arg[1].d;
			type[0] = 2;
		}
		break;
		case 3:
		if (typeComp==1)
		{
			arg[0].i = arg[0].i/arg[1].i;
		} else if (typeComp==2) {
			arg[0].d = arg[0].d/arg[1].d;
		} else if (type[0]==2) {
			arg[0].d = arg[0].d/arg[1].i;
		} else {
			arg[0].d = arg[0].i/arg[1].d;
			type[0] = 2;
		}
		break;
		case 4:
		if (typeComp==1)
		{
			arg[0].i = arg[0].i%arg[1].i;
		} else if (typeComp==2) {
			arg3 = arg[0].d/arg[1].d;
			arg3 = modf(arg3, @arg[0].d);
			arg[0].d = arg3*arg[1].d;
		} else if (type[0]==2) {
			arg[1].d = arg[1].i;
			arg3 = arg[0].d/arg[1].d;
			arg3 = modf(arg3, @arg[0].d);
			arg[0].d = arg3*arg[1].d;
		} else {
			arg[0].d = arg[0].i;
			arg3 = arg[0].d/arg[1].d;
			arg3 = modf(arg3, @arg[0].d);
			arg[0].d = arg3*arg[1].d;
			type[0] = 2;
		}
		break;
	}
	if ($t==LIST_END)
	{
		if (type[0]!=1)
		{
			end = listWriteFloat(out, arg[0].d);
		} else {
			//printf("computed value %ld\n", arg[0].i);
			end = listWriteInt(out, arg[0].i);
		}
		finalizeHeapCursor(e, out, end);
		return out;
	}
	goto again;
}

u8$
evalCompareExpr(S_Environment $e, u8 $cursor, u8 op, u8 $$sp)
{
	U_Data arg1, arg2;
	u8  $next;
	u32 type1, type2;
	u32 typeComp;
	next = cursor;
	cursor = giftEvaluate(e, cursor, sp);
	if($cursor <= LIST_INT8) // its an int
	{
		cursor+=1;
		arg1.i = readListInt(cursor, ($(cursor-1)));
		type1 = 1;
	} else if ($cursor == LIST_FLOAT) // its a float
	{
		cursor+=1;
		arg1.d = readListFloat(cursor);
		type1 = 2;
	} else {
		printf("ERROR: Math comparision is for only floats and ints.\n");
		return @e.undefinedValue;
	}
	next = skipItem(next);
	cursor = next;
	if ($cursor==LIST_END)
	{
		printf("ERROR: 2 parameters required for =,<,>,<=,>=.\n");
		return @e.undefinedValue;
	}
	// second argument
	cursor = giftEvaluate(e, cursor, sp);
	if($cursor <= LIST_INT8) // its an int
	{
		cursor+=1;
		arg2.i = readListInt(cursor, ($(cursor-1)));
		type2 = 1;
	} else if ($cursor == LIST_FLOAT) // its a float
	{
		cursor+=1;
		arg2.d = readListFloat(cursor);
		type2 = 2;
	} else {
		printf("ERROR: Math comparision is for only floats and ints.\n");
		return @e.undefinedValue;
	}
	// both arguments are read in time to process
	typeComp = type1|type2;
	if (typeComp == 1)
	{
		switch(op)
		{
			case 0:
			return @e.listFalseValue+(arg1.i==arg2.i);
			case 1:
			return @e.listFalseValue+(arg1.i>arg2.i);
			case 2:
			return @e.listFalseValue+(arg1.i<arg2.i);
			case 3:
			return @e.listFalseValue+(arg1.i<=arg2.i);
			case 4:
			return @e.listFalseValue+(arg1.i>=arg2.i);
		}
	} else if (typeComp == 2)
	{
		switch(op)
		{
			case 0:
			return @e.listFalseValue+(arg1.d==arg2.d);
			case 1:
			return @e.listFalseValue+(arg1.d>arg2.d);
			case 2:
			return @e.listFalseValue+(arg1.d<arg2.d);
			case 3:
			return @e.listFalseValue+(arg1.d<=arg2.d);
			case 4:
			return @e.listFalseValue+(arg1.d>=arg2.d);
		}
	}
	printf("ERROR: Math comparision must compare the same type.\n");
	return @e.undefinedValue;
}

u8$
evalLogicalNotExpr(S_Environment $e, u8 $cursor, u8 $$sp)
{
	cursor = giftEvaluate(e, cursor+1, sp);
	// TODO integrate check for more than argument error
	if( $cursor == LIST_FALSE )
	{
		return @e.listTrueValue;
	} else {
		return @e.listFalseValue;
	}
}

u8$
evalDefineExpr(S_Environment $e, u8 $cursor, u64 isSet, u8 $$sp)
{
	u8  $symbol;
	u8  $value;
	u8  $valueCopy;
	u64 symbolLength;
	u64 valueLength;
	T_hashTableNode $result;
	// define is a special form
	// creates a binding the global environment
	cursor+=1;
	symbol = cursor;
	// next expression must evalutate to a symbol
	if($cursor != LIST_SYMBOL)
	{
		symbol = giftEvaluate(e, cursor, sp);
		if($symbol != LIST_SYMBOL)
		{
			printf("Error: attempt to define ");
			giftPrint(cursor);
			printf(", must be a symbol.\n");
			return @e.undefinedValue;
		}
	}
	// skip over symbol or expression resulting in a symbol
	cursor = skipItem(cursor);
	// evaluate expression that will become bound to the symbol
	value = giftEvaluate(e, cursor, sp);
	// deal with special cases
	if($value == LIST_UNDEFINED){
		printf("Error: attempt to define ");
		giftPrint(symbol);
		printf(" as an expression resulting in an undefined value.\n");
		return value;
	}
	symbol+=1;
	symbolLength = $symbol;
	symbol+=1;
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
	// get size of value, cursor will point to the end
	// TODO this is not efficent is this is a large item, make better
	// make this a function call to getSize with special cases
	if($value == LIST_PROCEDURE)
	{
		valueLength = ($((u16$)(value-2)))*4-2;
		printf("procedure length %ld\n",valueLength);
	} else {
		cursor = skipItem(value);
		valueLength = cursor - value;
	}
	
	// insert value
	// search for existing symbol
	result =
	hashTable_find_internal(
			e.hashTable,
			symbol,
			symbolLength );
	
	// result is 0 if nothing is found
	if(result){
		// free old value
		free((u8$)result.value);
		// malloc fresh allocation
		valueCopy = malloc((valueLength+7)/8*8);
		// copy value in
		memmove(valueCopy, value, valueLength);
		result.value = (u64)valueCopy;
		return @e.listTrueValue;
	} else {
		if (isSet){
			// symbol does not currently exist
			printf("Error: attempt to set! ");
			giftPrint(symbol);
			printf(" which does not exist.\n");
			return @e.listFalseValue;
		} else {
			// malloc fresh allocation
			valueCopy = malloc((valueLength+7)/8*8);
			// copy value in
			memmove(valueCopy, value, valueLength);
			// insert into has table
			HashTable_insert_internal(
				e.hashTable,
				symbol,
				symbolLength,
				(u64)valueCopy);
			return @e.listTrueValue;
		}
	}
}

