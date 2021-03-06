// giftData.fl.c

void$
getHeapCursor(S_Environment $e)
{
	u8 $p;
	u64 currentIndex;
	
	currentIndex = e.heapIndex;
	if ( (currentIndex+(8*1024)) > e.heapTop) // garbage collection time
	{
		//garbage_collect();
		printf("should garbage collect!\n");
	}
	p = (@e.heap[currentIndex])+2;
	//e.cache = p;
	e.heapIndex+=4;
	return (void $)p;
}

// forces things to crush implied stack
void
finalizeHeapCursor(S_Environment $e, u8 $start, u8 $end)
{
	//u8  $cache;
	u16 $size;
	u64 amountWritten;
	
	//cache = e.cache;
	amountWritten = (end - start);
	amountWritten = (amountWritten+5)/4;
	// write size at start
	size = (u16$)(start-2);
	$size = amountWritten;
	// Index should be 8 above the size and 4 above the start
	e.heapIndex = (start+2)-e.heap;
	e.heapIndex+=(amountWritten-1)*4; // open cursor adds 8
	
	u8 $transformedSource=start-2;
	for(u32 x =0; x<24;x+=1)
	{
		printf("[%02X]",transformedSource[x]);
	}
	printf("\n");
	printf("heapIndex=%ld\n",e.heapIndex);
}

//~ void $
//~ giftMalloc(u64 bytes)
//~ {
	//~ void $p;
	//~ u64 next_index;
	
	//~ // align by 8 bytes;
	//~ bytes = (bytes+7)/8*8;
	//~ next_index = heap_data.i+bytes;
	//~ while (next_index>heap_data.t) // garbage collection time
	//~ {
		//~ garbage_collect(bytes);
		//~ next_index = heap_data.i+bytes;
	//~ }
	//~ p = @heap_data.h[heap_data.i];
	//~ heap_data.cache = p;
	//~ heap_data.i = next_index;
	//~ return p;
//~ }
