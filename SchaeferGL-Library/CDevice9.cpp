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
 
#include "CDevice9.h"
#include "CCubeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CVertexDeclaration9.h"
#include "CVertexShader9.h"


CDevice9::CDevice9()
{
	
}

CDevice9::~CDevice9()
{
	
}

//IUknown

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	return this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid,void  **ppv)
{
	return S_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	return this->Release(0);
}

//Device

HRESULT STDMETHODCALLTYPE CDevice9::BeginScene()
{

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle)
{
	return this->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, (CSurface9**)ppSurface, pSharedHandle, NULL);
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,CSurface9 **ppSurface,HANDLE *pSharedHandle,char *debugLabel)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle)
{


	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EvictManagedResources()
{

	return S_OK;	
}

UINT STDMETHODCALLTYPE CDevice9::GetAvailableTextureMem()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index,float *pPlane)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp)
{
	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index,D3DLIGHT9 *pLight)
{
	return E_NOTIMPL;
} 

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index,BOOL *pEnable)
{

	return S_OK;		
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	return E_NOTIMPL;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	return 0; //TODO: implement GetNPatchMode
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	return 0; //TODO: implement GetNumberOfSwapChains
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget)
{
	
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT *pRect)
{
	return E_NOTIMPL;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	return true; //TODO implement GetSoftwareVertexProcessing()
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9 **ppShader)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{

	return S_OK;		
}
	
HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex,BOOL bEnable)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion)
{


	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(
	UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{	
	return S_OK;		
}

HRESULT CDevice9::SetClipPlane(DWORD Index,const float *pPlane)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X,INT Y,DWORD Flags)
{
	return; //TODO: implement SetCursorPosition
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
		
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetFVF(DWORD FVF)
{

	return S_OK;	
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp)
{

}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index,const D3DLIGHT9 *pLight)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9 *pShader)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{


	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT *pRect)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride)
{

		
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9 *pShader)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount)
{
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9 *pViewport)
{

	return S_OK;	
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	return TRUE;	
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
						
	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::ValidateDevice(DWORD *pNumPasses)
{

	return S_OK;	
}

ULONG STDMETHODCALLTYPE CDevice9::AddRef( int which, char *comment)
{
	return 0;
}

ULONG STDMETHODCALLTYPE	CDevice9::Release( int which, char *comment)
{
	return 0;
}