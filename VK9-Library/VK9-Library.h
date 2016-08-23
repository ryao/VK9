/*
Copyright(c) 2016 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SCHAEFERGLLIBRARY_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SCHAEFERGLLIBRARY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SCHAEFERGLLIBRARY_EXPORTS
#define SCHAEFERGLLIBRARY_API __declspec(dllexport)
#else
#define SCHAEFERGLLIBRARY_API __declspec(dllimport)
#endif

// This class is exported from the SchaeferGL-Library.dll
class SCHAEFERGLLIBRARY_API CSchaeferGLLibrary {
public:
	CSchaeferGLLibrary(void);
	// TODO: add your methods here.
};

extern SCHAEFERGLLIBRARY_API int nSchaeferGLLibrary;

SCHAEFERGLLIBRARY_API int fnSchaeferGLLibrary(void);
