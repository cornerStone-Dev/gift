/* giftTest.c */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <math.h>
#include <termios.h>

#include "hashTable/hashTable.h"
#include "stringListStack/stringListStack.h"

#include "src/fl_std.h"
#include "src/interface.h"

/* function prototypes */
//~ static void
//~ save_function_addr(u8 *start, u64 len, u8 *addr);
//~ static void
//~ save_variable(u8 *start, u64 len, s64 val);
//~ static void
//~ garbage_collect(u64 last_requested_size);
//~ static u64
//~ LIST_getLen(const u8 *input);
//~ static void *
//~ heap_malloc(size_t bytes);
//~ static void *
//~ heap_realloc(u8 *ptr, size_t bytes);
//~ static u8 * 
//~ LIST_copy(u8 *YYCURSORx);
//~ static u8 *
//~ eval_set_expr(u8 * restrict t, u8 * restrict start, u8 **end);
//~ static u8 *
//~ eval(u8 * restrict t, u8 * restrict start, u8 **end);


//~ static s32
//~ add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l);
//~ static s32
//~ is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l);
//~ static u8 *
//~ indx_within(ScopeList * restrict str_l, u32  indx, u8 * restrict output);
//~ static void
//~ enter_scope(ScopeList * restrict scope_l);
//~ static inline void
//~ leave_scope(ScopeList * restrict scope_l);



/* globals */

//~ static FILE * outputFile,* typeProtoFile, * typesFile,
	//~ * funcProtoFile, * globalsFile, * interfaceFile, * includesFile;

//~ static void print_scheme(u8 *t)
//~ {
	//~ LIST_print(&t, 0);
//~ }

//#define INTPUT_FILE "input/weeklystat.c"

