/* giftLex.re */

/*!max:re2c*/                        // directive that defines YYMAXFILL
/*!re2c                              // start of re2c block
	
	// comments and white space
	scm = ";" [^\n\x00]* "\n";
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
	string_lit = ["] ([^"\x00] | ([\\] ["]))* ["];
	string_lit_chain = ([^"\n] | ([\\] ["]))* "\n";
	string_lit_end = ([^"\n] | ([\\] ["]))* ["];
	
	// character literals
	char_lit = [#] ['] ([^'\\\x00] | ([\\] ['nrt\\])) ['];
	
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
giftRead(u8 **YYCURSORp, u8 *cursor, s64 withinList) 
{                                    //
	u8 *YYMARKER;
	u8 *YYCURSOR=*YYCURSORp;
	u8 *start;
	u64 paren_count=0;
	
	goto skipCheck;

loop: // label for looping within the lexxer
	
	if(withinList==0){
		*YYCURSORp=YYCURSOR;
		return cursor;
	}
	
skipCheck:
	
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL

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
		goto skipCheck;
	}
	[\x00] { /* *t = LIST_END_OF_BUFF; */ return 0; }
	
	wsp {
		//~ while (start!=YYCURSOR){
			//~ if(*start=='\n'){
				//~ c->line_num+=1;
				//~ printf("wsp, %d\n", *line_num);
			//~ }
			//~ start++;
		//~ }
		goto skipCheck;
	}
 
	integer {
		cursor = listWriteInt(cursor, strtol(start, NULL, 0));
		goto loop;
	}
	
	flt {
		cursor = listWriteFloat(cursor, atof(start));
		goto loop;
	}
	
	string_lit { 
		start++;
		*cursor = LIST_STRING_N;
		cursor+=1;
		// concatenate all multiline strings
		cursor = compactStrings(start, cursor);
		goto loop;
	}

	char_lit {
		*cursor = LIST_CHAR_LIT;
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
				case '\\':
				*cursor = 0x5C;
				break;
			}
			break;
			default:
			*cursor = *(start+2);
			break;
		}
		cursor+=1;
		goto loop;
	}

	"#t" {
		*cursor = LIST_TRUE;
		cursor+=1;
		goto loop;
	}
	
	"#f" {
		*cursor = LIST_FALSE;
		cursor+=1;
		goto loop;
	}
	
	"#undefined" {
		*cursor = LIST_UNDEFINED;
		cursor+=1;
		goto loop;
	}
	
	"()" {
		*cursor = LIST_NULL;
		cursor+=1;
		goto loop;
	}

	")" {
		*cursor = LIST_END;
		cursor+=1;
		if(paren_count)
		{
			paren_count--;
		} else {
			withinList = 0;
		}
		goto loop;
	}

	"'" { // QUOTE very tough
		*cursor = LIST_QUOTE;
		cursor+=1;
		goto skipCheck;
	}

	"def" {
		*cursor = LIST_DEFINE;
		cursor+=1;
		goto loop;
	}

	"set" {
		*cursor = LIST_SET;
		cursor+=1;
		goto loop;
	}

	"if" {
		*cursor = LIST_IF;
		cursor+=1;
		goto loop;
	}
	
	".\\" {
		*cursor = LIST_LAMBDA;
		cursor+=1;
		goto loop;
	}

	"+" {
		*cursor = LIST_PLUS;
		cursor++;
		goto loop;
	}
	
	"-" {
		*cursor = LIST_SUBT;
		cursor++;
		goto loop;
	}
	
	"*" {
		*cursor = LIST_MULT;
		cursor++;
		goto loop;
	}
	
	"/" {
		*cursor = LIST_DIVI;
		cursor++;
		goto loop;
	}
	
	"%" {
		*cursor = LIST_REMA;
		cursor++;
		goto loop;
	}

	"=" {
		*cursor = LIST_EQUALS;
		cursor++;
		goto loop;
	}

	">" {
		*cursor = LIST_GREATER_THAN;
		cursor++;
		goto loop;
	}

	"<" {
		*cursor = LIST_LESS_THAN;
		cursor++;
		goto loop;
	}

	"<=" {
		*cursor = LIST_LESS_THAN_OR_EQUAL;
		cursor++;
		goto loop;
	}

	">=" {
		*cursor = LIST_GREATER_THAN_OR_EQUAL;
		cursor++;
		goto loop;
	}

	"not" {
		*cursor = LIST_NOT;
		cursor++;
		goto loop;
	}

	"." {
		*cursor = LIST_DOT;
		cursor++;
		goto loop;
	}

	"(" {
		if(withinList)
		{
			*cursor = LIST_START;
			cursor++;
			paren_count++;
			goto loop;
		}
		// need to gather rest of the list
		*cursor = LIST_START;
		cursor++;
		withinList = 1;
		goto loop;
	}

	// identifiers are stored differently then strings, assumed < 255 length
	identifier {
		u64 length = YYCURSOR-start;
		*cursor = LIST_SYMBOL;
		cursor+=1;
		*cursor = length;
		cursor+=1;
		memmove(cursor, start, length);
		cursor[length] = 0;
		cursor+=length+1;
		goto loop;
	}

	*/                               // end of re2c block
}  



