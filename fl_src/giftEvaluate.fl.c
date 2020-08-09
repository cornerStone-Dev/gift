// giftEvaluate.fl.c

u8$
giftEvaluate(S_Environment $e, u8 $cursor)
{
	// evaluateDispatch
	T_hashTableNode $result;
	u8              $symbol;
	u8              $value;
	u64             length;
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
		printf(" which does not a valid start to an expression.\n");
		return @e.undefinedValue;
	}
	
	switch($cursor)
	{	
		//~ case LIST_END:
		//~ printf(")");
		//~ value+=1;
		//~ if(parenCount)
		//~ {
			//~ parenCount-=1;
		//~ } else {
			//~ withinList = 0;
		//~ }
		//~ goto loop;
		
		//~ case LIST_SYMBOL:
		//~ value+=1;
		//~ length = strlen(value)+1; // including null
		//~ printf("%s", value);
		//~ value+=length;
		//~ goto loop;
		
		case LIST_DEFINE:
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
			result.value = (u64)malloc(length);
			// copy value in
			memmove((u8$)result.value, value, length);
			return @e.listTrueValue;
		} else {
			// malloc fresh allocation
			cursor = malloc(length);
			// copy value in
			memmove(cursor, value, length);
			// insert into has table
			HashTable_insert_internal(
				e.hashTable,
				symbol,
				symbolLength,
				(u64)cursor);
			return @e.listTrueValue;
		}
		
		
		return 0;
		
		//~ case LIST_SET:
		//~ printf("set!");
		//~ value+=1;
		//~ goto loop;
		
		//~ case LIST_IF:
		//~ printf("if");
		//~ value+=1;
		//~ goto loop;
		
		//~ case LIST_PLUS:
		//~ printf("+");
		//~ value+=1;
		//~ goto loop;
		
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

