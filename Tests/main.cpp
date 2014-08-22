#include <stdio.h>
#include "D3D9.h"

void InitTest();

int main(int argc, char **argv)
{
	printf("Starting Test Set\n");
	
	InitTest();
	return 0;
}

void InitTest()
{
	IDirect3D9* d3d9=NULL;
	
	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	
	if(d3d9!=NULL)
	{		
		d3d9->Release();
	}
}