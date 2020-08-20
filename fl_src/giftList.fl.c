// giftList.fl.c

s64
readListInt(u8 $input, s64 len)
{
	u64 listInt=0;
	
	len-=1;
	for(; len >= 0; len-=1)
	{
		listInt += (((u64)($input))<<(len*8));
		input+=1;
	}
	
	return (s64)listInt;
}

u8$
listWriteInt(u8 $out, s64 value)
{
	u64 raw = (u64) value;
	s64 x;
	u32 lz;
	// count leading zeros
	lz = __builtin_clzl(raw);
	// divide by eight to get number of bytes that are zero
	lz/=8;
	// subtract from 8 to get number of bytes I need to write
	x = 8 - lz;
	// write out LIST_INT enumeration
	$out = x;
	out+=1;
	// subtract one to work better for loop
	x-=1;
	// write out the number
	for(; x >= 0; x-=1)
	{
		$out = ( raw >> (x*8) ) & 0xFF;
		out+=1;
	}
	return out;
}

f64
readListFloat(u8 $input)
{
	U_Data data;
	u64 listInt=0;
	s64 len = 7;
	
	do{
		listInt += (((u64)($input))<<(len*8));
		input+=1;
		len-=1;
	}while(len>=0);
	
	data.i = listInt;
	return data.d;
}

u8$
listWriteFloat(u8 $out, f64 value)
{
	U_Data data;
	data.d = value;
	u64 raw = (u64) data.i;
	s64 x = 7;
	
	
	$out = LIST_FLOAT;
	out+=1;
	
	do{
		$out = ( raw >> (x*8) ) & 0xFF;
		out+=1;
		x-=1;
	} while(x>=0);

	return out;
}

u8$
skipItem(u8 $cursor)
{
	u64 parenCount=0;
	u64 withinList=0;
	
	goto skipCheck;

loop: // label for looping
	
	if(withinList==0){
		return cursor;
	}
	
skipCheck:

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
		cursor += ($cursor) + 1;
		goto loop;
		
		case LIST_FLOAT:
		cursor += 9;
		goto loop;
		
		case LIST_TRUE:
		case LIST_FALSE:
		case LIST_NULL:
		cursor += 1;
		goto loop;
		
		case LIST_CHAR_LIT:
		cursor += 2;
		goto loop;
		
		case LIST_STRING_N:
		cursor+=1;
		cursor += strlen(cursor)+1;
		goto loop;
		
		case LIST_SYMBOL:
		cursor+=1;
		cursor += ($cursor)+2;
		goto loop;
		
		case LIST_QUOTE:
		cursor+=1;
		goto skipCheck;
		
		case LIST_START:
		cursor+=1;
		if(withinList)
		{
			parenCount+=1;
		} else {
			// need to gather rest of the list
			withinList = 1;
		}
		goto skipCheck;
		
		case LIST_END:
		cursor+=1;
		if(parenCount)
		{
			parenCount-=1;
		} else {
			withinList = 0;
		}
		goto loop;
		
		case LIST_DEFINE:
		case LIST_SET:
		case LIST_IF: 
		case LIST_PLUS:
		case LIST_SUBT:
		case LIST_MULT:
		case LIST_DIVI:
		case LIST_REMA:
		case LIST_LAMBDA:
		case LIST_DOT:
		case LIST_REG0:
		case LIST_REG1:
		case LIST_REG2:
		case LIST_REG3:
		case LIST_REG4:
		case LIST_REG5:
		case LIST_REG6:
		case LIST_REG7:
		cursor += 1;
		goto loop;
		
		case LIST_PROCEDURE:
		cursor += 2;
		goto loop;
		
		default:
		printf("undefined value to skip!!\n");
		return cursor;
	}
}

//~ static void
//~ LIST_writeString(u8 **outx, const u8 *string, u32 len)
//~ {
	//~ u8 *out = *outx;
	//~ *out = LIST_STRING_N;
	//~ out++;
	//~ memcpy(out, string, len-2);
	//~ out+=len-2;
	//~ *out =0; //null terminate long strings
	//~ out++;
	//~ *outx = out;
	//~ return;
//~ }
