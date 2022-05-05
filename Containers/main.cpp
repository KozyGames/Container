#include "Multi_DynamicArray.h"
#include "TestObject.h"

#include <iostream>

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include <stdlib.h>
#include <crtdbg.h>


using std::endl; using std::cin; using std::cout;
using Kozy::Containers::Multi_DynamicArray;



int main()
{
	Kozy::Containers::testMulti_DynamicArray();

	_CrtDumpMemoryLeaks();
	return 0;
}
