// Автор: Азат Давлетшин
// Описание: Если при выполнении полетят исключения, то есть риск, что память, выделенная под узлы дерева и
// под состояния CLatexState, не будет освобождена. Поэтому в обработчике исключений удаляем кучу и создаем новую.

#include <Windows.h>
#include <assert.h>

extern HANDLE ConverterHeap = 0;

void* operator new( size_t size )
{
	if( ConverterHeap == 0 ) {
		return malloc( size );
	} 

	return ::HeapAlloc( ConverterHeap, 0, size );
}

void operator delete( void* ptr )
{
	if( ConverterHeap == 0 ) {
		return free( ptr );
	}

	::HeapFree( ConverterHeap, 0, ptr );
}