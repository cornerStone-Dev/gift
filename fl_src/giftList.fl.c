// giftList.fl.c

u8$
listWriteInt(u8 $out, s64 value)
{
	u64 raw = (u64) value;
	u32 lz, x;
	// count leading zeros
	lz = __builtin_clzl(raw);
	// divide by eight to get number of bytes I need write out
	lz/=8;
	
	x = 8 - lz;
	
	// write out LIST_INT enumeration
	$out = x;
	out+=1;
	
	// write out the number
	for(; x > 0; x-=1)
	{
		$out = ( raw >> ((x-1)*8) ) & 0xFF;
		out+=1;
	}
	return out;
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

u8
listStringType(void)
{
	return LIST_STRING_N;
}

u8
listCharType(void)
{
	return LIST_CHAR_LIT;
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
