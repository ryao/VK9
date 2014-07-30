#include "i_unknown.h"
#include <iostream> 

IUnknown::IUnknown()
{
	std::cout << "IUnknown::IUnknown()" << std::endl;
}

IUnknown::~IUnknown()
{
	
}

ULONG IUnknown::AddRef()
{
	
}

HRESULT IUnknown::QueryInterface(REFIID riid,void **ppvObject)
{
	
}

ULONG IUnknown::Release()
{
	
}
