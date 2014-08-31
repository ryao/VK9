// SchaeferGL-Library.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SchaeferGL-Library.h"


// This is an example of an exported variable
SCHAEFERGLLIBRARY_API int nSchaeferGLLibrary=0;

// This is an example of an exported function.
SCHAEFERGLLIBRARY_API int fnSchaeferGLLibrary(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SchaeferGL-Library.h for the class definition
CSchaeferGLLibrary::CSchaeferGLLibrary()
{
	return;
}
