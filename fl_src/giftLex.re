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
	mangled_string_lit = ["] ([^"\x00\x03] | ([\\] ["]))* "\x00";
	
	// character literals
	char_lit = [#] ['] ([^'\x03] | ([\\] [']))* ['];
	
	identifier = [a-zA-Z_!$%&/*:<>=?\x5e~][a-zA-Z_!$%&/*:<>=?\x5e~0-9+.@-]*;
	
*/                                   // end of re2c block


static int lex_options(u8 * YYCURSOR)
{
	u8 * YYMARKER;
	u8 * start;
	
	//YYCURSOR = *YYCURSOR_p;

//loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { printf("Invalid Option: %s, Operation aborted\n",start); return 0; } //   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal
	
	"--repl" {
		return 1;
	}
	
	//[a-zA-Z_/0-9-]+ ".fith" {
	[a-zA-Z_/0-9] [a-zA-Z_/0-9-]* ".gift" {
		return 2;
	}
	
	*/                               // end of re2c block
}

static s32
lex_string_lit_chain(u8 ** YYCURSOR_p)
{                                    //
	u8 * YYCURSOR;
	u8 *start;
	u8 *startMangledString;
	YYCURSOR = *YYCURSOR_p;
	startMangledString = YYCURSOR;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";
	re2c:yyfill:enable  = 0;

	* { goto loop; }//   default rule
	
	string_lit_chain {
		*(YYCURSOR-1) = 0;
		startMangledString = (u8*)stpcpy((char *)startMangledString,
										(const char *)start);
		goto loop;
	}
	
	string_lit_end {
		if(startMangledString==start)
		{
			*(YYCURSOR-1) = 0; // null terminate
			*YYCURSOR_p = YYCURSOR-1;// set to null terminator
			return 0;
		}
		*(YYCURSOR-1) = 0;
		startMangledString = (u8*)stpcpy((char *)startMangledString,
										(const char *)start);
		*YYCURSOR_p = startMangledString; // set to null terminator
		return 1;
	}

	*/                               // end of re2c block
}

// reader
// returns single token OR list of tokens

static u8 * 
giftRead(u8 **YYCURSORp, void *c, u8 *t) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	u8 *YYCURSOR=*YYCURSORp;    // YYCURSOR is defined as a local variable
	u8 *buff_start=t;
	u8 * start;
	
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
		s++;
		printf("lex failure ");
		for (u32 ss=0; ss <(f-s); ss++){
			fputc ( s[ss], stdout);
		}
		fputc ( '\n', stdout);
		goto loop; 
	} //   default rule with its semantic action start =YYCURSOR;
	//~ [\x03] { *t = LIST_END_OF_BUFF; return t; 
	//~ }             // EOF rule with 0x03 sentinal
	
	//~ wsp {
		//~ while (start!=YYCURSOR){
			//~ if(*start=='\n'){
				//~ c->line_num+=1;
				//~ //printf("wsp, %d\n", *line_num);
			//~ }
			//~ start++;
		//~ }
		//~ goto loop;
	//~ }
 
	//~ integer {
		//~ LIST_writeInt(&t, strtol((const char *)start, NULL, 0));
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ flt {
		//~ LIST_writeFloat(&t, atof( (const char *)start) );
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ string_lit { 
		//~ start++;
		//~ u8 *string = start;
		//~ // concatenate all multiline strings
		//~ lex_string_lit_chain(&start); // start is null terminator
		//~ LIST_writeString(&t, string, start-string+2);
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ mangled_string_lit {
		//~ start++;
		//~ LIST_writeString(&t, start, strlen((const char *)start)+2);
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ char_lit {
		//~ *t = LIST_CHAR_LIT;
		//~ t++;
		//~ switch(*(start+2))
		//~ {
			//~ case '\\':
			//~ switch(*(start+3))
			//~ {
				//~ case '\'':
				//~ *t = 0x27;
				//~ break;
				//~ case 'n':
				//~ *t = 0x0A;
				//~ break;
				//~ case 'r':
				//~ *t = 0x0D;
				//~ break;
				//~ case 't':
				//~ *t = 0x09;
				//~ break;
			//~ }
			//~ break;
			//~ default:
			//~ *t = *(start+2);
			//~ break;
		//~ }
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "#t" {
		//~ *t = LIST_TRUE;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "#f" {
		//~ *t = LIST_FALSE;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "()" {
		//~ *t = LIST_NIL;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "(" {
		//~ //u64 len;
		//~ u32 paren_count;
		//~ if(c->in_list==0)
		//~ {
			//~ // need to gather rest of the list
			//~ *t = LIST_START;
			//~ t++;
			//~ //list_continue:
			//~ paren_count=0;
			//~ c->in_list=1;
			//~ do {
				//~ t = giftRead(&YYCURSOR, c, t);
				//~ //printf("token## %d\n",(t[c->token_index].t>>56));
				//~ if(*(t-1)==LIST_START)
				//~ {
					//~ paren_count++;
				//~ } else if(*(t-1)==LIST_END) {
					//~ if(paren_count==0)
					//~ {
						//~ break;
					//~ } else {
						//~ paren_count--;
					//~ }
				//~ } else if(*t==LIST_END_OF_BUFF) {
					//~ printf("Error Input: Buffer ended before list.\n");
					//~ c->in_list=0;
					//~ t = buff_start;
					//~ *t = LIST_FALSE;
					//~ t++;
					//~ *YYCURSORp=YYCURSOR;
					//~ return t;
				//~ }
			//~ } while (1);
			//~ c->in_list=0;
			//~ *YYCURSORp=YYCURSOR;
			//~ return t;
		//~ }
		//~ *t = LIST_START;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ ")" {
		//~ *t = LIST_END;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "'" { // this could be a list with recursive call to lex...
		//~ *t = LIST_QUOTE;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ goto loop;
	//~ }

	//~ "define" {
		//~ *t = LIST_DEFINE;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "set!" {
		//~ *t = LIST_SET;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ "if" {
		//~ *t = LIST_IF;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ "+" {
		//~ *t = LIST_PLUS;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ "-" {
		//~ *t = LIST_SUBT;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ "*" {
		//~ *t = LIST_MULT;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ "/" {
		//~ *t = LIST_DIVI;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }
	
	//~ "%" {
		//~ *t = LIST_REMA;
		//~ t++;
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	//~ identifier {
		//~ LIST_writeString(&t, start, YYCURSOR-start+2);
		//~ *(t-(YYCURSOR-start+2))=LIST_SYMBOL;
		//~ //printf("variable %s\n", (t-(YYCURSOR-start+1)));
		//~ *YYCURSORp=YYCURSOR;
		//~ return t;
	//~ }

	*/                               // end of re2c block
}  



