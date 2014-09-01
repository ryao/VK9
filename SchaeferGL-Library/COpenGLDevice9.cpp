//========= Copyright Valve Corporation, All rights reserved. ============//
//                       TOGL CODE LICENSE
//
//  Copyright 2011-2014 Valve Corporation
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
// dxabstract.h
//
//==================================================================================================

/*
 * Code from ToGL has been modified to fit the design.
 */
 
#include "COpenGLDevice9.h"

COpenGLDevice9::COpenGLDevice9()
{
}

COpenGLDevice9::~COpenGLDevice9()
{
}

HRESULT COpenGLDevice9::BeginScene()
{
}
HRESULT COpenGLDevice9::BeginStateBlock()
{
}
HRESULT COpenGLDevice9::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
}
HRESULT COpenGLDevice9::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color)
{
}
HRESULT COpenGLDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain)
{
}
HRESULT COpenGLDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
}
HRESULT COpenGLDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
}
HRESULT COpenGLDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
}
HRESULT COpenGLDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
}
HRESULT COpenGLDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
}
HRESULT COpenGLDevice9::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
}
HRESULT COpenGLDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
}
HRESULT COpenGLDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
}
HRESULT COpenGLDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
}
HRESULT COpenGLDevice9::DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo)
{
}
HRESULT COpenGLDevice9::DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo)
{
}
HRESULT COpenGLDevice9::EndScene()
{
}
HRESULT COpenGLDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
}
HRESULT COpenGLDevice9::EvictManagedResources()
{
}
UINT COpenGLDevice9::GetAvailableTextureMem()
{
}
HRESULT COpenGLDevice9::GetBackBuffer(UINT iSwapChain, UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
}
HRESULT COpenGLDevice9::GetClipPlane(DWORD Index, float* pPlane)
{
}
HRESULT COpenGLDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
}
HRESULT COpenGLDevice9::GetDeviceCaps(D3DCAPS9* pCaps)
{
}
HRESULT COpenGLDevice9::GetDirect3D(IDirect3D9** ppD3D9)
{
}
HRESULT COpenGLDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
}
HRESULT COpenGLDevice9::GetFVF(DWORD* pFVF)
{
}
HRESULT COpenGLDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
}
void COpenGLDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
}
HRESULT COpenGLDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
}
HRESULT COpenGLDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
}
HRESULT COpenGLDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
}
HRESULT COpenGLDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
}
HRESULT COpenGLDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
}
HRESULT COpenGLDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
}
HRESULT COpenGLDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
}
HRESULT COpenGLDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
}
BOOL COpenGLDevice9::GetSoftwareVertexProcessing()
{
}
HRESULT COpenGLDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
}
HRESULT COpenGLDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* pDivider)
{
}
HRESULT COpenGLDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** ppSwapChain)
{
}
HRESULT COpenGLDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
}
HRESULT COpenGLDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
}
HRESULT COpenGLDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
}
HRESULT COpenGLDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
}
HRESULT COpenGLDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
}
HRESULT COpenGLDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
}
HRESULT COpenGLDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{
}
HRESULT COpenGLDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
}
HRESULT COpenGLDevice9::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
}
HRESULT COpenGLDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
}
HRESULT COpenGLDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
}
HRESULT COpenGLDevice9::SetClipPlane(DWORD Index, const float* pPlane)
{
}
HRESULT COpenGLDevice9::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus)
{
}
HRESULT COpenGLDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
}
void COpenGLDevice9::SetCursorPosition(INT X, INT Y, DWORD Flags)
{
}
HRESULT COpenGLDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
}
HRESULT COpenGLDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
}
HRESULT COpenGLDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
}
HRESULT COpenGLDevice9::SetFVF(DWORD FVF)
{
}
void COpenGLDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp)
{
}
HRESULT COpenGLDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
}
HRESULT COpenGLDevice9::SetLight(DWORD Index, const D3DLIGHT9* pLight)
{
}
HRESULT COpenGLDevice9::SetMaterial(const D3DMATERIAL9* pMaterial)
{
}
HRESULT COpenGLDevice9::SetNPatchMode(float nSegments)
{
}
HRESULT COpenGLDevice9::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries)
{
}
HRESULT COpenGLDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
}
HRESULT COpenGLDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
}
HRESULT COpenGLDevice9::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
}
HRESULT COpenGLDevice9::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
}
HRESULT COpenGLDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
}
HRESULT COpenGLDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
}
HRESULT COpenGLDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
}
HRESULT COpenGLDevice9::SetScissorRect(const RECT* pRect)
{
}
HRESULT COpenGLDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
}
HRESULT COpenGLDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
}
HRESULT COpenGLDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter)
{
}
HRESULT COpenGLDevice9::SetTexture(DWORD Sampler, IDirect3DBaseTexture9* pTexture)
{
}
HRESULT COpenGLDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
}
HRESULT COpenGLDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
}
HRESULT COpenGLDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
}
HRESULT COpenGLDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
}
HRESULT COpenGLDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
}
HRESULT COpenGLDevice9::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
}
HRESULT COpenGLDevice9::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
}
HRESULT COpenGLDevice9::SetViewport(const D3DVIEWPORT9* pViewport)
{
}
BOOL COpenGLDevice9::ShowCursor(BOOL bShow)
{
}
HRESULT COpenGLDevice9::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
}
HRESULT COpenGLDevice9::TestCooperativeLevel()
{
}
HRESULT COpenGLDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestinationPoint)
{
}
HRESULT COpenGLDevice9::ValidateDevice(DWORD* pNumPasses)
{
}
