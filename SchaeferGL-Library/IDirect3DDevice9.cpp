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
 
#include "IDirect3DDevice9.h"
#include <iostream> 

IDirect3DDevice9::IDirect3DDevice9()
{
	
}

IDirect3DDevice9::~IDirect3DDevice9()
{
	
}

HRESULT IDirect3DDevice9::BeginScene()
{
	std::cout << "IDirect3DDevice9::BeginScene()" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::BeginStateBlock()
{
	std::cout << "IDirect3DDevice9::BeginStateBlock()" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	std::cout << "IDirect3DDevice9::Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color)
{
	std::cout << "IDirect3DDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain)
{
	std::cout << "IDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)
{
	std::cout << "IDirect3DDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)
{
	std::cout << "IDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB)
{
	std::cout << "IDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)
{
	std::cout << "IDirect3DDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)
{
	std::cout << "IDirect3DDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)
{
	std::cout << "IDirect3DDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DeletePatch(UINT Handle)
{
	std::cout << "IDirect3DDevice9::DeletePatch(UINT Handle)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	std::cout << "IDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	std::cout << "IDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	std::cout << "IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	std::cout << "IDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	std::cout << "IDirect3DDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	std::cout << "IDirect3DDevice9::" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::EndScene()
{
	std::cout << "IDirect3DDevice9::EndScene()" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	std::cout << "IDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::EvictManagedResources()
{
	std::cout << "IDirect3DDevice9::EvictManagedResources()" << std::endl;
	
	return E_NOTIMPL;
}

UINT IDirect3DDevice9::GetAvailableTextureMem()
{
	std::cout << "IDirect3DDevice9::GetAvailableTextureMem()" << std::endl;
	
	return 0;
}

HRESULT IDirect3DDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)
{
	std::cout << "IDirect3DDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetClipPlane(DWORD Index,float *pPlane)
{
	std::cout << "IDirect3DDevice9::GetClipPlane(DWORD Index,float *pPlane)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	std::cout << "IDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	std::cout << "IDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	std::cout << "IDirect3DDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	std::cout << "IDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	std::cout << "IDirect3DDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	std::cout << "IDirect3DDevice9::GetDirect3D(IDirect3D9 **ppD3D9)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode)
{
	std::cout << "IDirect3DDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)
{
	std::cout << "IDirect3DDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetFVF(DWORD *pFVF)
{
	std::cout << "IDirect3DDevice9::GetFVF(DWORD *pFVF)" << std::endl;
	
	return E_NOTIMPL;
}

void IDirect3DDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp)
{
	std::cout << "IDirect3DDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp)" << std::endl;
	
	return;
}

HRESULT IDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData,UINT *pBaseVertexIndex)
{
	std::cout << "IDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData,UINT *pBaseVertexIndex)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight) /*?documentation lists D3DLight9*/
{
	std::cout << "IDirect3DDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)
{
	std::cout << "IDirect3DDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	std::cout << "IDirect3DDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)" << std::endl;
	
	return E_NOTIMPL;
}

FLOAT IDirect3DDevice9::GetNPatchMode()
{
	std::cout << "IDirect3DDevice9::GetNPatchMode()" << std::endl;
	
	return 0;
}

UINT IDirect3DDevice9::GetNumberOfSwapChains()
{
	std::cout << "IDirect3DDevice9::GetNumberOfSwapChains()" << std::endl;
	
	return 0;
}

HRESULT IDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries)
{
	std::cout << "IDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	std::cout << "IDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	std::cout << "IDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	std::cout << "IDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	std::cout << "IDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus)
{
	std::cout << "IDirect3DDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue)
{
	std::cout << "IDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)
{
	std::cout << "IDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)
{
	std::cout << "IDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue)
{
	std::cout << "IDirect3DDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetScissorRect(RECT *pRect)
{
	std::cout << "IDirect3DDevice9::GetScissorRect(RECT *pRect)" << std::endl;
	
	return E_NOTIMPL;
}

BOOL IDirect3DDevice9::GetSoftwareVertexProcessing()
{
	std::cout << "IDirect3DDevice9::GetSoftwareVertexProcessing()" << std::endl;
	
	return false;
}

HRESULT IDirect3DDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride)
{
	std::cout << "IDirect3DDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider)
{
	std::cout << "IDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain)
{
	std::cout << "IDirect3DDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)
{
	std::cout << "IDirect3DDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue)
{
	std::cout << "IDirect3DDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix)
{
	std::cout << "IDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	std::cout << "IDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	std::cout << "IDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	std::cout << "IDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	std::cout << "IDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	std::cout << "IDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	std::cout << "IDirect3DDevice9::GetViewport(D3DVIEWPORT9 *pViewport)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::LightEnable(DWORD LightIndex,BOOL bEnable)
{
	std::cout << "IDirect3DDevice9::LightEnable(DWORD LightIndex,BOOL bEnable)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	std::cout << "IDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion)
{
	std::cout << "IDirect3DDevice9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags)
{
	std::cout << "IDirect3DDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	std::cout << "IDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetClipPlane(DWORD Index,const float *pPlane)
{
	std::cout << "IDirect3DDevice9::SetClipPlane(DWORD Index,const float *pPlane)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	std::cout << "IDirect3DDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	std::cout << "IDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)" << std::endl;
	
	return E_NOTIMPL;
}

void IDirect3DDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags)
{
	std::cout << "IDirect3DDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags)" << std::endl;
	
	return;
}

HRESULT IDirect3DDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap)
{
	std::cout << "IDirect3DDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	std::cout << "IDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	std::cout << "IDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetFVF(DWORD FVF)
{
	std::cout << "IDirect3DDevice9::SetFVF(DWORD FVF)" << std::endl;
	
	return E_NOTIMPL;
}

void IDirect3DDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)
{
	std::cout << "IDirect3DDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)" << std::endl;
	
	return;
}

HRESULT IDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{
	std::cout << "IDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)
{
	std::cout << "IDirect3DDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	std::cout << "IDirect3DDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetNPatchMode(float nSegments)
{
	std::cout << "IDirect3DDevice9::SetNPatchMode(float nSegments)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries)
{
	std::cout << "IDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{
	std::cout << "IDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	std::cout << "IDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	std::cout << "IDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	std::cout << "IDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	std::cout << "IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)
{
	std::cout << "IDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	std::cout << "IDirect3DDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetScissorRect(const RECT *pRect)
{
	std::cout << "IDirect3DDevice9::SetScissorRect(const RECT *pRect)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	std::cout << "IDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)
{
	std::cout << "IDirect3DDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter)
{
	std::cout << "IDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)
{
	std::cout << "IDirect3DDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	std::cout << "IDirect3DDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	std::cout << "IDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{
	std::cout << "IDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{
	std::cout << "IDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	std::cout << "IDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	std::cout << "IDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	std::cout << "IDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{
	std::cout << "IDirect3DDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)" << std::endl;
	
	return E_NOTIMPL;
}

BOOL IDirect3DDevice9::ShowCursor(BOOL bShow)
{
	std::cout << "IDirect3DDevice9::ShowCursor(BOOL bShow)" << std::endl;
	
	return false;
}

HRESULT IDirect3DDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	std::cout << "IDirect3DDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::TestCooperativeLevel()
{
	std::cout << "IDirect3DDevice9::TestCooperativeLevel()" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint)
{
	std::cout << "IDirect3DDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint)" << std::endl;
	
	return E_NOTIMPL;
}

HRESULT IDirect3DDevice9::ValidateDevice(DWORD *pNumPasses)
{
	std::cout << "IDirect3DDevice9::ValidateDevice(DWORD *pNumPasses)" << std::endl;
	
	return E_NOTIMPL;
}
