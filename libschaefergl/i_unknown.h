#include "types.h"

#ifndef IUNKNOWN_H
#define IUNKNOWN_H

class IUnknown
{
public:
	IUnknown();
	~IUnknown();

	/*
	 * Increments the reference count for an interface on an object. 
	 * This method should be called for every new copy of a pointer to an interface on an object.
	 */
	ULONG AddRef();

	/*
	 * Retrieves pointers to the supported interfaces on an object.
	 */
	HRESULT QueryInterface(REFIID riid,void **ppvObject);

	ULONG Release();
};

#endif // IUNKNOWN_H
