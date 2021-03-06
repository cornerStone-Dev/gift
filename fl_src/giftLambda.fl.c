// giftLambda.fl.c




/*******************************************************************************
 * Lambda, the whole party comes together with this function, as it does the
 * compiling of procedures for the evaluation virtual machine.
 * 
 * 1. We need to walk through the formal arguments and deal with their different
 * forms. 
 * (arg1 arg2 ...):: arguments presented as a list. This is a contract that the
 * number of arguments must be exactly this number or it is an error. Emit a
 * bytecode that encodes this requirement.
 * 
 * arg1:: 0 or more arguments, they are put into a list and bound to this arg1.
 * 
 * (arg1 arg2 . arg3):: 2 or more args, the extras if any are put into a list.
 * This is combination of the first two cases.
 * 
 * At this point we have emitted a requirement bytecode for number of args. We
 * have also emitted a create list bytecode for that case. Next we will add the
 * symbols(args) to a symbol table stack to represent local lexical scoping
 * rules. Each arg saves its relative position which corresponds to the operand
 * stack.
 * 
 * 2. Next we do a first pass of the body of the lambda. This is to fill in the
 * rest of the symbol table. nested lambda's are skipped over because they will
be evaluated later. let expressions bind in symbols locally, these are processed
 * as creating a new scope, the let variable name is replaced with a operand
 * stack offset and a stack assignment expression is emitted. All uses of this
 * local variable are also replaced with this information. However, and this is
 * the important part, lambda is creating a lexical closure and needs to save
 * its environment. When we hit a symbol that is not in the local let bindings
 * or the argument list OR the global symbol table, it must be from the enclos-
 * nig context. This first pass will copy the value these types of variables
 * immediatly after part 1 above, they will be prefixed with a load into operand
 * stack command and effictively "tack on" to the argument list. They are up in
 * front so they are processed before we get into the procedure body and are 
 * ready to go. If a symbol is found in the global symbol table (the last place
 * we look) then the symbol itself is emitted. Easy.
 * KEY: this is just the first hurdle of being a correct environment extension.
 * We also need to pay attention to ANY mutation to these guys. All of them will
 * be allocated a space of MAX(sizeOfVal, 9) in order to cover the common case
 * of floats and INTs of all sizes. When a mutation occurs that alters the size
 * beyond what can be put there then the entire lambda is remade, copying the
 * unchanged parts and inserting the new value. This seems expensive, but we the
 * garbage collector will mitigate this. Also, most the time the things changed
 * are simple ints and floats.
 * The byte code for this needs thought through.
 * First passes are done incrementally through the code due to the nature of the
 * stack repsenting the lexical scope, which will be poped and overwritten by
 * the next scope at the same level through the function.
 * 
 * 3. We now walk through through the body of lambda. The body of the lambda is
 * a series of one or more expressions. Excluding lambda, let, and other derived
 * or similar to them we just transcribe the bytecode as is. Special processing
 * for symbols is needed to resolve them correctly as described above. All the
 * symbols are resolved to operand stack references or a regular symbol which is
 * a symbol table look at runtime.
 * 
 * 4. This entire procedure is then returned.
 * 
 * Section Ultimate
*******************************************************************************/


// we will need a stack of stack pointers or offsets to manage the generated
// stack offset code so that the evaluation of sub procedures is done correctly
// that is to say when they increment the stack pointer(correctly) it does not
// cause our byte to return the wrong thing.

// lambda ::= formals body.

// formals ::= SYMBOL
// formals ::= OPAREN SYMBOL* CPAREN
// formals ::= OPAREN SYMBOL+ DOT SYMBOL CPAREN

// body ::= expression+

// expression ::= everything stuff

struct S_Formals
{
	u8  $cursor;
	u64 argumentCount;
};

u8$
giftLambda(S_Environment $e, u8 $cursor)
{
	u8 $output = getHeapCursor(e);
	u8 $start = output;
	S_Formals c;
	cursor+=1;
	$output = LIST_PROCEDURE;
	output+=1;
	
	// enter into scope
	stringListStack_enterScope(e.sls);
	c = parseFormals(e, cursor);
	cursor = c.cursor;
	$output = c.argumentCount;
	output+=1;
	
	output = parseBody(e, cursor, output);
	// leave scope
	stringListStack_leaveScope(e.sls);
	finalizeHeapCursor(e, start, output);
	
	return start;
}

// when parse formals is complete the symbol table is loaded with argument
// names, the arity requirements are emitted, the cursor is one past
// the arguments
S_Formals
parseFormals(S_Environment $e, u8 $cursor)
{
	S_Formals c;
	c.cursor = cursor;
	c.argumentCount = 0;
	
	if($c.cursor== LIST_SYMBOL)
	{
		// single argument, everything will get concatenated
		c.cursor = parseSymbol(e, c.cursor);
		// emit takes any amount of arguments token
		// VM needs to concatenate all arguments pass to the procedure
		c.argumentCount = 0xFF;
	} else if ($c.cursor == LIST_START) {
		// list of arguments
		c.cursor+=1;
		
	loop:
		if ($c.cursor != LIST_SYMBOL)
		{
			goto error;
		}
		// $c.cursor == LIST_SYMBOL
		c.argumentCount+=1;
		c.cursor = parseSymbol(e, c.cursor);
		
		// check for dot
		if ($c.cursor == LIST_DOT)
		{
			c.cursor+=1;
			c.cursor = parseSymbol(e, c.cursor);
			c.argumentCount|=0x80;
			// VM needs to concatenate all arguments pass to the procedure
			// must end after this symbol
			if ($c.cursor != LIST_END)
			{
				goto error;
			}
			goto done;
		}
		if ($c.cursor == LIST_END)
		{
			// we have completed the parsing of the arguments
			// emit the arity requirements bytecode
			c.cursor+=1;
			goto done;
		}
		
		goto loop;
	} else if ($c.cursor == LIST_NULL) {
		c.cursor+=1;
		goto done;
	} else {
		goto error;
	}
	done:
	return c;
	
	error:
	// syntax error, with message
	printf("Lambda Parser: Expected single symbol or list of symbols "
			"or a list of symbols dot symbol.");
	//TODO return a dummy function?
	return (S_Formals){0,0};
}

