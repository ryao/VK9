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
#include "i_direct3d_volume_texture9.h"

class IDirect3D9;

class IDirect3DDevice9
 : public IUnknown
{
public:
	IDirect3DDevice9();
	~IDirect3DDevice9();

	/*
	 * Begins a scene.
	 */
	virtual HRESULT BeginScene();
	
	/*
	 * Signals Direct3D to begin recording a device-state block.
	 */
	virtual HRESULT BeginStateBlock();
	
	/*
	 * Clears on or more surfaces such as a render target, multiple render targets, a stencil buffer, or a depth buffer.
	 */
	virtual HRESULT Clear(DWORD Count,const D3DRECT *pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	
	/*
	 * Allows an application to fill a rectangular area of a D3DPOOL_DEFAULT surface with a specified color.
	 */
	virtual HRESULT ColorFill(IDirect3DSurface9 *pSurface,const RECT *pRect,D3DCOLOR color);
	
	/*
	 * Creates an additional swap chain for rendering multiple views.
	 */
	virtual HRESULT CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DSwapChain9 **ppSwapChain);
	
	/*
	 * Creates a cube texture resource.
	 */
	virtual HRESULT CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9 **ppCubeTexture,HANDLE *pSharedHandle);
	
	/*
	 * Creates a depth-stencil resource.
	 */
	virtual HRESULT CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	
	/*
	 * Creates an index buffer.
	 */
	virtual HRESULT CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9 **ppIndexBuffer,HANDLE *pSharedHandle);
	
	/*
	 * Creates an off-screen surface.
	 */
	virtual HRESULT CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	
	/*
	 * Creates a pixel shader.
	 */
	virtual HRESULT CreatePixelShader(const DWORD *pFunction,IDirect3DPixelShader9 **ppShader);
	
	/*
	 * Creates a status query.
	 */
	virtual HRESULT CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9 **ppQuery);
	
	/*
	 * Creates a render target surface.
	 */
	virtual HRESULT CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9 **ppSurface,HANDLE *pSharedHandle);
	
	/*
	 * Creates a new state block that contains the values for all device states, vertex-related states, or pixel-related states.
	 */
	virtual HRESULT CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9 **ppSB);
	
	/*
	 * Creates a texture resource.
	 */
	virtual HRESULT CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9 **ppTexture,HANDLE *pSharedHandle);
	
	/*
	 * Creates a vertex buffer.
	 */
	virtual HRESULT CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9 **ppVertexBuffer,HANDLE *pSharedHandle);
  
	/*
	 * Create a vertex shader declaration from the device and the vertex elements.
	 */
	virtual HRESULT CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements,IDirect3DVertexDeclaration9 **ppDecl);
	
	/*
	 * Creates a vertex shader.
	 */
	virtual HRESULT CreateVertexShader(const DWORD *pFunction,IDirect3DVertexShader9 **ppShader);
	
	/*
	 * Creates a volume texture resource.
	 */
	virtual HRESULT CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9 **ppVolumeTexture,HANDLE *pSharedHandle);
  
	/*
	 * Frees a cached high-order patch.
	 */
	virtual HRESULT DeletePatch(UINT Handle);
	
	/*
	 * Based on indexing, renders the specified geometric primitive into an array of vertices.
	 */
	virtual HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount);
	
	/*
	 * Renders the specified geometric primitive with data specified by a user memory pointer.
	 */
	virtual HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,const void *pIndexData,D3DFORMAT IndexDataFormat,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride);
  
	/*
	 * Renders a sequence of nonindexed, geometric primitives of the specified type from the current set of data input streams.
	 */
	virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	
	/*
	 * Renders data specified by a user memory pointer as a sequence of geometric primitives of the specified type.
	 */
	virtual HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,const void *pVertexStreamZeroData,UINT VertexStreamZeroStride);
	
	/*
	 * Draws a rectangular patch using the currently set streams.
	 */
	virtual HRESULT DrawRectPatch(UINT Handle,const float *pNumSegs,const D3DRECTPATCH_INFO *pRectPatchInfo);
	
	/*
	 * Draws a triangular patch using the currently set streams.
	 */
	virtual HRESULT DrawTriPatch(UINT Handle,const float *pNumSegs,const D3DTRIPATCH_INFO *pTriPatchInfo);
	
	/*
	 * Ends a scene that was begun by calling BeginScene.
	 */
	virtual HRESULT EndScene();
	
	/*
	 * Signals Direct3D to stop recording a device-state block and retrieve a pointer to the state block interface.
	 */
	virtual HRESULT EndStateBlock(IDirect3DStateBlock9 **ppSB);
	
	/*
	 * Evicts all managed resources.
	 */
	virtual HRESULT EvictManagedResources();
	
	/*
	 * Returns an estimate of the amount of available texture memory.
	 */
	virtual UINT GetAvailableTextureMem();
	
	/*
	 * Retrieves a back buffer from the device's swap chain.
	 */
	virtual HRESULT GetBackBuffer(UINT  iSwapChain,UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9 **ppBackBuffer);
	
	/*
	 * Retrieves the coefficients of a user-defined clipping plane for the device.
	 */
	virtual HRESULT GetClipPlane(DWORD Index,float *pPlane);
	
	/*
	 * Retrieves the clip status
	 */
	virtual HRESULT GetClipStatus(D3DCLIPSTATUS9 *pClipStatus);
	
	/*
	 * Retrieves the creation parameters of the device.
	 */
	virtual HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
  
	/*
	 * Retrieves the current texture palette.
	 */
	virtual HRESULT GetCurrentTexturePalette(UINT *pPaletteNumber);
  
	/*
	 * Gets the depth-stencil surface owned by the Direct3DDevice object.
	 */
	virtual HRESULT GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface);
	
	/*
	 * Retrieves the capabilities of the rendering device.
	 */
	virtual HRESULT GetDeviceCaps(D3DCAPS9 *pCaps);
	
	/*
	 * Returns an interface to the instance of the Direct3D object that created the device.
	 */
	virtual HRESULT GetDirect3D(IDirect3D9 **ppD3D9);
  
	/*
	 * Retrieves the display mode's spatial resolution, color resolution, and fresh frequency.
	 */
	virtual HRESULT GetDisplayMode(UINT  iSwapChain,D3DDISPLAYMODE *pMode);
	
	/*
	 * Generates a copy of the device's front buffer and places that copy in a system memory buffer provided by the application.
	 */
	virtual HRESULT GetFrontBufferData(UINT  iSwapChain,IDirect3DSurface9 *pDestSurface);
	
	/*
	 * Gets the fixed vertex function declaration.
	 */
	virtual HRESULT GetFVF(DWORD *pFVF);
	
	/*
	 * Retrieves the gamma correction ramp for the swap chain.
	 */
	virtual void GetGammaRamp(UINT  iSwapChain,D3DGAMMARAMP *pRamp);
	
	/*
	 * Retrieves index data.
	 */
	virtual HRESULT GetIndices(IDirect3DIndexBuffer9 **ppIndexData,UINT *pBaseVertexIndex);
	
	/*
	 * Retrieves a set of lighting properties that this device uses.
	 */
	virtual HRESULT GetLight(DWORD Index,D3DLIGHT9 *pLight); /*?documentation lists D3DLight9*/
	
	/*
	 * Retrieves the activity status - enabled or disabled - for a set of lighting parameters withing a device.
	 */
	virtual HRESULT GetLightEnable(DWORD Index,BOOL *pEnable);
	
	/*
	 * Retrieves the current material properties for the device.
	 */
	virtual HRESULT GetMaterial(D3DMATERIAL9 *pMaterial);
	
	/*
	 * Gets the N-patch mode segments.
	 */
	virtual FLOAT GetNPatchMode();
	
	/*
	 * Gets the number of implicit swap chains.
	 */
	virtual UINT GetNumberOfSwapChains();
	
	/*
	 * Retrieves palette entries.
	 */
	virtual HRESULT GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY *pEntries);
	
	/*
	 * Retrieves the currently set pixel shader.
	 */
	virtual HRESULT GetPixelShader(IDirect3DPixelShader9 **ppShader);
	
	/*
	 * Gets a Boolean shader constant.
	 */
	virtual HRESULT GetPixelShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount);
	
	/*
	 * Gets a floating-point shader constant.
	 */
	virtual HRESULT GetPixelShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount);
	
	/*
	 * Gets an integer shader constant.
	 */
	virtual HRESULT GetPixelShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount);
	
	/*
	 * Returns information describing the raster of the monitor on which the swap chain is presented.
	 */
	virtual HRESULT GetRasterStatus(UINT  iSwapChain,D3DRASTER_STATUS *pRasterStatus);
	
	/*
	 * Retrieves a render-state value for a device.
	 */
	virtual HRESULT GetRenderState(D3DRENDERSTATETYPE State,DWORD *pValue);
	
	/*
	 * Retrieves a render-target surface.
	 */
	virtual HRESULT GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 **ppRenderTarget);
	
	/*
	 * Copies the render-target data from device memory to system memory.
	 */
	virtual HRESULT GetRenderTargetData(IDirect3DSurface9 *pRenderTarget,IDirect3DSurface9 *pDestSurface);
	
	/*
	 * Gets the sampler state value.
	 */
	virtual HRESULT GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD *pValue);
	
	/*
	 * Gets the scissor rectangle.
	 */
	virtual HRESULT GetScissorRect(RECT *pRect);
	
	/*
	 * Gets the vertex processing mode.
	 * (hardware or software)
	 */
	virtual BOOL GetSoftwareVertexProcessing();
	
	/*
	 * Retrieves a vertex buffer bound to the specified data stream.
	 */
	virtual HRESULT GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 **ppStreamData,UINT *pOffsetInBytes,UINT *pStride);
	
	/*
	 * Gets the stream source frequency divider value.
	 */
	virtual HRESULT GetStreamSourceFreq(UINT StreamNumber,UINT *pDivider);

	/*
	 * Gets a pointer to a swap chain.
	 */
	virtual HRESULT GetSwapChain(UINT  iSwapChain,IDirect3DSwapChain9 **ppSwapChain);
  
	/*
	 * Retrieves a texture assigned to a stage for a device.
	 */
	virtual HRESULT GetTexture(DWORD Stage,IDirect3DBaseTexture9 **ppTexture);
	
	/*
	 * Retrieves a state value for an assigned texture.
	 */
	virtual HRESULT GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD *pValue);
	
	/*
	 * Retrieves a matrix describing a transformation state.
	 */
	virtual HRESULT GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX *pMatrix);
	
	/*
	 * Gets a vertex shader declaration.
	 */
	virtual HRESULT GetVertexDeclaration(IDirect3DVertexDeclaration9 **ppDecl);
	
	/*
	 * Retrieves the currently set vertex shader.
	 */
	virtual HRESULT GetVertexShader(IDirect3DVertexShader9 **ppShader);
	
	/*
	 * Gets a Boolean vertex shader constant.
	 */
	virtual HRESULT GetVertexShaderConstantB(UINT StartRegister,BOOL *pConstantData,UINT BoolCount);
	
	/*
	 * Gets a floating-point vertex shader constant.
	 */
	virtual HRESULT GetVertexShaderConstantF(UINT StartRegister,float *pConstantData,UINT Vector4fCount);
	
	/*
	 * Gets an integer vertex shader constant.
	 */
	virtual HRESULT GetVertexShaderConstantI(UINT StartRegister,int *pConstantData,UINT Vector4iCount);
	
	/*
	 * Retrieves the viewport parameters currently set for the device.
	 */
	virtual HRESULT GetViewport(D3DVIEWPORT9 *pViewport);

	/*
	 * Enables or disables a set of lighting parameters within a device.
	 */
	virtual HRESULT LightEnable(DWORD LightIndex,BOOL bEnable);
	
	/*
	 * Multiplies a device's world, view, or projection matrices by a specified matrix.
	 */
	virtual HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix);
	
	/*
	 * Presents the contents of the next buffer in the sequence of back buffers owned by the device.
	 */
	virtual HRESULT Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion);
	
	/*
	 * Applies the vertex processing defined by the vertex shader to the set of input dat streams, generating a single stream of interleaved vertex data to the destination vertex buffer.
	 */
	virtual HRESULT ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9 *pDestBuffer,IDirect3DVertexDeclaration9 *pVertexDecl,DWORD Flags);
	
	/*
	 * Resets the type, size, and format of the swap chain.
	 */
	virtual HRESULT Reset(D3DPRESENT_PARAMETERS *pPresentationParameters);
	
	/*
	 * Sets the coefficients of a user-defined clipping plane for the device.
	 */
	virtual HRESULT SetClipPlane(DWORD Index,const float *pPlane);
	
	/*
	 * Sets the clip status
	 */
	virtual HRESULT SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus);
	
	/*
	 * Sets the current texture palette.
	 */
	virtual HRESULT SetCurrentTexturePalette(UINT PaletteNumber);
	
	/*
	 * Sets the cursor position and update options.
	 */
	virtual void SetCursorPosition(INT X,INT Y,DWORD Flags);
	
	/*
	 * Sets properties for the cursor.
	 */
	virtual HRESULT SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9 *pCursorBitmap);
	
	/*
	 * Sets teh depth stencil surface.
	 */
	virtual HRESULT SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil);
	
	/*
	 * This method allows the use of GDI dialog boxes in full-screen mode applications.
	 */
	virtual HRESULT SetDialogBoxMode(BOOL bEnableDialogs);
	
	/*
	 * Sets the current vertex stream declaration.
	 */
	virtual HRESULT SetFVF(DWORD FVF);
	
	/*
	 * Sets teh gamma correction ramp for the implicit swap chain.
	 */
	virtual void SetGammaRamp(UINT  iSwapChain,DWORD Flags,const D3DGAMMARAMP *pRamp);
	
	/*
	 * Sets index data.
	 */
	virtual HRESULT SetIndices(IDirect3DIndexBuffer9 *pIndexData);
	
	/*
	 * Assigns a set of lighting properties for this device.
	 */
	virtual HRESULT SetLight(DWORD Index,const D3DLIGHT9 *pLight);
	
	/*
	 * Sets the material properties for the device.
	 */
	virtual HRESULT SetMaterial(const D3DMATERIAL9 *pMaterial);
	
	/*
	 * Enable or disable N-patches.
	 */
	virtual HRESULT SetNPatchMode(float nSegments);
	
	/*
	 * Sets palette entries.
	 */
	virtual HRESULT SetPaletteEntries(UINT PaletteNumber,const PALETTEENTRY *pEntries);
	
	/*
	 * Sets the current pixel shader to a previously created pixel shader.
	 */
	virtual HRESULT SetPixelShader(IDirect3DPixelShader9 *pShader);
	
	/*
	 * Sets a Boolean shader constant.
	 */
	virtual HRESULT SetPixelShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount);
	
	/*
	 * Sets a floating-point shader constant.
	 */
	virtual HRESULT SetPixelShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount);
	
	/*
	 * Sets an integer shader constant.
	 */
	virtual HRESULT SetPixelShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount);
	
	/*
	 * Sets a single device render-state parameter.
	 */
	virtual HRESULT SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
	
	/*
	 * Sets a new color buffer for the device.
	 */
	virtual HRESULT SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 *pRenderTarget);
	
	/*
	 * Sets the sampler state value
	 */
	virtual HRESULT SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	
	/*
	 * Sets the scissor rectangle.
	 */
	virtual HRESULT SetScissorRect(const RECT *pRect);
	
	/*
	 * Use this method to switch between software and hardware vertex processing.
	 */
	virtual HRESULT SetSoftwareVertexProcessing(BOOL bSoftware);
	
	/*
	 * Binds a vertex buffer to a device data stream.
	 */
	virtual HRESULT SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9 *pStreamData,UINT OffsetInBytes,UINT Stride);
	
	/*
	 * Sets the stream source frequency divider value.
	 */
	virtual HRESULT SetStreamSourceFreq(UINT StreamNumber,UINT FrequencyParameter);
	
	/*
	 * Assigns a texture to a stage for device.
	 */
	virtual HRESULT SetTexture(DWORD Sampler,IDirect3DBaseTexture9 *pTexture);
	
	/*
	 * Sets teh state value for the currently assigned texture.
	 */
	virtual HRESULT SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	
	/*
	 * Sets a single device transformation-related state.
	 */
	virtual HRESULT SetTransform(D3DTRANSFORMSTATETYPE State,const D3DMATRIX *pMatrix);
	
	/*
	 * Sets a vertex declaration.
	 */
	virtual HRESULT SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
	
	/*
	 * Sets the vertex shader.
	 */
	virtual HRESULT SetVertexShader(IDirect3DVertexShader9 *pShader);
	
	/*
	 * Sets a boolean vertex shader constant.
	 */
	virtual HRESULT SetVertexShaderConstantB(UINT StartRegister,const BOOL *pConstantData,UINT BoolCount);
	
	/*
	 * Sets a floating-point vertex shader constant.
	 */
	virtual HRESULT SetVertexShaderConstantF(UINT StartRegister,const float *pConstantData,UINT Vector4fCount);
  
	/*
	 * Sets an integer vertex shader constant.
	 */
	virtual HRESULT SetVertexShaderConstantI(UINT StartRegister,const int *pConstantData,UINT Vector4iCount);
  
	/*
	 * Sets the viewport parameters for the device.
	 */
	virtual HRESULT SetViewport(const D3DVIEWPORT9 *pViewport);
	
	/*
	 * Displays or hides the cursor
	 */
	virtual BOOL ShowCursor(BOOL bShow);
	
	/*
	 * Copy the contents of the source rectangle to the destination rectangle.
	 */
	virtual HRESULT StretchRect(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestSurface,const RECT *pDestRect,D3DTEXTUREFILTERTYPE Filter);
	
	/*
	 * Reports the current cooperative-level status of the Direct3D device for a windowed or full-scren application.
	 */
	virtual HRESULT TestCooperativeLevel();
	
	/*
	 * Copies rectangular subsets of pixels from one surface to another.
	 */
	virtual HRESULT UpdateSurface(IDirect3DSurface9 *pSourceSurface,const RECT *pSourceRect,IDirect3DSurface9 *pDestinationSurface,const POINT *pDestinationPoint);
	
	/*
	 * Reports the device's ability to render the current texture-blending operations and arguments in a single pass.
	 */
	virtual HRESULT ValidateDevice(DWORD *pNumPasses);
};

typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9, *PDIRECT3DDEVICE9;

#endif // IDIRECT3DDEVICE9_H
