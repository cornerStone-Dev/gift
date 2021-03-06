// giftPrintf.fl.c

#define LABEL0_8 case 0 ... 8

void
giftPrint(u8 $value)
{
	//u8 $start;
	u64 parenCount=0;
	u64 withinList=0;
	u64 length;
	
	goto skipCheck;

loop: // label for looping
	
	if(withinList==0){
		//printf("\n");
		return;
	}
	
	if( (($value)!=LIST_END) )
	{
		fputc (' ', stdout);
	}
	
skipCheck:
	
	//start = value;
	
	switch($value)
	{
		LABEL0_8:
		printf("%ld",readListInt(value+1, $value));
		value+=$value+1;
		goto loop;
		
		case LIST_FLOAT:
		printf("%f",readListFloat(value+1));
		value+=9;
		goto loop;
		
		case LIST_TRUE:
		printf("#t");
		value+=1;
		goto loop;
		
		case LIST_FALSE:
		printf("#f");
		value+=1;
		goto loop;
		
		case LIST_NULL:
		printf("()");
		value+=1;
		goto loop;
		
		case LIST_START:
		printf("(");
		value+=1;
		if(withinList)
		{
			parenCount+=1;
		} else {
			// need to gather rest of the list
			withinList = 1;
		}
		goto skipCheck;
		
		case LIST_END:
		printf(")");
		value+=1;
		if(parenCount)
		{
			parenCount-=1;
		} else {
			withinList = 0;
		}
		goto loop;
		
		case LIST_CHAR_LIT:
		value+=1;
		switch($value)
		{
			case 0x27:
			//t->t = 0x27;
			printf("#'\\''");
			break;
			case 0x0A:
			//t->t = 0x0A;
			printf("#'\\n'");
			break;
			case 0x0D:
			//t->t = 0x0D;
			printf("#'\\r'");
			break;
			case 0x09:
			//t->t = 0x09;
			printf("#'\\t'");
			break;
			default:
			printf("#'%c'",$value);
			break;
		}
		value+=1;
		goto loop;
		
		case LIST_SYMBOL:
		value+=1;
		length = $value;
		value+=1;
		printf("%s", value);
		value+=length+1;
		goto loop;
		
		case LIST_STRING_N:
		value+=1;
		length = strlen(value)+1; // including null
		printf("\"%s\"", value);
		value+=length;
		goto loop;
		
		case LIST_QUOTE:
		printf("'");
		value+=1;
		goto loop;
		
		case LIST_DEFINE:
		printf("define");
		value+=1;
		goto loop;
		
		case LIST_SET:
		printf("set!");
		value+=1;
		goto loop;
		
		case LIST_IF:
		printf("if");
		value+=1;
		goto loop;
		
		case LIST_PLUS:
		printf("+");
		value+=1;
		goto loop;
		
		case LIST_SUBT:
		printf("-");
		value+=1;
		goto loop;
		
		case LIST_MULT:
		printf("*");
		value+=1;
		goto loop;
		
		case LIST_DIVI:
		printf("/");
		value+=1;
		goto loop;
		
		case LIST_REMA:
		printf("%%");
		value+=1;
		goto loop;

		case LIST_EQUALS:
		printf("=");
		value+=1;
		goto loop;

		case LIST_GREATER_THAN:
		printf(">");
		value+=1;
		goto loop;

		case LIST_LESS_THAN:
		printf("<");
		value+=1;
		goto loop;

		case LIST_LESS_THAN_OR_EQUAL:
		printf("<=");
		value+=1;
		goto loop;

		case LIST_GREATER_THAN_OR_EQUAL:
		printf("<=");
		value+=1;
		goto loop;

		case LIST_NOT:
		printf("not");
		value+=1;
		goto loop;

		case LIST_LET:
		printf("let");
		value+=1;
		goto loop;

		case LIST_LAMBDA:
		printf("lambda");
		value+=1;
		goto loop;
		
		case LIST_DOT:
		printf(".");
		value+=1;
		goto loop;
		
		case LIST_PROCEDURE:
		printf("#<procedure argCode=%02X>", $(value+1));
		value+=2;
		goto loop;
		
		case LIST_REG0:
		case LIST_REG1:
		case LIST_REG2:
		case LIST_REG3:
		case LIST_REG4:
		case LIST_REG5:
		case LIST_REG6:
		case LIST_REG7:
		printf("register[%d]", $value-LIST_REG0);
		value+=1;
		goto loop;
		
		case LIST_UNDEFINED:
		printf("<Undefined Value>");
		value+=1;
		goto loop;
		
		default:
		printf("<Unknown Value: context [%02X][%d][%d]>", $(value-2), $value, $(value+1));
		value+=1;
		goto loop;
	}
}

