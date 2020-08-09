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
	p = (@e.heap[currentIndex])+4;
	//e.cache = p;
	e.heapIndex+=8;
	return (void $)p;
}

// forces things to crush implied stack
void
finalizeHeapCursor(S_Environment $e, u8 $start, u8 $end)
{
	//u8  $cache;
	u32 $size;
	u64 amountWritten;
	
	//cache = e.cache;
	amountWritten = (end - start);
	amountWritten = (amountWritten+11)/8;
	// write size at start
	size = (u32$)(start-4);
	$size = amountWritten;
	// Index should be 8 above the size and 4 above the start
	e.heapIndex = (start+4)-e.heap;
	e.heapIndex+=(amountWritten-1)*8; // open cursor adds 8
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
