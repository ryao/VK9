/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "Perf_CommandStreamManager.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/format.hpp>

#include "CStateBlock9.h"

void ProcessQueue(CommandStreamManager* commandStreamManager)
{
	while (commandStreamManager->IsRunning)
	{
		WorkItem workItem;
		if (commandStreamManager->mWorkItems.pop(workItem))
		{
			commandStreamManager->IsBusy = 1;
			switch (workItem.WorkItemType)
			{
			case Instance_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateInstance();
			}
			break;
			case Instance_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyInstance(workItem.Id);
			}
			break;
			case Device_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateWindow1(workItem.Id, workItem.Argument1, workItem.Argument2);
			}
			break;
			case Device_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyWindow(workItem.Id);
			}
			break;
			case Device_Clear:
			{
				DWORD Count = boost::any_cast<DWORD>(workItem.Argument1);
				D3DRECT* pRects = boost::any_cast<D3DRECT*>(workItem.Argument2);
				DWORD Flags = boost::any_cast<DWORD>(workItem.Argument3);
				D3DCOLOR Color = boost::any_cast<D3DCOLOR>(workItem.Argument4);
				float Z = boost::any_cast<float>(workItem.Argument5);
				DWORD Stencil = boost::any_cast<DWORD>(workItem.Argument6);

				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				commandStreamManager->mRenderManager.Clear(realWindow, Count, pRects, Flags, Color, Z, Stencil);
			}
			break;
			case Device_BeginScene:
			{
				auto& renderManager = commandStreamManager->mRenderManager;
				auto& realWindow = (*renderManager.mStateManager.mWindows[workItem.Id]);
				if (!realWindow.mIsSceneStarted)
				{
					renderManager.StartScene(realWindow);
				}
			}
			break;
			case Device_Present:
			{
				RECT* pSourceRect = boost::any_cast<RECT*>(workItem.Argument1);
				RECT* pDestRect = boost::any_cast<RECT*>(workItem.Argument2);
				HWND hDestWindowOverride = boost::any_cast<HWND>(workItem.Argument3);
				RGNDATA* pDirtyRegion = boost::any_cast<RGNDATA*>(workItem.Argument4);

				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				commandStreamManager->mRenderManager.Present(realWindow, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
			}
			break;
			case Device_DrawIndexedPrimitive:
			{
				D3DPRIMITIVETYPE Type = boost::any_cast<D3DPRIMITIVETYPE>(workItem.Argument1);
				INT BaseVertexIndex = boost::any_cast<INT>(workItem.Argument2);
				UINT MinIndex = boost::any_cast<UINT>(workItem.Argument3);
				UINT NumVertices = boost::any_cast<UINT>(workItem.Argument4);
				UINT StartIndex = boost::any_cast<UINT>(workItem.Argument5);
				UINT PrimitiveCount = boost::any_cast<UINT>(workItem.Argument6);

				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				commandStreamManager->mRenderManager.DrawIndexedPrimitive(realWindow, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}
			break;
			case Device_DrawPrimitive:
			{
				D3DPRIMITIVETYPE PrimitiveType = boost::any_cast<D3DPRIMITIVETYPE>(workItem.Argument1);
				UINT StartVertex = boost::any_cast<UINT>(workItem.Argument2);
				UINT PrimitiveCount = boost::any_cast<UINT>(workItem.Argument3);

				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				commandStreamManager->mRenderManager.DrawPrimitive(realWindow, PrimitiveType, StartVertex, PrimitiveCount);
			}
			break;
			case Device_GetDisplayMode:
			{
				UINT iSwapChain = boost::any_cast<UINT>(workItem.Argument1);
				D3DDISPLAYMODE* pMode = boost::any_cast<D3DDISPLAYMODE*>(workItem.Argument2);
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);

				if (iSwapChain)
				{
					//TODO: Implement.
					BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetDisplayMode multiple swapchains are not implemented!";
				}
				else
				{
					pMode->Height = realWindow.mSwapchainExtent.height;
					pMode->Width = realWindow.mSwapchainExtent.width;
					pMode->RefreshRate = 60; //fake it till you make it.
					pMode->Format = ConvertFormat(realWindow.mFormat);
				}
			}
			break;
			case Device_GetFVF:
			{
				DWORD* pFVF = boost::any_cast<DWORD*>(workItem.Argument1);
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);

				(*pFVF) = realWindow.mDeviceState.mFVF;
			}
			break;
			case Device_GetLight:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				DWORD Index = boost::any_cast<DWORD>(workItem.Argument1);
				D3DLIGHT9* pLight = boost::any_cast<D3DLIGHT9*>(workItem.Argument2);
				auto& light = realWindow.mDeviceState.mLights[Index];

				pLight->Type = (*(D3DLIGHTTYPE*)light.Type);
				pLight->Diffuse = (*(D3DCOLORVALUE*)light.Diffuse);
				pLight->Specular = (*(D3DCOLORVALUE*)light.Specular);
				pLight->Ambient = (*(D3DCOLORVALUE*)light.Ambient);

				pLight->Position = (*(D3DVECTOR*)light.Position);
				pLight->Direction = (*(D3DVECTOR*)light.Direction);

				pLight->Range = light.Range;
				pLight->Falloff = light.Falloff;
				pLight->Attenuation0 = light.Attenuation0;
				pLight->Attenuation1 = light.Attenuation1;
				pLight->Attenuation2 = light.Attenuation2;
				pLight->Theta = light.Theta;
				pLight->Phi = light.Phi;
			}
			break;
			case Device_GetLightEnable:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				DWORD Index = boost::any_cast<DWORD>(workItem.Argument1);
				BOOL* pEnable = boost::any_cast<BOOL*>(workItem.Argument2);

				(*pEnable) = realWindow.mDeviceState.mLights[Index].IsEnabled;
			}
			break;
			case Device_GetMaterial:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				D3DMATERIAL9* pMaterial = boost::any_cast<D3DMATERIAL9*>(workItem.Argument1);

				(*pMaterial) = realWindow.mDeviceState.mMaterial;
			}
			break;
			case Device_GetNPatchMode:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				FLOAT* output = boost::any_cast<FLOAT*>(workItem.Argument1);

				(*output) = realWindow.mDeviceState.mNSegments;
			}
			break;
			case Device_GetPixelShader:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				IDirect3DPixelShader9** ppShader = boost::any_cast<IDirect3DPixelShader9**>(workItem.Argument1);

				(*ppShader) = (IDirect3DPixelShader9*)realWindow.mDeviceState.mPixelShader;
			}
			break;
			case Device_GetPixelShaderConstantB:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				UINT StartRegister = boost::any_cast<UINT>(workItem.Argument1);
				BOOL* pConstantData = boost::any_cast<BOOL*>(workItem.Argument2);
				UINT BoolCount = boost::any_cast<UINT>(workItem.Argument3);

				auto& slots = realWindow.mDeviceState.mPixelShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					pConstantData[i] = slots.BooleanConstants[StartRegister + i];
				}
			}
			break;
			case Device_GetPixelShaderConstantF:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				UINT StartRegister = boost::any_cast<UINT>(workItem.Argument1);
				float* pConstantData = boost::any_cast<float*>(workItem.Argument2);
				UINT Vector4fCount = boost::any_cast<UINT>(workItem.Argument3);

				auto& slots = realWindow.mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.FloatConstants[startIndex + i];
				}
			}
			break;
			case Device_GetPixelShaderConstantI:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				UINT StartRegister = boost::any_cast<UINT>(workItem.Argument1);
				int* pConstantData = boost::any_cast<int*>(workItem.Argument2);
				UINT Vector4iCount = boost::any_cast<UINT>(workItem.Argument3);

				auto& slots = realWindow.mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.IntegerConstants[startIndex + i];
				}
			}
			break;
			case Device_GetRenderState:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				D3DRENDERSTATETYPE State = boost::any_cast<D3DRENDERSTATETYPE>(workItem.Argument1);
				DWORD* pValue = boost::any_cast<DWORD*>(workItem.Argument2);

				SpecializationConstants* constants = nullptr;

				if (realWindow.mCurrentStateRecording != nullptr)
				{
					constants = &realWindow.mCurrentStateRecording->mDeviceState.mSpecializationConstants;
				}
				else
				{
					constants = &realWindow.mDeviceState.mSpecializationConstants;
				}

				switch (State)
				{
				case D3DRS_ZENABLE:
					(*pValue) = constants->zEnable;
					break;
				case D3DRS_FILLMODE:
					(*pValue) = constants->fillMode;
					break;
				case D3DRS_SHADEMODE:
					(*pValue) = constants->shadeMode;
					break;
				case D3DRS_ZWRITEENABLE:
					(*pValue) = constants->zWriteEnable;
					break;
				case D3DRS_ALPHATESTENABLE:
					(*pValue) = constants->alphaTestEnable;
					break;
				case D3DRS_LASTPIXEL:
					(*pValue) = constants->lastPixel;
					break;
				case D3DRS_SRCBLEND:
					(*pValue) = constants->sourceBlend;
					break;
				case D3DRS_DESTBLEND:
					(*pValue) = constants->destinationBlend;
					break;
				case D3DRS_CULLMODE:
					(*pValue) = constants->cullMode;
					break;
				case D3DRS_ZFUNC:
					(*pValue) = constants->zFunction;
					break;
				case D3DRS_ALPHAREF:
					(*pValue) = constants->alphaReference;
					break;
				case D3DRS_ALPHAFUNC:
					(*pValue) = constants->alphaFunction;
					break;
				case D3DRS_DITHERENABLE:
					(*pValue) = constants->ditherEnable;
					break;
				case D3DRS_ALPHABLENDENABLE:
					(*pValue) = constants->alphaBlendEnable;
					break;
				case D3DRS_FOGENABLE:
					(*pValue) = constants->fogEnable;
					break;
				case D3DRS_SPECULARENABLE:
					(*pValue) = constants->specularEnable;
					break;
				case D3DRS_FOGCOLOR:
					(*pValue) = constants->fogColor;
					break;
				case D3DRS_FOGTABLEMODE:
					(*pValue) = constants->fogTableMode;
					break;
				case D3DRS_FOGSTART:
					(*pValue) = bit_cast(constants->fogStart);
					break;
				case D3DRS_FOGEND:
					(*pValue) = bit_cast(constants->fogEnd);
					break;
				case D3DRS_FOGDENSITY:
					(*pValue) = bit_cast(constants->fogDensity);
					break;
				case D3DRS_RANGEFOGENABLE:
					(*pValue) = constants->rangeFogEnable;
					break;
				case D3DRS_STENCILENABLE:
					(*pValue) = constants->stencilEnable;
					break;
				case D3DRS_STENCILFAIL:
					(*pValue) = constants->stencilFail;
					break;
				case D3DRS_STENCILZFAIL:
					(*pValue) = constants->stencilZFail;
					break;
				case D3DRS_STENCILPASS:
					(*pValue) = constants->stencilPass;
					break;
				case D3DRS_STENCILFUNC:
					(*pValue) = constants->stencilFunction;
					break;
				case D3DRS_STENCILREF:
					(*pValue) = constants->stencilReference;
					break;
				case D3DRS_STENCILMASK:
					(*pValue) = constants->stencilMask;
					break;
				case D3DRS_STENCILWRITEMASK:
					(*pValue) = constants->stencilWriteMask;
					break;
				case D3DRS_TEXTUREFACTOR:
					(*pValue) = constants->textureFactor;
					break;
				case D3DRS_WRAP0:
					(*pValue) = constants->wrap0;
					break;
				case D3DRS_WRAP1:
					(*pValue) = constants->wrap1;
					break;
				case D3DRS_WRAP2:
					(*pValue) = constants->wrap2;
					break;
				case D3DRS_WRAP3:
					(*pValue) = constants->wrap3;
					break;
				case D3DRS_WRAP4:
					(*pValue) = constants->wrap4;
					break;
				case D3DRS_WRAP5:
					(*pValue) = constants->wrap5;
					break;
				case D3DRS_WRAP6:
					(*pValue) = constants->wrap6;
					break;
				case D3DRS_WRAP7:
					(*pValue) = constants->wrap7;
					break;
				case D3DRS_CLIPPING:
					(*pValue) = constants->clipping;
					break;
				case D3DRS_LIGHTING:
					(*pValue) = constants->lighting;
					break;
				case D3DRS_AMBIENT:
					(*pValue) = constants->ambient;
					break;
				case D3DRS_FOGVERTEXMODE:
					(*pValue) = constants->fogVertexMode;
					break;
				case D3DRS_COLORVERTEX:
					(*pValue) = constants->colorVertex;
					break;
				case D3DRS_LOCALVIEWER:
					(*pValue) = constants->localViewer;
					break;
				case D3DRS_NORMALIZENORMALS:
					(*pValue) = constants->normalizeNormals;
					break;
				case D3DRS_DIFFUSEMATERIALSOURCE:
					(*pValue) = constants->diffuseMaterialSource;
					break;
				case D3DRS_SPECULARMATERIALSOURCE:
					(*pValue) = constants->specularMaterialSource;
					break;
				case D3DRS_AMBIENTMATERIALSOURCE:
					(*pValue) = constants->ambientMaterialSource;
					break;
				case D3DRS_EMISSIVEMATERIALSOURCE:
					(*pValue) = constants->emissiveMaterialSource;
					break;
				case D3DRS_VERTEXBLEND:
					(*pValue) = constants->vertexBlend;
					break;
				case D3DRS_CLIPPLANEENABLE:
					(*pValue) = constants->clipPlaneEnable;
					break;
				case D3DRS_POINTSIZE:
					(*pValue) = constants->pointSize;
					break;
				case D3DRS_POINTSIZE_MIN:
					(*pValue) = bit_cast(constants->pointSizeMinimum);
					break;
				case D3DRS_POINTSPRITEENABLE:
					(*pValue) = constants->pointSpriteEnable;
					break;
				case D3DRS_POINTSCALEENABLE:
					(*pValue) = constants->pointScaleEnable;
					break;
				case D3DRS_POINTSCALE_A:
					(*pValue) = bit_cast(constants->pointScaleA);
					break;
				case D3DRS_POINTSCALE_B:
					(*pValue) = bit_cast(constants->pointScaleB);
					break;
				case D3DRS_POINTSCALE_C:
					(*pValue) = bit_cast(constants->pointScaleC);
					break;
				case D3DRS_MULTISAMPLEANTIALIAS:
					(*pValue) = constants->multisampleAntiAlias;
					break;
				case D3DRS_MULTISAMPLEMASK:
					(*pValue) = constants->multisampleMask;
					break;
				case D3DRS_PATCHEDGESTYLE:
					(*pValue) = constants->patchEdgeStyle;
					break;
				case D3DRS_DEBUGMONITORTOKEN:
					(*pValue) = constants->debugMonitorToken;
					break;
				case D3DRS_POINTSIZE_MAX:
					(*pValue) = bit_cast(constants->pointSizeMaximum);
					break;
				case D3DRS_INDEXEDVERTEXBLENDENABLE:
					(*pValue) = constants->indexedVertexBlendEnable;
					break;
				case D3DRS_COLORWRITEENABLE:
					(*pValue) = constants->colorWriteEnable;
					break;
				case D3DRS_TWEENFACTOR:
					(*pValue) = bit_cast(constants->tweenFactor);
					break;
				case D3DRS_BLENDOP:
					(*pValue) = constants->blendOperation;
					break;
				case D3DRS_POSITIONDEGREE:
					(*pValue) = constants->positionDegree;
					break;
				case D3DRS_NORMALDEGREE:
					(*pValue) = constants->normalDegree;
					break;
				case D3DRS_SCISSORTESTENABLE:
					(*pValue) = constants->scissorTestEnable;
					break;
				case D3DRS_SLOPESCALEDEPTHBIAS:
					(*pValue) = bit_cast(constants->slopeScaleDepthBias);
					break;
				case D3DRS_ANTIALIASEDLINEENABLE:
					(*pValue) = constants->antiAliasedLineEnable;
					break;
				case D3DRS_MINTESSELLATIONLEVEL:
					(*pValue) = bit_cast(constants->minimumTessellationLevel);
					break;
				case D3DRS_MAXTESSELLATIONLEVEL:
					(*pValue) = bit_cast(constants->maximumTessellationLevel);
					break;
				case D3DRS_ADAPTIVETESS_X:
					(*pValue) = bit_cast(constants->adaptivetessX);
					break;
				case D3DRS_ADAPTIVETESS_Y:
					(*pValue) = bit_cast(constants->adaptivetessY);
					break;
				case D3DRS_ADAPTIVETESS_Z:
					(*pValue) = bit_cast(constants->adaptivetessZ);
					break;
				case D3DRS_ADAPTIVETESS_W:
					(*pValue) = bit_cast(constants->adaptivetessW);
					break;
				case D3DRS_ENABLEADAPTIVETESSELLATION:
					(*pValue) = constants->enableAdaptiveTessellation;
					break;
				case D3DRS_TWOSIDEDSTENCILMODE:
					(*pValue) = constants->twoSidedStencilMode;
					break;
				case D3DRS_CCW_STENCILFAIL:
					(*pValue) = constants->ccwStencilFail;
					break;
				case D3DRS_CCW_STENCILZFAIL:
					(*pValue) = constants->ccwStencilZFail;
					break;
				case D3DRS_CCW_STENCILPASS:
					(*pValue) = constants->ccwStencilPass;
					break;
				case D3DRS_CCW_STENCILFUNC:
					(*pValue) = constants->ccwStencilFunction;
					break;
				case D3DRS_COLORWRITEENABLE1:
					(*pValue) = constants->colorWriteEnable1;
					break;
				case D3DRS_COLORWRITEENABLE2:
					(*pValue) = constants->colorWriteEnable2;
					break;
				case D3DRS_COLORWRITEENABLE3:
					(*pValue) = constants->colorWriteEnable3;
					break;
				case D3DRS_BLENDFACTOR:
					(*pValue) = constants->blendFactor;
					break;
				case D3DRS_SRGBWRITEENABLE:
					(*pValue) = constants->srgbWriteEnable;
					break;
				case D3DRS_DEPTHBIAS:
					(*pValue) = bit_cast(constants->depthBias);
					break;
				case D3DRS_WRAP8:
					(*pValue) = constants->wrap8;
					break;
				case D3DRS_WRAP9:
					(*pValue) = constants->wrap9;
					break;
				case D3DRS_WRAP10:
					(*pValue) = constants->wrap10;
					break;
				case D3DRS_WRAP11:
					(*pValue) = constants->wrap11;
					break;
				case D3DRS_WRAP12:
					(*pValue) = constants->wrap12;
					break;
				case D3DRS_WRAP13:
					(*pValue) = constants->wrap13;
					break;
				case D3DRS_WRAP14:
					(*pValue) = constants->wrap14;
					break;
				case D3DRS_WRAP15:
					(*pValue) = constants->wrap15;
					break;
				case D3DRS_SEPARATEALPHABLENDENABLE:
					(*pValue) = constants->separateAlphaBlendEnable;
					break;
				case D3DRS_SRCBLENDALPHA:
					(*pValue) = constants->sourceBlendAlpha;
					break;
				case D3DRS_DESTBLENDALPHA:
					(*pValue) = constants->destinationBlendAlpha;
					break;
				case D3DRS_BLENDOPALPHA:
					(*pValue) = constants->blendOperationAlpha;
					break;
				default:
					BOOST_LOG_TRIVIAL(warning) << "CDevice9::GetRenderState unknown state! " << State;
					break;
				}
			}
			break;
			case Device_GetSamplerState:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				DWORD Sampler = boost::any_cast<D3DRENDERSTATETYPE>(workItem.Argument1);
				D3DSAMPLERSTATETYPE Type = boost::any_cast<D3DSAMPLERSTATETYPE>(workItem.Argument2);
				DWORD* pValue = boost::any_cast<DWORD*>(workItem.Argument3);

				(*pValue) = realWindow.mDeviceState.mSamplerStates[Sampler][Type];
			}
			break;
			case Device_GetScissorRect:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				RECT* pRect = boost::any_cast<RECT*>(workItem.Argument1);

				(*pRect) = realWindow.mDeviceState.m9Scissor;
			}
			break;
			case Device_GetStreamSource:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				UINT StreamNumber = boost::any_cast<UINT>(workItem.Argument1);
				IDirect3DVertexBuffer9** ppStreamData = boost::any_cast<IDirect3DVertexBuffer9**>(workItem.Argument2);
				UINT* pOffsetInBytes = boost::any_cast<UINT*>(workItem.Argument3);
				UINT* pStride = boost::any_cast<UINT*>(workItem.Argument4);

				StreamSource& value = realWindow.mDeviceState.mStreamSources[StreamNumber];

				(*ppStreamData) = (IDirect3DVertexBuffer9*)value.StreamData;
				/*
				Vulkan wants 64bit uint but d3d9 uses 32bit uint. This cast just keeps compiler from complaining.
				This should be safe because only 32bit can be set and d3d9 is x86 only so endianness issues shouldn't come into play.
				*/
				(*pOffsetInBytes) = (UINT)value.OffsetInBytes;
				(*pStride) = value.Stride;
			}
			break;
			case Device_GetTexture:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				DWORD Stage = boost::any_cast<DWORD>(workItem.Argument1);
				IDirect3DBaseTexture9** ppTexture = boost::any_cast<IDirect3DBaseTexture9**>(workItem.Argument2);

				DeviceState* state = NULL;

				if (realWindow.mCurrentStateRecording != nullptr)
				{
					state = &realWindow.mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realWindow.mDeviceState;
				}

				auto it = state->mTextures.find(Stage);
				if (it != state->mTextures.end())
				{
					(*ppTexture) = it->second;
				}
				else
				{
					(*ppTexture) = nullptr;
				}
			}
			break;
			case Device_GetTextureStageState:
			{
				auto& realWindow = (*commandStreamManager->mRenderManager.mStateManager.mWindows[workItem.Id]);
				DWORD Stage = boost::any_cast<DWORD>(workItem.Argument1);
				D3DTEXTURESTAGESTATETYPE Type = boost::any_cast<D3DTEXTURESTAGESTATETYPE>(workItem.Argument2);
				DWORD* pValue = boost::any_cast<DWORD*>(workItem.Argument3);

				DeviceState* state = nullptr;

				if (realWindow.mCurrentStateRecording != nullptr)
				{
					state = &realWindow.mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realWindow.mDeviceState;
				}

				switch (Type)
				{
				case D3DTSS_COLOROP:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.colorOperation_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.colorOperation_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.colorOperation_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.colorOperation_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.colorOperation_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.colorOperation_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.colorOperation_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.colorOperation_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_COLORARG1:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.colorArgument1_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.colorArgument1_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.colorArgument1_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.colorArgument1_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.colorArgument1_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.colorArgument1_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.colorArgument1_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.colorArgument1_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_COLORARG2:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.colorArgument2_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.colorArgument2_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.colorArgument2_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.colorArgument2_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.colorArgument2_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.colorArgument2_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.colorArgument2_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.colorArgument2_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_ALPHAOP:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.alphaOperation_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.alphaOperation_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.alphaOperation_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.alphaOperation_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.alphaOperation_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.alphaOperation_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.alphaOperation_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.alphaOperation_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_ALPHAARG1:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.alphaArgument1_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_ALPHAARG2:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.alphaArgument2_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_BUMPENVMAT00:
					switch (Stage)
					{
					case 0:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_0);
						break;
					case 1:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_1);
						break;
					case 2:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_2);
						break;
					case 3:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_3);
						break;
					case 4:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_4);
						break;
					case 5:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_5);
						break;
					case 6:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_6);
						break;
					case 7:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix00_7);
						break;
					default:
						break;
					}
					break;
				case D3DTSS_BUMPENVMAT01:
					switch (Stage)
					{
					case 0:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_0);
						break;
					case 1:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_1);
						break;
					case 2:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_2);
						break;
					case 3:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_3);
						break;
					case 4:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_4);
						break;
					case 5:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_5);
						break;
					case 6:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_6);
						break;
					case 7:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix01_7);
						break;
					default:
						break;
					}
					break;
				case D3DTSS_BUMPENVMAT10:
					switch (Stage)
					{
					case 0:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_0);
						break;
					case 1:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_1);
						break;
					case 2:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_2);
						break;
					case 3:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_3);
						break;
					case 4:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_4);
						break;
					case 5:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_5);
						break;
					case 6:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_6);
						break;
					case 7:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix10_7);
						break;
					default:
						break;
					}
					break;
				case D3DTSS_BUMPENVMAT11:
					switch (Stage)
					{
					case 0:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_0);
						break;
					case 1:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_1);
						break;
					case 2:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_2);
						break;
					case 3:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_3);
						break;
					case 4:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_4);
						break;
					case 5:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_5);
						break;
					case 6:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_6);
						break;
					case 7:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapMatrix11_7);
						break;
					default:
						break;
					}
					break;
				case D3DTSS_TEXCOORDINDEX:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.texureCoordinateIndex_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_BUMPENVLSCALE:
					switch (Stage)
					{
					case 0:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_0);
						break;
					case 1:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_1);
						break;
					case 2:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_2);
						break;
					case 3:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_3);
						break;
					case 4:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_4);
						break;
					case 5:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_5);
						break;
					case 6:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_6);
						break;
					case 7:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapScale_7);
						break;
					default:
						break;
					}
					break;
				case D3DTSS_BUMPENVLOFFSET:
					switch (Stage)
					{
					case 0:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_0);
						break;
					case 1:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_1);
						break;
					case 2:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_2);
						break;
					case 3:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_3);
						break;
					case 4:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_4);
						break;
					case 5:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_5);
						break;
					case 6:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_6);
						break;
					case 7:
						(*pValue) = bit_cast(state->mSpecializationConstants.bumpMapOffset_7);
						break;
					default:
						break;
					}
					break;
				case D3DTSS_TEXTURETRANSFORMFLAGS:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.textureTransformationFlags_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_COLORARG0:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.colorArgument0_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.colorArgument0_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.colorArgument0_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.colorArgument0_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.colorArgument0_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.colorArgument0_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.colorArgument0_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.colorArgument0_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_ALPHAARG0:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.alphaArgument0_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_RESULTARG:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.Result_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.Result_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.Result_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.Result_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.Result_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.Result_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.Result_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.Result_7;
						break;
					default:
						break;
					}
					break;
				case D3DTSS_CONSTANT:
					switch (Stage)
					{
					case 0:
						(*pValue) = state->mSpecializationConstants.Constant_0;
						break;
					case 1:
						(*pValue) = state->mSpecializationConstants.Constant_1;
						break;
					case 2:
						(*pValue) = state->mSpecializationConstants.Constant_2;
						break;
					case 3:
						(*pValue) = state->mSpecializationConstants.Constant_3;
						break;
					case 4:
						(*pValue) = state->mSpecializationConstants.Constant_4;
						break;
					case 5:
						(*pValue) = state->mSpecializationConstants.Constant_5;
						break;
					case 6:
						(*pValue) = state->mSpecializationConstants.Constant_6;
						break;
					case 7:
						(*pValue) = state->mSpecializationConstants.Constant_7;
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}
			break;
			case Device_GetTransform:
			{

			}
			break;
			case Instance_GetAdapterIdentifier:
			{
				UINT Adapter = boost::any_cast<UINT>(workItem.Argument1);
				DWORD Flags = boost::any_cast<DWORD>(workItem.Argument2);
				D3DADAPTER_IDENTIFIER9* pIdentifier = boost::any_cast<D3DADAPTER_IDENTIFIER9*>(workItem.Argument3);
				auto instance = commandStreamManager->mRenderManager.mStateManager.mInstances[workItem.Id];
				vk::PhysicalDeviceProperties& properties = instance->mDevices[Adapter].mPhysicalDeviceProperties; //mPhysicalDeviceProperties

				(*pIdentifier) = {}; //zero it out.

				strcpy(pIdentifier->DeviceName, properties.deviceName); //256 to 32 revisit
				strcpy(pIdentifier->Driver, ""); //revisit
				strcpy(pIdentifier->Description, ""); //revisit
				pIdentifier->VendorId = properties.vendorID;
				pIdentifier->DeviceId = properties.deviceID;
				pIdentifier->DriverVersion.QuadPart = properties.driverVersion;

				//pIdentifier->SubSysId = 0;
				//pIdentifier->Revision = 0;
				//pIdentifier->DeviceIdentifier = 0;
				//pIdentifier->WHQLLevel = 0;
			}
			break;
			case Instance_GetDeviceCaps:
			{
				UINT Adapter = boost::any_cast<UINT>(workItem.Argument1);
				D3DDEVTYPE DeviceType = boost::any_cast<D3DDEVTYPE>(workItem.Argument2);
				D3DCAPS9* pCaps = boost::any_cast<D3DCAPS9*>(workItem.Argument3);
				auto instance = commandStreamManager->mRenderManager.mStateManager.mInstances[workItem.Id];
				vk::PhysicalDeviceProperties& properties = instance->mDevices[Adapter].mPhysicalDeviceProperties;
				vk::PhysicalDeviceFeatures& features = instance->mDevices[Adapter].mPhysicalDeviceFeatures;

				/*
				https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb172635(v=vs.85).aspx
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb172591(v=vs.85).aspx
				*/

				//Translate the vulkan properties & features into D3D9 capabilities.
				pCaps->DeviceType = DeviceType;
				pCaps->AdapterOrdinal = 0;
				pCaps->Caps = 0;
				pCaps->Caps2 = D3DCAPS2_CANMANAGERESOURCE | D3DCAPS2_DYNAMICTEXTURES | D3DCAPS2_FULLSCREENGAMMA | D3DCAPS2_CANAUTOGENMIPMAP;
				pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD | D3DCAPS3_COPY_TO_VIDMEM | D3DCAPS3_COPY_TO_SYSTEMMEM | D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;
				pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_DEFAULT | D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_TWO | D3DPRESENT_INTERVAL_THREE | D3DPRESENT_INTERVAL_FOUR | D3DPRESENT_INTERVAL_IMMEDIATE;
				pCaps->CursorCaps = D3DCURSORCAPS_COLOR | D3DCURSORCAPS_LOWRES;
				pCaps->DevCaps = D3DDEVCAPS_CANBLTSYSTONONLOCAL | D3DDEVCAPS_CANRENDERAFTERFLIP | D3DDEVCAPS_DRAWPRIMITIVES2 | D3DDEVCAPS_DRAWPRIMITIVES2EX | D3DDEVCAPS_DRAWPRIMTLVERTEX | D3DDEVCAPS_EXECUTESYSTEMMEMORY | D3DDEVCAPS_EXECUTEVIDEOMEMORY | D3DDEVCAPS_HWRASTERIZATION | D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_PUREDEVICE | D3DDEVCAPS_TEXTURENONLOCALVIDMEM | D3DDEVCAPS_TEXTUREVIDEOMEMORY | D3DDEVCAPS_TLVERTEXSYSTEMMEMORY | D3DDEVCAPS_TLVERTEXVIDEOMEMORY;
				pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_MASKZ | D3DPMISCCAPS_CULLNONE | D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_COLORWRITEENABLE | D3DPMISCCAPS_CLIPPLANESCALEDPOINTS | D3DPMISCCAPS_TSSARGTEMP | D3DPMISCCAPS_BLENDOP | D3DPMISCCAPS_INDEPENDENTWRITEMASKS | D3DPMISCCAPS_FOGANDSPECULARALPHA | D3DPMISCCAPS_SEPARATEALPHABLEND | D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS | D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING | D3DPMISCCAPS_FOGVERTEXCLAMPED;
				pCaps->RasterCaps = D3DPRASTERCAPS_ANISOTROPY | D3DPRASTERCAPS_COLORPERSPECTIVE | D3DPRASTERCAPS_DITHER | D3DPRASTERCAPS_DEPTHBIAS | D3DPRASTERCAPS_FOGRANGE | D3DPRASTERCAPS_FOGTABLE | D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_MIPMAPLODBIAS | D3DPRASTERCAPS_MULTISAMPLE_TOGGLE | D3DPRASTERCAPS_SCISSORTEST | D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS | D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG | D3DPRASTERCAPS_ZTEST;
				pCaps->ZCmpCaps = D3DPCMPCAPS_NEVER | D3DPCMPCAPS_LESS | D3DPCMPCAPS_EQUAL | D3DPCMPCAPS_LESSEQUAL | D3DPCMPCAPS_GREATER | D3DPCMPCAPS_NOTEQUAL | D3DPCMPCAPS_GREATEREQUAL | D3DPCMPCAPS_ALWAYS;
				pCaps->SrcBlendCaps = D3DPBLENDCAPS_ZERO | D3DPBLENDCAPS_ONE | D3DPBLENDCAPS_SRCCOLOR | D3DPBLENDCAPS_INVSRCCOLOR | D3DPBLENDCAPS_SRCALPHA | D3DPBLENDCAPS_INVSRCALPHA | D3DPBLENDCAPS_DESTALPHA | D3DPBLENDCAPS_INVDESTALPHA | D3DPBLENDCAPS_DESTCOLOR | D3DPBLENDCAPS_INVDESTCOLOR | D3DPBLENDCAPS_SRCALPHASAT | D3DPBLENDCAPS_BOTHSRCALPHA | D3DPBLENDCAPS_BOTHINVSRCALPHA | D3DPBLENDCAPS_BLENDFACTOR | D3DPBLENDCAPS_INVSRCCOLOR2 | D3DPBLENDCAPS_SRCCOLOR2;
				pCaps->DestBlendCaps = pCaps->SrcBlendCaps;
				pCaps->AlphaCmpCaps = D3DPCMPCAPS_NEVER | D3DPCMPCAPS_LESS | D3DPCMPCAPS_EQUAL | D3DPCMPCAPS_LESSEQUAL | D3DPCMPCAPS_GREATER | D3DPCMPCAPS_NOTEQUAL | D3DPCMPCAPS_GREATEREQUAL | D3DPCMPCAPS_ALWAYS;
				pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB | D3DPSHADECAPS_SPECULARGOURAUDRGB | D3DPSHADECAPS_ALPHAGOURAUDBLEND | D3DPSHADECAPS_FOGGOURAUD;
				pCaps->TextureCaps = D3DPTEXTURECAPS_ALPHA | D3DPTEXTURECAPS_ALPHAPALETTE | D3DPTEXTURECAPS_PERSPECTIVE | D3DPTEXTURECAPS_PROJECTED | D3DPTEXTURECAPS_CUBEMAP | D3DPTEXTURECAPS_VOLUMEMAP | D3DPTEXTURECAPS_POW2 | D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_CUBEMAP_POW2 | D3DPTEXTURECAPS_VOLUMEMAP_POW2 | D3DPTEXTURECAPS_MIPMAP | D3DPTEXTURECAPS_MIPVOLUMEMAP | D3DPTEXTURECAPS_MIPCUBEMAP;
				pCaps->TextureFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MINFANISOTROPIC | D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR | D3DPTFILTERCAPS_MAGFANISOTROPIC;
				pCaps->CubeTextureFilterCaps = pCaps->TextureFilterCaps;
				pCaps->VolumeTextureFilterCaps = pCaps->TextureFilterCaps;
				pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER | D3DPTADDRESSCAPS_INDEPENDENTUV | D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_MIRROR | D3DPTADDRESSCAPS_CLAMP | D3DPTADDRESSCAPS_MIRRORONCE;
				pCaps->VolumeTextureAddressCaps = pCaps->TextureAddressCaps;
				pCaps->LineCaps = D3DLINECAPS_ALPHACMP | D3DLINECAPS_BLEND | D3DLINECAPS_TEXTURE | D3DLINECAPS_ZTEST | D3DLINECAPS_FOG;
				pCaps->MaxTextureWidth = properties.limits.maxImageDimension2D; //Revisit
				pCaps->MaxTextureHeight = properties.limits.maxImageDimension2D; //Revisit
				pCaps->MaxVolumeExtent = properties.limits.maxImageDimensionCube; //Revisit
				pCaps->MaxTextureRepeat = 32768; //revisit
				pCaps->MaxTextureAspectRatio = pCaps->MaxTextureWidth;
				pCaps->MaxAnisotropy = features.samplerAnisotropy;
				pCaps->MaxVertexW = 1e10f; //revisit
				pCaps->GuardBandLeft = -1e9f; //revisit
				pCaps->GuardBandTop = -1e9f; //revisit
				pCaps->GuardBandRight = 1e9f; //revisit
				pCaps->GuardBandBottom = 1e9f; //revisit
				pCaps->ExtentsAdjust = 0.0f; //revisit
				pCaps->StencilCaps = D3DSTENCILCAPS_KEEP | D3DSTENCILCAPS_ZERO | D3DSTENCILCAPS_REPLACE | D3DSTENCILCAPS_INCRSAT | D3DSTENCILCAPS_DECRSAT | D3DSTENCILCAPS_INVERT | D3DSTENCILCAPS_INCR | D3DSTENCILCAPS_DECR | D3DSTENCILCAPS_TWOSIDED;
				pCaps->FVFCaps = D3DFVFCAPS_PSIZE;
				pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE4X | D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_ADDSIGNED | D3DTEXOPCAPS_ADDSIGNED2X | D3DTEXOPCAPS_SUBTRACT | D3DTEXOPCAPS_ADDSMOOTH | D3DTEXOPCAPS_BLENDDIFFUSEALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHA | D3DTEXOPCAPS_BLENDFACTORALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHAPM | D3DTEXOPCAPS_BLENDCURRENTALPHA | D3DTEXOPCAPS_PREMODULATE | D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA | D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR | D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA | D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE | D3DTEXOPCAPS_DOTPRODUCT3 | D3DTEXOPCAPS_MULTIPLYADD | D3DTEXOPCAPS_LERP;
				pCaps->MaxTextureBlendStages = properties.limits.maxDescriptorSetSamplers; //revisit

				/*
				I'm setting this to 16 so I can make my array 16 and be safe ish
				*/
				pCaps->MaxSimultaneousTextures = 16; // properties.limits.maxDescriptorSetSampledImages; //revisit

				pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 | D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER | D3DVTXPCAPS_TWEENING;
				pCaps->MaxActiveLights = 0;  //Revsit should be infinite but games may not read it that way.
				pCaps->MaxUserClipPlanes = 8; //revisit
				pCaps->MaxVertexBlendMatrices = 4; //revisit
				pCaps->MaxVertexBlendMatrixIndex = 7; //revisit
				pCaps->MaxPointSize = properties.limits.pointSizeRange[1]; //revisit
				pCaps->MaxPrimitiveCount = 0xFFFFFFFF; //revisit
				pCaps->MaxVertexIndex = 0xFFFFFFFF; //revisit
				pCaps->MaxStreams = properties.limits.maxVertexInputBindings; //revisit
				pCaps->MaxStreamStride = properties.limits.maxVertexInputBindingStride; //revisit
				pCaps->VertexShaderVersion = D3DVS_VERSION(3, 0);
				pCaps->MaxVertexShaderConst = 256; //revisit
				pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0);
				pCaps->PixelShader1xMaxValue = 65504.f;
				pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET | D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET | D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;
				pCaps->MasterAdapterOrdinal = 0;
				pCaps->AdapterOrdinalInGroup = 0;
				pCaps->NumberOfAdaptersInGroup = 1;
				pCaps->DeclTypes = D3DDTCAPS_UBYTE4 | D3DDTCAPS_UBYTE4N | D3DDTCAPS_SHORT2N | D3DDTCAPS_SHORT4N | D3DDTCAPS_USHORT2N | D3DDTCAPS_USHORT4N | D3DDTCAPS_UDEC3 | D3DDTCAPS_DEC3N | D3DDTCAPS_FLOAT16_2 | D3DDTCAPS_FLOAT16_4;
				pCaps->NumSimultaneousRTs = 4; //revisit
				pCaps->StretchRectFilterCaps = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR;
				pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
				pCaps->VS20Caps.DynamicFlowControlDepth = 24; //revsit
				pCaps->VS20Caps.StaticFlowControlDepth = 4; //revsit
				pCaps->VS20Caps.NumTemps = 32; //revsit
				pCaps->PS20Caps.Caps = D3DPS20CAPS_ARBITRARYSWIZZLE | D3DPS20CAPS_GRADIENTINSTRUCTIONS | D3DPS20CAPS_PREDICATION;
				pCaps->PS20Caps.DynamicFlowControlDepth = 24; //revsit
				pCaps->PS20Caps.StaticFlowControlDepth = 4; //revsit
				pCaps->PS20Caps.NumTemps = 32; //revsit
				pCaps->VertexTextureFilterCaps = pCaps->TextureFilterCaps; //revisit
				pCaps->MaxVertexShader30InstructionSlots = 32768; //revisit
				pCaps->MaxPixelShader30InstructionSlots = 32768; //revisit
				pCaps->MaxVShaderInstructionsExecuted = 65535;
				pCaps->MaxPShaderInstructionsExecuted = 65535;

				pCaps->MaxNpatchTessellationLevel = 0.0f;
				pCaps->Reserved5 = 0;
			}
			break;
			default:
			{
				BOOST_LOG_TRIVIAL(error) << "ProcessQueue unknown work item " << workItem.WorkItemType;
			}
			break;
			}
			commandStreamManager->IsBusy = 0;
		}
	}
}

