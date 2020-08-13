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
u8$
giftLambda(S_Environment $e, u8 $cursor)
{
	u8 $output = getHeapCursor(e);
	cursor+=1;
	if($cursor== LIST_START)
	{
		
	}
	return 0;
}