u8$
parseSymbol(S_Environment $e, u8 $cursor)
{
	// we have symbol in the formals
	// add to symbol table and move cursor past it
	u64 length;
	cursor+=1;
	length = $cursor;
	cursor+=1;
	stringListStack_insert_internal(e.sls, cursor, length, 0);
	cursor = cursor + length + 1;
	return cursor;
}


// this walks through the body of lambda
// there is an implicit "begin" at the start of the body, lets make it explicit
// going to try to not do it
// the arguments to the function have been aded to the symbols
// they were added in register order, so now we get their index and emit
// corresonding register commands
u8$
parseBody(S_Environment $e, u8 $cursor, u8 $output)
{
	u8 $start;
	u64 length;
	u32 $parenCountSp;
	u32 parenCount=0;
	u32 index=0;
	s32 returnCode;
	u32 parenCountArray[32];
	
	parenCountSp = parenCountArray;
	
	//$output = LIST_BEGIN;
	//output+=1;
	
again:
	start = cursor;
	
loop:
	
	// skip list starts
	if ($cursor == LIST_START)
	{
		cursor+=1;
		parenCount+=1;
	}
	switch($cursor)
	{
		case LIST_END:
		cursor+=1;
		if(parenCount)
		{
			parenCount-=1;
			goto loop;
		} else {
			goto exit;
		}
		case LIST_SYMBOL:

		// get length
		length = $(cursor+1);

		returnCode = stringListStack_find_internal(
						e.sls,
						cursor+2,
						length,
						@index);
		
		// symbol not found, assume it will be in global symbol table at runtime
		if (returnCode){
			cursor = skipItem(cursor);
			goto loop;
		}
		
		// write out what has been read so far
		output = writeRun(start,cursor,output);
		// move cursor
		cursor+=length+3;
		
		// emit register look up
		$output = LIST_REG0 + index;
		output+=1;
		
		goto again;
		case LIST_LET:{
		// let_expression ::= (let ( (x1 v1)...(x v) ) body)
		printf("let expression\n");
		// write out what has been read so far
		output = writeRun(start,cursor,output);
		// enter into scope
		stringListStack_enterScope(e.sls);
		// record each variable inside the binding
		u8 $tmpCursor = cursor;
		u8 $nextBinding;
		u8 $startBinding;
		tmpCursor+=1;
		if($tmpCursor != LIST_START){
			printf("Syntax Error:Need a list following a let.\n");
			goto exit;
		}
		tmpCursor+=1;
		startBinding = tmpCursor;
		
		// first add ALL symbols to symbol table
		while(1){
			if($tmpCursor != LIST_START){
				printf("Syntax Error:Need a list for each binding .\n");
				goto exit;
			}
			nextBinding = skipItem(tmpCursor);
			tmpCursor+=1;
			if($tmpCursor != LIST_SYMBOL){
				printf("Syntax Error:Need to bind to symbol.\n");
				goto exit;
			}
			parseSymbol(e, tmpCursor);
			tmpCursor = nextBinding;
			if($nextBinding == LIST_END)
			{
				break;
			}
		}
		
		// next write out expressions
		$output = LIST_LET;
		output+=1;
		
		tmpCursor = startBinding;
		while(1){
			// get next binding
			nextBinding = skipItem(tmpCursor);
			// move past list start
			tmpCursor+=1;
			// skip symbol
			tmpCursor = skipItem(tmpCursor);
			// output list start
			$output = LIST_START;
			output+=1;
			// output expression
			output = parseBody(e, tmpCursor, output);
			tmpCursor = nextBinding;
			// exit criteria
			if($nextBinding == LIST_END)
			{
				break;
			}
			
		}
		$output = LIST_END;
		output+=1;
		
		// move past list end
		tmpCursor+=1;
		
		// parse the body of the let
		output = parseBody(e, tmpCursor, output);
		
		// skip body
		while(1){
			tmpCursor = skipItem(tmpCursor);
			if($tmpCursor == LIST_END)
			{
				break;
			}
		}
		// move past list end
		tmpCursor+=1;
		// leave scope
		stringListStack_leaveScope(e.sls);
		
		cursor = tmpCursor;
		goto again;
			
		}
		 
	}
	cursor = skipItem(cursor);
	goto loop;
	
exit:
	output = writeRun(start,cursor,output);
	
	// TODO could return cursor!!
	return output;
}

u8$
writeRun(u8 $start, u8 $cursor, u8 $output)
{
	u64 runLength;
	runLength = cursor - start;
	memmove(output, start, runLength);
	output += runLength;
	return output;
}