int main(int argc, char **argv)
{
	S_Environment *e;
	giftInit(&e);
	giftCommandLine(e, argc, (u8**)argv);
	return 0;
	#if 0
	unsigned char * data=0, *output_string_base;
	unsigned char output_string[65536] = {0};
	unsigned char stringBuffer[4096] = {0};
	unsigned char *strBuff;
	Data stack[384]={0};
	Data vars[512]={0};
	//unsigned char file_name_buff[512] = {0};
	unsigned char * output = output_string;
	//u8 dirName[512];
	Data cstack[128]={0};
	//u8 * dirName_p;
	u8 *tmp_token;
	u32 x, inputLen,z;
	Context c = {0};
	u8 tokens[2048] = {0};
	FILE * pFile;
	DIR *d=0;
	struct dirent *dir;
	output_string[0]=3;
	output_string_base=&output_string[1];
	c.out = output;
	c.buff_start = output_string_base;
	c.buff = output_string_base;
	c.stk = stack;
	c.stk_start = stack;
	c.stk_end = &stack[374];
	c.vars = vars;
	c.cstk = cstack;
	c.tokens = tokens;
	strBuff = stringBuffer;
	
	var_data.v = malloc(4096);
	var_data.hw=511;
	
	heap_data.h = malloc(128*1024*1);
	heap_data.t=(128*1024*1)-1;
	heap_data.cache=(u8*)5; // set to garbage

	u32 i=1;
	if (argc > 1)
	{
		for (; i<argc; i++)
		{
			x = lex_options((u8 *)argv[i]);
			
			switch (x){
				case 0:
				return 0;
				break;
				case 1:
				
				while (1)
				{

					raw_begin();
					inputLen = fith_fgets(strBuff, 4096, data);
					raw_end();

					if (inputLen!= 0 )
					{
						if (!(strncmp((const char *)strBuff, ".exit", 5)))
						{
							return 0;
						}
						//printf("strBuff: %s\n", strBuff);
						if (!(strncmp((const char *)strBuff, ".dump", 5)))
						{
							//*(c.buff_start) = '\000';
							//printf("xxx: %s end\n", output_string_base);
							//printf("%s",output_string_base);
							print_code(output_string_base, c.buff_start-output_string_base);
							continue;
						}
						if (!(strncmp((const char *)strBuff, ".save", 5)))
						{
							*(c.buff_start) = '\000';
							pFile = fopen ( "session.fith", "w" );
							if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
							fwrite (output_string_base,
								sizeof(char),
								c.buff_start-output_string_base,
								pFile);
							fflush (pFile);
							fclose (pFile);
							printf("session.fith saved\n");
							continue;
						}
						
						data = c.buff_start;
						//printf("data: %X\n", *strBuff);
						z=0;
						if((*strBuff!='\n')&&(*strBuff!='.')){
							while(strBuff[z]!=3){
								*c.buff_start=strBuff[z];
								z++;
								c.buff_start++;
							}
						}
						//printf("data: %X\n", *strBuff);
						
						*(c.buff_start) = '\003';
						
						c.yycur=data;
						tmp_token = giftRead(&c.yycur, &c, &tokens[5]);
						//printf("token## %ld\n",tmp_token);
						*tmp_token=0;
						tmp_token++;
						print_scheme(eval(&tokens[5], tokens, &tmp_token));
						data=c.yycur;
						//print_scheme(tokens[0]);
						printf("\n");
						*(c.buff_start) = 3;
					}
				}
				break;
				case 2:
				printf("target file: %s\n",argv[i]);
				c.is_def=1;
				sprintf((char *)c.file_name_buff, "%s", argv[i]);
				goto one_file;
				one_file_return:
				break;
			}
			c.buff_start = output_string_base;
		}
	}
	
	if (c.is_def)
	{
		return 0;
	}
	
	/** Set up parser **/
	//~ pEngine = &sEngine;
	//~ ParseInit(pEngine, &c);
	
	/* open current directory */
	d = opendir("scm_src");
	if (d==0)
	{
		//printf("NO DIR!!!\n");
		return -1;
	}
	
	
	//printf("about to start\n");
	while ( ((dir = readdir(d)) != NULL) /*&& (strstr(dir->d_name, ".c")!=0)*/ )
	{
		//printf("Got in\n");
		if ( (strstr(dir->d_name, ".scm")!=0) ) {
		//printf("%s\n", dir->d_name);
		c.line_num=1;
		//~ output = (uint8_t *)stpcpy((char *)output, "/* src/");
		//~ output = (uint8_t *)stpcpy((char *)output, dir->d_name);
		//~ output = (uint8_t *)stpcpy((char *)output, " */\n");
		//~ fwrite (output_string,
				//~ sizeof(char),
				//~ output-output_string,
				//~ outputFile);
		//~ output = output_string;
		
		c.out = output;
		
		sprintf((char *)c.file_name_buff, "scm_src/%s", dir->d_name);

		one_file:
		
		data = load_file(c.file_name_buff, 0);
		

		//tmp_token = lex(&data, &c.line_num, &c, &tokens[c.token_index]);


		}
		if (c.is_def)
		{
			goto one_file_return;
		}
	}
	
	/* close directory */
	closedir(d);
	
	/* flush file out of cache and close all files */
	//~ fflush (outputFile); 
	//~ fclose (outputFile);
	
	
	/*** De-comission parser * */
	//ParseFinalize(pEngine);
	s32 rcode;
	if(c.printed_error){
		rcode = 1;
	} else {
		rcode = 0;
	}
	return rcode;
	#endif
}

//~ static s32
//~ add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l)
//~ {
	//~ u8 * cursor = str_l->cursor_stack[str_l->scopeIdx];
	//~ u8 * end =    str_l->end;
	
	//~ if ( (end - cursor) < l )
	//~ {
		//~ printf ("ScopeList: out of room\n");
		//~ return -1;
	//~ }
	
	//~ /* copy string to type_table */
	//~ for (s32 i=0;i<l;i++)
	//~ {
		//~ *cursor = *s;
		//~ cursor++;
		//~ s++;
	//~ }
	
	//~ /* store length of string */
	//~ *cursor = l;
	//~ cursor++;
	
	
	//~ //printf("*str_l->count =%d\n", str_l->item_count_stack[str_l->scopeIdx]);
	//~ str_l->cursor_stack[str_l->scopeIdx] = cursor;
	//~ str_l->item_count_stack[str_l->scopeIdx]+=1;
	
	//~ return str_l->item_count_stack[str_l->scopeIdx]-1;
//~ }

