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
 
#ifndef COPENGLDEVICE9_H
#define COPENGLDEVICE9_H

#include "IDirect3DDevice9.h" // Base class: IDirect3DDevice9
#include "COpenGLUnknown.h"

class COpenGLDevice9 : public IDirect3DDevice9,COpenGLUnknown
{
public:
	COpenGLDevice9();
	~COpenGLDevice9();

public:
	virtual HRESULT Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	virtual HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain);
	virtual HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	virtual HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle);
	virtual HRESULT CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	virtual HRESULT CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader);
	virtual HRESULT CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery);
	virtual HRESULT CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	virtual HRESULT CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
	virtual HRESULT DeletePatch(UINT Handle);
	virtual HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);
	virtual HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	virtual HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	virtual HRESULT EndScene();
	virtual HRESULT EndStateBlock(IDirect3DStateBlock9** ppSB);
	virtual HRESULT GetClipPlane(DWORD Index, float* pPlane);
	virtual HRESULT GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);
	virtual HRESULT GetFVF(DWORD* pFVF);
	virtual HRESULT GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface);
	virtual HRESULT GetIndices(IDirect3DIndexBuffer9** ppIndexData, UINT* pBaseVertexIndex);
	virtual HRESULT GetLight(DWORD Index, D3DLIGHT9* pLight);
	virtual HRESULT GetLightEnable(DWORD Index, BOOL* pEnable);
	virtual HRESULT GetMaterial(D3DMATERIAL9* pMaterial);
	virtual HRESULT GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);
	virtual HRESULT GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
	virtual HRESULT GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
	virtual HRESULT GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
	virtual HRESULT GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl);
	virtual HRESULT GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount);
	virtual HRESULT GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount);
	virtual HRESULT GetViewport(D3DVIEWPORT9* pViewport);
	virtual HRESULT LightEnable(DWORD LightIndex, BOOL bEnable);
	virtual HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix);
	virtual HRESULT Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	virtual HRESULT ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags);
	virtual HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT SetClipPlane(DWORD Index, const float* pPlane);
	virtual HRESULT SetClipStatus(const D3DCLIPSTATUS9* pClipStatus);
	virtual HRESULT SetCurrentTexturePalette(UINT PaletteNumber);
	virtual void SetCursorPosition(INT X, INT Y, DWORD Flags);
	virtual HRESULT SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap);
	virtual HRESULT SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
	virtual HRESULT SetDialogBoxMode(BOOL bEnableDialogs);
	virtual HRESULT SetFVF(DWORD FVF);
	virtual void SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp);
	virtual HRESULT SetIndices(IDirect3DIndexBuffer9* pIndexData);
	virtual HRESULT SetLight(DWORD Index, const D3DLIGHT9* pLight);
	virtual HRESULT SetMaterial(const D3DMATERIAL9* pMaterial);
	virtual HRESULT SetNPatchMode(float nSegments);
	virtual HRESULT SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries);
	virtual HRESULT SetPixelShader(IDirect3DPixelShader9* pShader);
	virtual HRESULT SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
	virtual HRESULT SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount);
	virtual HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	virtual HRESULT SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
	virtual HRESULT SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT SetScissorRect(const RECT* pRect);
	virtual HRESULT SetSoftwareVertexProcessing(BOOL bSoftware);
	virtual HRESULT SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
	virtual HRESULT SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter);
	virtual HRESULT SetTexture(DWORD Sampler, IDirect3DBaseTexture9* pTexture);
	virtual HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	virtual HRESULT SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix);
	virtual HRESULT SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);
	virtual HRESULT SetVertexShader(IDirect3DVertexShader9* pShader);
	virtual HRESULT SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount);
	virtual HRESULT SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
	virtual HRESULT SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount);
	virtual HRESULT SetViewport(const D3DVIEWPORT9* pViewport);
	virtual BOOL ShowCursor(BOOL bShow);
	virtual HRESULT StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
	virtual HRESULT TestCooperativeLevel();
	virtual HRESULT UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestinationPoint);
	virtual HRESULT ValidateDevice(DWORD* pNumPasses);
};

#endif // COPENGLDEVICE9_H
