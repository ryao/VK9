/*
 * Copyright (C) 2014 Christopher Joseph Dean Schaefer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef IDIRECT3DDEVICE9_H
#define IDIRECT3DDEVICE9_H

#include "i_unknown.h"
#include "i_direct3d_surface9.h"
#include "i_direct3d_swap_chain9.h"
#include "i_direct3d_cube_texture9.h"
#include "i_direct3d_index_buffer9.h"
#include "i_direct3d_pixel_shader9.h"
#include "i_direct3d_query9.h"
#include "i_direct3d_state_block9.h"
#include "i_direct3d_texture9.h"
#include "i_direct3d_vertex_buffer9.h"
#include "i_direct3d_vertex_declaration9.h"
#include "i_direct3d_vertex_shader9.h"

class IDirect3DDevice9
 : public IUnknown
{
public:
	IDirect3DDevice9();
	~IDirect3DDevice9();

	/*
	 * Begins a scene.
	 */
	HRESULT BeginScene();
	
	/*
	 * Signals Direct3D to begin recording a device-state block.
	 */
	HRESULT BeginStateBlock();
	
	/*
	 * Clears on or more surfaces such as a render target, multiple render targets, a stencil buffer, or a depth buffer.
	 */
	HRESULT Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	
	/*
	 * Allows an application to fill a rectangular area of a D3DPOOL_DEFAULT surface with a specified color.
	 */
	HRESULT ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color);
	
	/*
	 * Creates an additional swap chain for rendering multiple views.
	 */
	HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain);
	
	/*
	 * Creates a cube texture resource.
	 */
	HRESULT CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle);
	
	/*
	 * Creates a depth-stencil resource.
	 */
	HRESULT CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	
	/*
	 * Creates an index buffer.
	 */
	HRESULT CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle);
	
	/*
	 * Creates an off-screen surface.
	 */
	HRESULT CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	
	/*
	 * Creates a pixel shader.
	 */
	HRESULT CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader);
	
	/*
	 * Creates a status query.
	 */
	HRESULT CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery);
	
	/*
	 * Creates a render target surface.
	 */
	HRESULT CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	
	/*
	 * Creates a new state block that contains the values for all device states, vertex-related states, or pixel-related states.
	 */
	HRESULT CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB);
	
	/*
	 * Creates a texture resource.
	 */
	HRESULT CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle);
	
	/*
	 * Creates a vertex buffer.
	 */
	HRESULT CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle);
  
	/*
	 * Create a vertex shader declaration from the device and the vertex elements.
	 */
	HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl);
	
	/*
	 * Creates a vertex shader.
	 */
	HRESULT CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader);
	
	/*
	 * Creates a volume texture resource.
	 */
	HRESULT CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle);
  
};

#endif // IDIRECT3DDEVICE9_H