//~ /* search type table for IDENT */
//~ static s32
//~ is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l)
//~ {
	//~ u8 * cursor = str_l->cursor_stack[str_l->scopeIdx]-1;
	//~ u8 * start =  str_l->table;
	//~ u8 * string = s;
	//~ u8 * len_cursor;
	//~ s32  idx = str_l->item_count_stack[str_l->scopeIdx];
	//~ u8   len;
	
	
	//~ /* search backwards, view as a stack */
	//~ while ( cursor >= start )
	//~ {
		//~ len = (*cursor);
		//~ len_cursor = cursor;
		//~ idx--;
		//~ if ( l != len )
		//~ {
			//~ /* skip to next one */
			//~ cursor = len_cursor-len-1;
			//~ continue;
		//~ }
		//~ cursor-=len;
		//~ /* matching length */
		//~ for (s32 i=0;i<l;i++)
		//~ {
			//~ if ( (*cursor)!= (*s) )
			//~ {
				//~ /* skip to next string */
				//~ cursor = len_cursor-len-1;
				//~ /* reset string */
				//~ s = string;
				//~ break;
			//~ }
			//~ cursor++;
			//~ s++;
		//~ }
		//~ /* see if success */
		//~ if ( s != string )
		//~ {
			//~ //printf(" found something!!\n");
			//~ return idx;
		//~ }
	//~ }
	//~ /* we have found nothing */
	//~ //printf(" found nothing!!\n");
	//~ return -1;
//~ }

/* search type table at index */
//~ static u8 *
//~ indx_within(ScopeList * restrict str_l, u32 indx, u8 * restrict output)
//~ {
	//~ u8 * cursor = str_l->cursor_stack[str_l->scopeIdx]-1;
	//~ s32  idx = str_l->item_count_stack[str_l->scopeIdx];
	//~ u8   len=0;
	
	//~ if (indx > (idx-1) ) {
		//~ return output;
	//~ }
	
	//~ /* one comparision */
	//~ while ( idx > 0 )
	//~ {
		//~ len = (*cursor);
		//~ idx--;
		//~ if ( idx != indx )
		//~ {
			//~ /* skip to next one */
			//~ cursor = cursor-len-1;
			//~ continue;
		//~ }
		//~ break;
	//~ }
	//~ cursor-=len;
	//~ /* copy out string */
	//~ for (s32 i=0;i<len;i++)
	//~ {
		//~ *output = *cursor;
		//~ cursor++;
		//~ output++;
	//~ }
	
	//~ return output;
//~ }

//~ static void
//~ enter_scope(ScopeList * restrict scope_l)
//~ {
	//~ u32 idx;
	//~ scope_l->scopeIdx++;
	//~ idx = scope_l->scopeIdx;
	//~ /* copy end of previous scope to new scope */
	//~ scope_l->item_count_stack[idx] = scope_l->item_count_stack[idx-1];
	//~ scope_l->cursor_stack[idx] = scope_l->cursor_stack[idx-1];
	//~ //printf("entering scope!%d\n",scope_l->scopeIdx);
//~ }

//~ static inline void
//~ leave_scope(ScopeList * restrict scope_l)
//~ {
	//~ scope_l->scopeIdx--;
	//~ //printf("leaving scope!%d\n",scope_l->scopeIdx);
//~ }


//~ const u8 base64EncodeLookup[64]= 
	//~ { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	  //~ 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	  //~ 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	  //~ 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_' };
//~ static u32 //TODO reword this function. It works just not pretty
//~ base64encode(u8 * output, u32 input)
//~ {
	//~ u8 temp_output[16];
	//~ u8 * output_p = output;
	//~ s32 count = 1;
	//~ s32 tmp_value;
	
	//~ temp_output[count] = '\0';
	//~ do{
		//~ tmp_value = input%64;
		//~ input = input / 64;
		//~ count+=1;
		//~ temp_output[count] = (base64EncodeLookup[tmp_value]);
	//~ }while(input);
	
	//~ while (count){
		//~ *output_p = temp_output[count];
		//~ output_p+=1;
		//~ count-=1;
	//~ }
	//~ return (output_p - output-1);
//~ }

//~ static u32 
//~ safe_atol_64(const u8 ** strp) /* 18 decimal digits only */
//~ {
	//~ static const u8 base64intLookup[78]= 
	//~ {  62,   0,   0,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,
	    //~ 0,   0,   0,   0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11, 
	   //~ 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,
	    //~ 0,   0,  63,   0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,
	   //~ 38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51 };
	
	//~ const u8 * str = *strp;
	//~ u32 cnt=0;
	//~ u32 val=0;
	
	//~ while ( (*str >= '-') && (*str <= 'z') && (cnt<10) ) {
		//~ val = (val * 64) + (base64intLookup[(*str - '-')]);
		//~ ++str;
		//~ ++cnt;
	//~ }
	//~ *strp=str;
	//~ return val;
//~ }
