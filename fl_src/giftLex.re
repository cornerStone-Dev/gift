/* giftLex.re */

/*!max:re2c*/                        // directive that defines YYMAXFILL
/*!re2c                              // start of re2c block
	
	// comments and white space
	scm = ";" [^\n\x03]* "\n";
	wsp = ([ \r\n\t] | scm )+;

	// integer literals
	oct = "0" [0-7]*;
	dec = [1-9][0-9]*;
	hex = '0x' [0-9A-F]+; // a-f removed
	integer = "-"? (oct | dec | hex);

	// floating literals
	frc = [0-9]* "." [0-9]+ | [0-9]+ ".";
	exp = 'e' [+-]? [0-9]+;
	flt = "-"? (frc exp? | [0-9]+ exp) [fFlL]?;

	// string literals
	string_lit = ["] ([^"\x03] | ([\\] ["]))* ["];
	string_lit_chain = ([^"\n] | ([\\] ["]))* "\n";
	string_lit_end = ([^"\n] | ([\\] ["]))* ["];
	
	// character literals
	char_lit = [#] ['] ([^'\x03] | ([\\] [']))* ['];
	
	identifier = [a-zA-Z_!$%&/*:<>=?\x5e~][a-zA-Z_!$%&/*:<>=?\x5e~0-9+.@-]*;
	
*/                                   // end of re2c block


static s32
giftLexOptions(u8 *YYCURSOR)
{
	u8 *YYMARKER;
	u8 *start;

	start = YYCURSOR;

	/*!re2c                            // start of re2c block **/
	re2c:define:YYCTYPE = "u8";
	re2c:yyfill:enable  = 0;

	* { printf("Invalid Option: %s, try --help.\n",start); return 0; }
	[\x00] { printf("Invalid Option: NULL try --help.\n"); return 0; }
	
	"--help" {
		return 1;
	}
	
	"--repl" {
		return 2;
	}
	
	[a-zA-Z_/0-9] [a-zA-Z_/0-9-]* ".gift" {
		return 3;
	}
	
	*/                                  // end of re2c block
}

static u8*
compactStrings(u8 *YYCURSOR, u8 *out)
{
	//u8 * YYCURSOR;
	u8 *start;
	u64 length=0;
	//YYCURSOR = *YYCURSOR_p;
	//startMangledString = out;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";
	re2c:yyfill:enable  = 0;
	
	* { goto loop; }
	
	string_lit_chain {
		// part of multi-line string literal
		// subtract 1 from length to remove newline
		length = YYCURSOR-start-1;
		// move string into position(first part of chain is moved wastefully)
		memmove(out, start, length);
		out+=length;
		// skip starting tabs to allow formatting
		while(*YYCURSOR=='\t'){YYCURSOR++;}
		goto loop;
	}
	
	string_lit_end {
		// end of potentially multi-line string
		// if the start has been unchanged, we are regular string
		if(length==0)
		{
			// null terminate and return
			*(YYCURSOR-1) = 0;
			// length including ending null
			length = YYCURSOR-start;
			// copy string out
			memmove(out, start, length);
			out+=length;
			return out;
		}
		*(YYCURSOR-1) = 0;
		// length including ending null
		length = YYCURSOR-start;
		// move string into position
		memmove(out, start, length);
		out+=length;
		return out;
	}

	*/                               // end of re2c block
}

// reader
// returns single token OR list of tokens
// lisp requires garbage collection, therefore a "stack" cannot be used
// as a scratchpad
static u8* 
giftRead(S_Environment *e, u8 **YYCURSORp, u8 *cursor, s64 withinList) 
{                                    //
	u8 *YYMARKER;
	u8 *YYCURSOR=*YYCURSORp;
	u8 *start;
	
	//YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		u8 *s=start;
		u8 *f=YYCURSOR;
		while (*s!='\n'){
			s--;
		}
		s++;
		while (*f!='\n'){
			f++;
		}
		f++;
		printf("lex failure ");
		for (u32 ss=0; ss <(f-s); ss++){
			fputc ( s[ss], stdout);
		}
		fputc ( '\n', stdout);
		goto loop;
	}
	[\x03] { /* *t = LIST_END_OF_BUFF; */ return 0; }
	
	wsp {
		//~ while (start!=YYCURSOR){
			//~ if(*start=='\n'){
				//~ c->line_num+=1;
				//~ printf("wsp, %d\n", *line_num);
			//~ }
			//~ start++;
		//~ }
		goto loop;
	}
 
	integer {
		*YYCURSORp=YYCURSOR;
		return listWriteInt(cursor, strtol(start, NULL, 0));
	}
	
	flt {
		*YYCURSORp=YYCURSOR;
		return listWriteFloat(cursor, atof(start));
	}
	
	string_lit { 
		start++;
		//u8 *string = start;
		*YYCURSORp=YYCURSOR;
		*cursor = listStringType();
		cursor+=1;
		// concatenate all multiline strings
		return compactStrings(start, cursor);
	}

	char_lit {
		*cursor = listCharType();
		cursor+=1;
		switch(*(start+2))
		{
			case '\\':
			switch(*(start+3))
			{
				case '\'':
				*cursor = 0x27;
				break;
				case 'n':
				*cursor = 0x0A;
				break;
				case 'r':
				*cursor = 0x0D;
				break;
				case 't':
				*cursor = 0x09;
				break;
			}
			break;
			default:
			*cursor = *(start+2);
			break;
		}
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	"#t" {
		*cursor = LIST_TRUE;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
	"#f" {
		*cursor = LIST_FALSE;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
	"()" {
		*cursor = LIST_NIL;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	")" {
		*cursor = LIST_END;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	"'" { // QUOTE very tough
		*cursor = LIST_QUOTE;
		cursor+=1;
		goto loop;
	}

	"define" {
		*cursor = LIST_DEFINE;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	"set!" {
		*cursor = LIST_SET;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	"if" {
		*cursor = LIST_IF;
		cursor+=1;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	"+" {
		*cursor = LIST_PLUS;
		cursor++;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
	"-" {
		*cursor = LIST_SUBT;
		cursor++;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
	"*" {
		*cursor = LIST_MULT;
		cursor++;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
	"/" {
		*cursor = LIST_DIVI;
		cursor++;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
	"%" {
		*cursor = LIST_REMA;
		cursor++;
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	"(" {
		//u64 len;
		u32 paren_count;
		if(withinList)
		{
			*cursor = LIST_START;
			cursor++;
			*YYCURSORp=YYCURSOR;
			return cursor;
		}
		// need to gather rest of the list
		*cursor = LIST_START;
		cursor++;
		//list_continue:
		paren_count=0;
		do {
			cursor = giftRead(e, &YYCURSOR, cursor, 1);
			if(cursor==0){
				printf("Error Input: Buffer ended before list.\n");
				// TODO error handling?
				return 0;
			}
			u8 prevToken = *(cursor-1);
			if(prevToken==LIST_START)
			{
				paren_count++;
			} else if(prevToken==LIST_END) {
				if(paren_count)
				{
					paren_count--;
				} else {
					break;
				}
			}
		} while (1);
		*YYCURSORp=YYCURSOR;
		return cursor;
	}

	//~ identifier {
		//~ LIST_writeString(&t, start, YYCURSOR-start+2);
		//~ *(t-(YYCURSOR-start+2))=LIST_SYMBOL;
		//~ //printf("variable %s\n", (t-(YYCURSOR-start+1)));
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	*/                               // end of re2c block
}  