CommandStreamManager::CommandStreamManager()
	: mWorkerThread(ProcessQueue, this)
{
	//Setup configuration & logging.
	mOptionDescriptions.add_options()
		("LogFile", boost::program_options::value<std::string>(), "The location of the log file.");

	boost::program_options::store(boost::program_options::parse_config_file<char>("VK9.conf", mOptionDescriptions), mOptions);
	boost::program_options::notify(mOptions);

	if (mOptions.count("LogFile"))
	{
		boost::log::add_file_log(
			boost::log::keywords::file_name = mOptions["LogFile"].as<std::string>(),
			boost::log::keywords::format = "[%TimeStamp%]: %Message%",
			boost::log::keywords::auto_flush = true
		);
	}
	else
	{
		boost::log::add_file_log(
			boost::log::keywords::file_name = "VK9.log",
			boost::log::keywords::format = "[%TimeStamp%]: %Message%",
			boost::log::keywords::auto_flush = true
		);
	}

#ifndef _DEBUG
	boost::log::core::get()->set_filter(boost::log::trivial::severity > boost::log::trivial::info);
#endif

	BOOST_LOG_TRIVIAL(info) << "CommandStreamManager::CommandStreamManager";
	}

CommandStreamManager::~CommandStreamManager()
{
	IsRunning = 0;
	BOOST_LOG_TRIVIAL(info) << "CommandStreamManager::~CommandStreamManager";
}

size_t CommandStreamManager::RequestWork(const WorkItem& workItem)
{
	while (!mWorkItems.push(workItem)) {}

	size_t key = 0;

	//fetching key should be atomic because it's an atomic size_t.
	switch (workItem.WorkItemType)
	{
	case WorkItemType::Instance_Create:
		key = mRenderManager.mStateManager.mInstanceKey++;
		break;
	case WorkItemType::Device_Create:
		key = mRenderManager.mStateManager.mWindowsKey++;
		break;
	default:
		break;
	}

	return key;
}

size_t CommandStreamManager::RequestWorkAndWait(const WorkItem& workItem)
{
	size_t result = this->RequestWork(workItem);
	while (!mWorkItems.empty() && IsBusy) {} //This is basically a spin lock.
	return result;
}