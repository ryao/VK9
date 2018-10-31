// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include "Perf_ProcessQueue.h"
#include "Perf_CommandStreamManager.h"

#include "CStateBlock9.h"
#include "CPixelShader9.h"
#include "CVertexShader9.h"
#include "CIndexBuffer9.h"
#include "CVertexBuffer9.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CDevice9.h"
#include "CVolume9.h"

#include "Utilities.h"

void ProcessQueue(CommandStreamManager* commandStreamManager)
{
	size_t count = 0;

	Sleep(100);
	while (commandStreamManager->IsRunning)
	{
		WorkItem* workItem = nullptr;

		if (commandStreamManager->mWorkItems.Pop(workItem, count))
		{
			std::lock_guard<std::mutex> lk(workItem->Mutex);
			//try
			//{
			switch (workItem->WorkItemType)
			{
			case Instance_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateInstance();
			}
			break;
			case Instance_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyInstance(workItem->Id);
			}
			break;
			case Device_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateDevice(workItem->Id, workItem->Argument1);

				//TODO: revisit
				commandStreamManager->mResult = vk::Result::eSuccess;
			}
			break;
			case Device_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyDevice(workItem->Id);
			}
			break;
			case VertexBuffer_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateVertexBuffer(workItem->Id, workItem->Argument1);
			}
			break;
			case VertexBuffer_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyVertexBuffer(workItem->Id);
			}
			break;
			case IndexBuffer_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateIndexBuffer(workItem->Id, workItem->Argument1);
			}
			break;
			case IndexBuffer_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyIndexBuffer(workItem->Id);
			}
			break;
			case Texture_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateTexture(workItem->Id, workItem->Argument1);
			}
			break;
			case Texture_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyTexture(workItem->Id);
			}
			break;
			case CubeTexture_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateCubeTexture(workItem->Id, workItem->Argument1);
			}
			break;
			case CubeTexture_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyCubeTexture(workItem->Id);
			}
			break;
			case VolumeTexture_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateVolumeTexture(workItem->Id, workItem->Argument1);
			}
			break;
			case VolumeTexture_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyVolumeTexture(workItem->Id);
			}
			break;
			case Surface_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateSurface(workItem->Id, workItem->Argument1);
			}
			break;
			case Surface_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroySurface(workItem->Id);
			}
			break;
			case Volume_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateVolume(workItem->Id, workItem->Argument1);
			}
			break;
			case Volume_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyVolume(workItem->Id);
			}
			break;
			case Shader_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateShader(workItem->Id, workItem->Argument1, workItem->Argument2, workItem->Argument3);
			}
			break;
			case Shader_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyShader(workItem->Id);
			}
			break;
			case Query_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateQuery(workItem->Id, workItem->Argument1);
			}
			break;
			case Query_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyQuery(workItem->Id);
			}
			break;
			case Device_SetRenderTarget:
			{
				CDevice9* device9 = bit_cast<CDevice9*>(workItem->Caller);
				DWORD RenderTargetIndex = bit_cast<DWORD>(workItem->Argument1);
				CSurface9* pRenderTarget = bit_cast<CSurface9*>(workItem->Argument2);

				auto& renderManager = commandStreamManager->mRenderManager;
				auto& stateManager = renderManager.mStateManager;
				auto& realDevice = stateManager.mDevices[workItem->Id];

				RealSurface* colorSurface = nullptr;
				RealTexture* colorTexture = nullptr;
				RealSurface* depthSurface = nullptr;

				if (pRenderTarget != nullptr)
				{
					auto& deviceState = realDevice->mDeviceState;
					auto& constants = deviceState.mShaderState;
					constants.screenWidth = pRenderTarget->mWidth;
					constants.screenHeight = pRenderTarget->mHeight;

					colorSurface = stateManager.mSurfaces[pRenderTarget->mId].get();

					if (pRenderTarget->mTexture != nullptr)
					{
						colorTexture = stateManager.mTextures[pRenderTarget->mTexture->mId].get();

						if (realDevice->mCurrentStateRecording != nullptr)
						{
							depthSurface = realDevice->mCurrentStateRecording->mDeviceState.mRenderTarget->mDepthSurface;
							realDevice->mCurrentStateRecording->mDeviceState.mRenderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorTexture, colorSurface, depthSurface);
						}
						else
						{
							if (deviceState.mRenderTarget != nullptr && deviceState.mRenderTarget->mIsSceneStarted)
							{
								renderManager.StopScene(realDevice);
							}

							depthSurface = deviceState.mRenderTarget->mDepthSurface;
							deviceState.mRenderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorTexture, colorSurface, depthSurface);
							realDevice->mRenderTargets.push_back(deviceState.mRenderTarget);
						}
					}
					else if (pRenderTarget->mCubeTexture != nullptr)
					{
						BOOST_LOG_TRIVIAL(fatal) << "Cube texture not supported for render target!";
					}
					else
					{
						if (realDevice->mCurrentStateRecording != nullptr)
						{
							if (realDevice->mCurrentStateRecording->mDeviceState.mRenderTarget != nullptr)
							{
								depthSurface = realDevice->mCurrentStateRecording->mDeviceState.mRenderTarget->mDepthSurface;
							}
							realDevice->mCurrentStateRecording->mDeviceState.mRenderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorSurface, depthSurface);
						}
						else
						{
							if (deviceState.mRenderTarget != nullptr)
							{
								if (deviceState.mRenderTarget->mIsSceneStarted)
								{
									renderManager.StopScene(realDevice);
								}
								depthSurface = deviceState.mRenderTarget->mDepthSurface;
							}
							deviceState.mRenderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorSurface, depthSurface);
							realDevice->mRenderTargets.push_back(deviceState.mRenderTarget);
						}
					}
				}
			}
			break;
			case Device_SetDepthStencilSurface:
			{
				CDevice9* device9 = bit_cast<CDevice9*>(workItem->Caller);
				CSurface9* pNewZStencil = bit_cast<CSurface9*>(workItem->Argument1);

				auto& stateManager = commandStreamManager->mRenderManager.mStateManager;
				auto& realDevice = stateManager.mDevices[workItem->Id];
				auto& renderManager = commandStreamManager->mRenderManager;

				RealSurface* colorSurface = nullptr;
				RealTexture* colorTexture = nullptr;
				RealSurface* depthSurface = nullptr;

				if (pNewZStencil != nullptr)
				{
					auto& constants = realDevice->mDeviceState.mShaderState;
					constants.screenWidth = pNewZStencil->mWidth;
					constants.screenHeight = pNewZStencil->mHeight;

					depthSurface = stateManager.mSurfaces[pNewZStencil->mId].get();
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& renderTarget = realDevice->mCurrentStateRecording->mDeviceState.mRenderTarget;
					if (renderTarget != nullptr)
					{
						colorSurface = renderTarget->mColorSurface;
						colorTexture = renderTarget->mColorTexture;
					}

					if (colorTexture != nullptr)
					{
						renderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorTexture, colorSurface, depthSurface);
					}
					else
					{
						renderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorSurface, depthSurface);
					}
				}
				else
				{
					auto& renderTarget = realDevice->mDeviceState.mRenderTarget;
					if (renderTarget != nullptr)
					{
						if (renderTarget->mIsSceneStarted)
						{
							renderManager.StopScene(realDevice);
						}

						colorSurface = renderTarget->mColorSurface;
						colorTexture = renderTarget->mColorTexture;
					}

					if (colorTexture != nullptr)
					{
						renderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorTexture, colorSurface, depthSurface);
					}
					else
					{
						renderTarget = std::make_shared<RealRenderTarget>(realDevice->mDevice, colorSurface, depthSurface);
					}
					realDevice->mRenderTargets.push_back(renderTarget);
				}
			}
			break;
			case Device_Clear:
			{
				DWORD Count = bit_cast<DWORD>(workItem->Argument1);
				D3DRECT* pRects = bit_cast<D3DRECT*>(workItem->Argument2);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument3);
				D3DCOLOR Color = bit_cast<D3DCOLOR>(workItem->Argument4);
				float Z = bit_cast<float>(workItem->Argument5);
				DWORD Stencil = bit_cast<DWORD>(workItem->Argument6);

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				commandStreamManager->mRenderManager.Clear(realDevice, Count, pRects, Flags, Color, Z, Stencil);
			}
			break;
			case Device_BeginScene:
			{
				auto& renderManager = commandStreamManager->mRenderManager;
				auto& realDevice = renderManager.mStateManager.mDevices[workItem->Id];
				if (!realDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
				{
					renderManager.StartScene(realDevice, false, false, false);
				}
			}
			break;
			case Device_Present:
			{
				CDevice9* device9 = bit_cast<CDevice9*>(workItem->Caller);
				RECT* pSourceRect = bit_cast<RECT*>(workItem->Argument1);
				RECT* pDestRect = bit_cast<RECT*>(workItem->Argument2);
				HWND hDestWindowOverride = bit_cast<HWND>(workItem->Argument3);
				RGNDATA* pDirtyRegion = bit_cast<RGNDATA*>(workItem->Argument4);

				if (!hDestWindowOverride)
				{
					hDestWindowOverride = device9->mFocusWindow;
				}

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				commandStreamManager->mResult = commandStreamManager->mRenderManager.Present(realDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

				//This isn't truely atomic but worst case that some extra buffers will be created.
				commandStreamManager->mFrameBit = !commandStreamManager->mFrameBit;
			}
			break;
			case Device_BeginStateBlock:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CDevice9* device = bit_cast<CDevice9*>(workItem->Argument1);

				realDevice->mCurrentStateRecording = new CStateBlock9(device);
			}
			break;
			case Device_DrawIndexedPrimitive:
			{
				D3DPRIMITIVETYPE Type = bit_cast<D3DPRIMITIVETYPE>(workItem->Argument1);
				INT BaseVertexIndex = bit_cast<INT>(workItem->Argument2);
				UINT MinIndex = bit_cast<UINT>(workItem->Argument3);
				UINT NumVertices = bit_cast<UINT>(workItem->Argument4);
				UINT StartIndex = bit_cast<UINT>(workItem->Argument5);
				UINT PrimitiveCount = bit_cast<UINT>(workItem->Argument6);

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				commandStreamManager->mRenderManager.DrawIndexedPrimitive(realDevice, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}
			break;
			case Device_DrawPrimitive:
			{
				D3DPRIMITIVETYPE PrimitiveType = bit_cast<D3DPRIMITIVETYPE>(workItem->Argument1);
				UINT StartVertex = bit_cast<UINT>(workItem->Argument2);
				UINT PrimitiveCount = bit_cast<UINT>(workItem->Argument3);

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				commandStreamManager->mRenderManager.DrawPrimitive(realDevice, PrimitiveType, StartVertex, PrimitiveCount);
			}
			break;
			case Device_EndStateBlock:
			{
				IDirect3DStateBlock9** ppSB = bit_cast<IDirect3DStateBlock9**>(workItem->Argument1);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				(*ppSB) = realDevice->mCurrentStateRecording;
				realDevice->mCurrentStateRecording = nullptr;
			}
			break;
			case Device_GetDisplayMode:
			{
				UINT iSwapChain = bit_cast<UINT>(workItem->Argument1);
				D3DDISPLAYMODE* pMode = bit_cast<D3DDISPLAYMODE*>(workItem->Argument2);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				if (iSwapChain)
				{
					//TODO: Implement.
					BOOST_LOG_TRIVIAL(warning) << "ProcessQueue multiple swapchains are not implemented!";
				}
				else
				{
					auto& colorSurface = realDevice->mDeviceState.mRenderTarget->mColorSurface;

					pMode->Height = colorSurface->mExtent.height;
					pMode->Width = colorSurface->mExtent.width;
					pMode->RefreshRate = 60; //fake it till you make it.
					pMode->Format = ConvertFormat(colorSurface->mRealFormat);
				}
			}
			break;
			case Device_GetFVF:
			{
				DWORD* pFVF = bit_cast<DWORD*>(workItem->Argument1);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				(*pFVF) = realDevice->mDeviceState.mFVF;
			}
			break;
			case Device_GetLight:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Index = bit_cast<DWORD>(workItem->Argument1);
				D3DLIGHT9* pLight = bit_cast<D3DLIGHT9*>(workItem->Argument2);
				auto& light = realDevice->mDeviceState.mShaderState.mLights[Index];

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
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Index = bit_cast<DWORD>(workItem->Argument1);
				BOOL* pEnable = bit_cast<BOOL*>(workItem->Argument2);

				(*pEnable) = realDevice->mDeviceState.mShaderState.mLights[Index].IsEnabled;
			}
			break;
			case Device_GetMaterial:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DMATERIAL9* pMaterial = bit_cast<D3DMATERIAL9*>(workItem->Argument1);

				(*pMaterial) = realDevice->mDeviceState.mShaderState.mMaterial;
			}
			break;
			case Device_GetNPatchMode:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				FLOAT* output = bit_cast<FLOAT*>(workItem->Argument1);

				(*output) = realDevice->mDeviceState.mNSegments;
			}
			break;
			case Device_GetPixelShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DPixelShader9** ppShader = bit_cast<IDirect3DPixelShader9**>(workItem->Argument1);

				(*ppShader) = (IDirect3DPixelShader9*)realDevice->mDeviceState.mPixelShader;
			}
			break;
			case Device_GetPixelShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					pConstantData[i] = slots.BooleanConstants[StartRegister + i];
				}
			}
			break;
			case Device_GetPixelShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
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
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
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
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DRENDERSTATETYPE State = bit_cast<D3DRENDERSTATETYPE>(workItem->Argument1);
				DWORD* pValue = bit_cast<DWORD*>(workItem->Argument2);

				ShaderState* constants = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					constants = &realDevice->mCurrentStateRecording->mDeviceState.mShaderState;
				}
				else
				{
					constants = &realDevice->mDeviceState.mShaderState;
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
					BOOST_LOG_TRIVIAL(warning) << "ProcessQueue unknown state! " << State;
					break;
				}
			}
			break;
			case Device_GetSamplerState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Sampler = bit_cast<D3DRENDERSTATETYPE>(workItem->Argument1);
				D3DSAMPLERSTATETYPE Type = bit_cast<D3DSAMPLERSTATETYPE>(workItem->Argument2);
				DWORD* pValue = bit_cast<DWORD*>(workItem->Argument3);

				(*pValue) = realDevice->mDeviceState.mSamplerStates[Sampler][Type];
			}
			break;
			case Device_GetScissorRect:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				RECT* pRect = bit_cast<RECT*>(workItem->Argument1);

				(*pRect) = realDevice->mDeviceState.m9Scissor;
			}
			break;
			case Device_GetStreamSource:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StreamNumber = bit_cast<UINT>(workItem->Argument1);
				IDirect3DVertexBuffer9** ppStreamData = bit_cast<IDirect3DVertexBuffer9**>(workItem->Argument2);
				UINT* pOffsetInBytes = bit_cast<UINT*>(workItem->Argument3);
				UINT* pStride = bit_cast<UINT*>(workItem->Argument4);

				StreamSource& value = realDevice->mDeviceState.mStreamSources[StreamNumber];

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
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Stage = bit_cast<DWORD>(workItem->Argument1);
				IDirect3DBaseTexture9** ppTexture = bit_cast<IDirect3DBaseTexture9**>(workItem->Argument2);

				DeviceState* state = NULL;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				(*ppTexture) = state->mTextures[Stage];
			}
			break;
			case Device_GetTextureStageState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Stage = bit_cast<DWORD>(workItem->Argument1);
				D3DTEXTURESTAGESTATETYPE Type = bit_cast<D3DTEXTURESTAGESTATETYPE>(workItem->Argument2);
				DWORD* pValue = bit_cast<DWORD*>(workItem->Argument3);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				switch (Type)
				{
				case D3DTSS_COLOROP:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorOperation;
					break;
				case D3DTSS_COLORARG1:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorArgument1;
					break;
				case D3DTSS_COLORARG2:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorArgument2;
					break;
				case D3DTSS_ALPHAOP:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaOperation;
					break;
				case D3DTSS_ALPHAARG1:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaArgument1;
					break;
				case D3DTSS_ALPHAARG2:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaArgument2;
					break;
				case D3DTSS_BUMPENVMAT00:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix00;
					break;
				case D3DTSS_BUMPENVMAT01:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix01;
					break;
				case D3DTSS_BUMPENVMAT10:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix10;
					break;
				case D3DTSS_BUMPENVMAT11:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix11;
					break;
				case D3DTSS_TEXCOORDINDEX:
					(*pValue) = state->mShaderState.mTextureStages[Stage].texureCoordinateIndex;
					break;
				case D3DTSS_BUMPENVLSCALE:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapScale;
					break;
				case D3DTSS_BUMPENVLOFFSET:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapOffset;
					break;
				case D3DTSS_TEXTURETRANSFORMFLAGS:
					(*pValue) = state->mShaderState.mTextureStages[Stage].textureTransformationFlags;
					break;
				case D3DTSS_COLORARG0:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorArgument0;
					break;
				case D3DTSS_ALPHAARG0:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaArgument0;
					break;
				case D3DTSS_RESULTARG:
					(*pValue) = state->mShaderState.mTextureStages[Stage].Result;
					break;
				case D3DTSS_CONSTANT:
					(*pValue) = state->mShaderState.mTextureStages[Stage].Constant;
				default:
					break;
				}
			}
			break;
			case Device_GetTransform:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DTRANSFORMSTATETYPE State = bit_cast<D3DTRANSFORMSTATETYPE>(workItem->Argument1);
				D3DMATRIX* pMatrix = bit_cast<D3DMATRIX*>(workItem->Argument2);

				(*pMatrix) = realDevice->mDeviceState.mTransforms[State];
			}
			break;
			case Device_GetVertexDeclaration:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexDeclaration9** ppDecl = bit_cast<IDirect3DVertexDeclaration9**>(workItem->Argument1);

				(*ppDecl) = (IDirect3DVertexDeclaration9*)realDevice->mDeviceState.mVertexDeclaration;
			}
			break;
			case Device_GetVertexShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexShader9** ppShader = bit_cast<IDirect3DVertexShader9**>(workItem->Argument1);

				(*ppShader) = (IDirect3DVertexShader9*)realDevice->mDeviceState.mVertexShader;
			}
			break;
			case Device_GetVertexShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					pConstantData[i] = slots.BooleanConstants[StartRegister + i];
				}
			}
			break;
			case Device_GetVertexShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					if ((startIndex + i) < 128)
					{
						pConstantData[i] = realDevice->mDeviceState.mPushConstants[startIndex + i];
					}
					else
					{
						pConstantData[i] = slots.FloatConstants[startIndex + i];
					}
				}
			}
			break;
			case Device_GetVertexShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.IntegerConstants[startIndex + i];
				}
			}
			break;
			case Device_GetViewport:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DVIEWPORT9* pViewport = bit_cast<D3DVIEWPORT9*>(workItem->Argument1);

				(*pViewport) = realDevice->mDeviceState.m9Viewport;
			}
			break;
			case Device_LightEnable:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD LightIndex = bit_cast<DWORD>(workItem->Argument1);
				BOOL bEnable = bit_cast<BOOL>(workItem->Argument2);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mShaderState.mLights[LightIndex].IsEnabled = bEnable;
				state->mIsShaderStateDirty = true;
			}
			break;
			case Device_Reset:
			{
				auto& stateManager = commandStreamManager->mRenderManager.mStateManager;
				HWND FocusWindow = bit_cast<HWND>(workItem->Argument1);

				stateManager.mSwapChains.erase(FocusWindow);
			}
			break;
			case Device_SetFVF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD FVF = bit_cast<DWORD>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mFVF = FVF;
					deviceState.mHasFVF = true;
					deviceState.mHasVertexDeclaration = false;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mFVF = FVF;
					deviceState.mHasFVF = true;
					deviceState.mHasVertexDeclaration = false;
				}
			}
			break;
			case Device_SetIndices:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DIndexBuffer9* pIndexData = bit_cast<IDirect3DIndexBuffer9*>(workItem->Argument1);
				DeviceState* state = nullptr;
				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				if (pIndexData != nullptr)
				{
					auto realIndex = commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[((CIndexBuffer9*)pIndexData)->mId];

					state->mIndexBuffer = realIndex.get();
					state->mOriginalIndexBuffer = (CIndexBuffer9*)pIndexData;
					state->mHasIndexBuffer = true;
				}
				else
				{
					state->mIndexBuffer = nullptr;
					state->mOriginalIndexBuffer = nullptr;
					state->mHasIndexBuffer = false;
				}
			}
			break;
			case Device_GetIndices:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DIndexBuffer9** pIndexData = bit_cast<IDirect3DIndexBuffer9**>(workItem->Argument1);

				(*pIndexData) = realDevice->mDeviceState.mOriginalIndexBuffer;
			}
			break;
			case Device_SetLight:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Index = bit_cast<DWORD>(workItem->Argument1);
				D3DLIGHT9* pLight = bit_cast<D3DLIGHT9*>(workItem->Argument2);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				Light light = {};

				light.Type = pLight->Type;
				light.Diffuse[0] = pLight->Diffuse.r;
				light.Diffuse[1] = pLight->Diffuse.g;
				light.Diffuse[2] = pLight->Diffuse.b;
				light.Diffuse[3] = pLight->Diffuse.a;

				light.Specular[0] = pLight->Specular.r;
				light.Specular[1] = pLight->Specular.g;
				light.Specular[2] = pLight->Specular.b;
				light.Specular[3] = pLight->Specular.a;

				light.Ambient[0] = pLight->Ambient.r;
				light.Ambient[1] = pLight->Ambient.g;
				light.Ambient[2] = pLight->Ambient.b;
				light.Ambient[3] = pLight->Ambient.a;

				light.Position[0] = pLight->Position.x;
				light.Position[1] = pLight->Position.y;
				light.Position[2] = pLight->Position.z;
				//No need to set [3] because structure is init with {} so it's already 0.

				light.Direction[0] = pLight->Direction.x;
				light.Direction[1] = pLight->Direction.y;
				light.Direction[2] = pLight->Direction.z;
				//No need to set [3] because structure is init with {} so it's already 0.

				light.Range = pLight->Range;
				light.Falloff = pLight->Falloff;
				light.Attenuation0 = pLight->Attenuation0;
				light.Attenuation1 = pLight->Attenuation1;
				light.Attenuation2 = pLight->Attenuation2;
				light.Theta = pLight->Theta;
				light.Phi = pLight->Phi;


				light.IsEnabled = state->mShaderState.mLights[Index].IsEnabled;
				state->mShaderState.mLights[Index] = light;

				state->mIsShaderStateDirty = true;
			}
			break;
			case Device_SetMaterial:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DMATERIAL9* pMaterial = bit_cast<D3DMATERIAL9*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mShaderState.mMaterial = (*pMaterial);
					deviceState.mIsShaderStateDirty = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mShaderState.mMaterial = (*pMaterial);
					deviceState.mIsShaderStateDirty = true;
				}
			}
			break;
			case Device_SetNPatchMode:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				float nSegments = bit_cast<float>(workItem->Argument1);

				if (nSegments > 0.0f)
				{
					BOOST_LOG_TRIVIAL(warning) << "ProcessQueue nPatch greater than zero not supported.";
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					realDevice->mCurrentStateRecording->mDeviceState.mNSegments = nSegments;
				}
				else
				{
					realDevice->mDeviceState.mNSegments = nSegments;
				}
			}
			break;
			case Device_SetPixelShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DPixelShader9* pShader = bit_cast<IDirect3DPixelShader9*>(workItem->Argument1);

				if (pShader != nullptr)
				{
					pShader->AddRef();
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					realDevice->mCurrentStateRecording->mDeviceState.mPixelShader = (CPixelShader9*)pShader;
					realDevice->mCurrentStateRecording->mDeviceState.mHasPixelShader = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					if (deviceState.mPixelShader != nullptr)
					{
						deviceState.mPixelShader->Release();
					}

					deviceState.mPixelShader = (CPixelShader9*)pShader;
					deviceState.mHasPixelShader = true;
				}
			}
			break;
			case Device_SetPixelShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					slots.BooleanConstants[StartRegister + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetPixelShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.FloatConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetPixelShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.IntegerConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetRenderState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DRENDERSTATETYPE State = bit_cast<D3DRENDERSTATETYPE>(workItem->Argument1);
				DWORD Value = bit_cast<DWORD>(workItem->Argument2);

				ShaderState* constants = nullptr;
				DeviceState* state = NULL;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					constants = &realDevice->mCurrentStateRecording->mDeviceState.mShaderState;
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					constants = &realDevice->mDeviceState.mShaderState;
					state = &realDevice->mDeviceState;
				}

				state->mIsShaderStateDirty = true;

				switch (State)
				{
				case D3DRS_ZENABLE:
					constants->zEnable = Value;
					state->wasDsaGroupModified = true;
					state->wasMultisampleGroupModified = true;
					break;
				case D3DRS_FILLMODE:
					constants->fillMode = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_SHADEMODE:
					constants->shadeMode = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_ZWRITEENABLE:
					constants->zWriteEnable = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_ALPHATESTENABLE:
					constants->alphaTestEnable = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_LASTPIXEL:
					constants->lastPixel = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_SRCBLEND:
					constants->sourceBlend = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_DESTBLEND:
					constants->destinationBlend = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_CULLMODE:
					constants->cullMode = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_ZFUNC:
					constants->zFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_ALPHAREF:
					constants->alphaReference = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_ALPHAFUNC:
					constants->alphaFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_DITHERENABLE:
					constants->ditherEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_ALPHABLENDENABLE:
					constants->alphaBlendEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_FOGENABLE:
					constants->fogEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasFogGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_SPECULARENABLE:
					constants->specularEnable = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_FOGCOLOR:
					constants->fogColor = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGTABLEMODE:
					constants->fogTableMode = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasFogGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGSTART:
					constants->fogStart = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGEND:
					constants->fogEnd = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGDENSITY:
					constants->fogDensity = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_RANGEFOGENABLE:
					constants->rangeFogEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_STENCILENABLE:
					constants->stencilEnable = Value;
					state->wasDsaGroupModified = true;
					state->wasMultisampleGroupModified = true;
					break;
				case D3DRS_STENCILFAIL:
					constants->stencilFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILZFAIL:
					constants->stencilZFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILPASS:
					constants->stencilPass = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILFUNC:
					constants->stencilFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILREF:
					constants->stencilReference = Value;
					state->hasStencilReference = true;
					break;
				case D3DRS_STENCILMASK:
					constants->stencilMask = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILWRITEMASK:
					constants->stencilWriteMask = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_TEXTUREFACTOR:
					constants->textureFactor = Value;
					state->hasTextureFactor = true;
					break;
				case D3DRS_WRAP0:
					constants->wrap0 = Value;
					state->hasWrap0 = true;
					break;
				case D3DRS_WRAP1:
					constants->wrap1 = Value;
					state->hasWrap1 = true;
					break;
				case D3DRS_WRAP2:
					constants->wrap2 = Value;
					state->hasWrap2 = true;
					break;
				case D3DRS_WRAP3:
					constants->wrap3 = Value;
					state->hasWrap3 = true;
					break;
				case D3DRS_WRAP4:
					constants->wrap4 = Value;
					state->hasWrap4 = true;
					break;
				case D3DRS_WRAP5:
					constants->wrap5 = Value;
					state->hasWrap5 = true;
					break;
				case D3DRS_WRAP6:
					constants->wrap6 = Value;
					state->hasWrap6 = true;
					break;
				case D3DRS_WRAP7:
					constants->wrap7 = Value;
					state->hasWrap7 = true;
					break;
				case D3DRS_CLIPPING:
					constants->clipping = Value;
					state->hasClipping = true;
					break;
				case D3DRS_LIGHTING:
					constants->lighting = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_AMBIENT:
					constants->ambient = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_FOGVERTEXMODE:
					constants->fogVertexMode = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_COLORVERTEX:
					constants->colorVertex = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_LOCALVIEWER:
					constants->localViewer = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_NORMALIZENORMALS:
					constants->normalizeNormals = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_DIFFUSEMATERIALSOURCE:
					constants->diffuseMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_SPECULARMATERIALSOURCE:
					constants->specularMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_AMBIENTMATERIALSOURCE:
					constants->ambientMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_EMISSIVEMATERIALSOURCE:
					constants->emissiveMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_VERTEXBLEND:
					constants->vertexBlend = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_CLIPPLANEENABLE:
					constants->clipPlaneEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSIZE:
					constants->pointSize = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSIZE_MIN:
					constants->pointSizeMinimum = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSPRITEENABLE:
					constants->pointSpriteEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSCALEENABLE:
					constants->pointScaleEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_POINTSCALE_A:
					constants->pointScaleA = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_POINTSCALE_B:
					constants->pointScaleB = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_POINTSCALE_C:
					constants->pointScaleC = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_MULTISAMPLEANTIALIAS:
					constants->multisampleAntiAlias = Value;
					state->wasMultisampleGroupModified = true;
					break;
				case D3DRS_MULTISAMPLEMASK:
					constants->multisampleMask = Value;
					state->hasMultisampleMask = true;
					break;
				case D3DRS_PATCHEDGESTYLE:
					constants->patchEdgeStyle = Value;
					state->hasPatchEdgeStyle = true;
					break;
				case D3DRS_DEBUGMONITORTOKEN:
					constants->debugMonitorToken = Value;
					state->hasDebugMonitorToken = true;
					break;
				case D3DRS_POINTSIZE_MAX:
					constants->pointSizeMaximum = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_INDEXEDVERTEXBLENDENABLE:
					constants->indexedVertexBlendEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE:
					constants->colorWriteEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_TWEENFACTOR:
					constants->tweenFactor = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_BLENDOP:
					constants->blendOperation = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_POSITIONDEGREE:
					constants->positionDegree = Value;
					state->hasPositionDegree = true;
					break;
				case D3DRS_NORMALDEGREE:
					constants->normalDegree = Value;
					state->hasNormalDegree = true;
					break;
				case D3DRS_SCISSORTESTENABLE:
					constants->scissorTestEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_SLOPESCALEDEPTHBIAS:
					constants->slopeScaleDepthBias = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_ANTIALIASEDLINEENABLE:
					constants->antiAliasedLineEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_MINTESSELLATIONLEVEL:
					constants->minimumTessellationLevel = bit_cast(Value);
					state->hasMinimumTessellationLevel = true;
					break;
				case D3DRS_MAXTESSELLATIONLEVEL:
					constants->maximumTessellationLevel = bit_cast(Value);
					state->hasMaximumTessellationLevel = true;
					break;
				case D3DRS_ADAPTIVETESS_X:
					constants->adaptivetessX = bit_cast(Value);
					state->hasAdaptivetessX = true;
					break;
				case D3DRS_ADAPTIVETESS_Y:
					constants->adaptivetessY = bit_cast(Value);
					state->hasAdaptivetessY = true;
					break;
				case D3DRS_ADAPTIVETESS_Z:
					constants->adaptivetessZ = bit_cast(Value);
					state->hasAdaptivetessZ = true;
					break;
				case D3DRS_ADAPTIVETESS_W:
					constants->adaptivetessW = bit_cast(Value);
					state->hasAdaptivetessW = true;
					break;
				case D3DRS_ENABLEADAPTIVETESSELLATION:
					constants->enableAdaptiveTessellation = Value;
					state->hasEnableAdaptiveTessellation = true;
					break;
				case D3DRS_TWOSIDEDSTENCILMODE:
					constants->twoSidedStencilMode = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILFAIL:
					constants->ccwStencilFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILZFAIL:
					constants->ccwStencilZFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILPASS:
					constants->ccwStencilPass = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILFUNC:
					constants->ccwStencilFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE1:
					constants->colorWriteEnable1 = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE2:
					constants->colorWriteEnable2 = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE3:
					constants->colorWriteEnable3 = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_BLENDFACTOR:
					constants->blendFactor = Value;
					state->hasBlendFactor = true;
					break;
				case D3DRS_SRGBWRITEENABLE:
					constants->srgbWriteEnable = Value;
					state->hasSrgbWriteEnable = true;
					break;
				case D3DRS_DEPTHBIAS:
					constants->depthBias = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_WRAP8:
					constants->wrap8 = Value;
					state->hasWrap8 = true;
					break;
				case D3DRS_WRAP9:
					constants->wrap9 = Value;
					state->hasWrap9 = true;
					break;
				case D3DRS_WRAP10:
					constants->wrap10 = Value;
					state->hasWrap10 = true;
					break;
				case D3DRS_WRAP11:
					constants->wrap11 = Value;
					state->hasWrap11 = true;
					break;
				case D3DRS_WRAP12:
					constants->wrap12 = Value;
					state->hasWrap12 = true;
					break;
				case D3DRS_WRAP13:
					constants->wrap13 = Value;
					state->hasWrap13 = true;
					break;
				case D3DRS_WRAP14:
					constants->wrap14 = Value;
					state->hasWrap14 = true;
					break;
				case D3DRS_WRAP15:
					constants->wrap15 = Value;
					state->hasWrap15 = true;
					break;
				case D3DRS_SEPARATEALPHABLENDENABLE:
					constants->separateAlphaBlendEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_SRCBLENDALPHA:
					constants->sourceBlendAlpha = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_DESTBLENDALPHA:
					constants->destinationBlendAlpha = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_BLENDOPALPHA:
					constants->blendOperationAlpha = Value;
					state->wasBlendGroupModified = true;
					break;
				default:
					BOOST_LOG_TRIVIAL(warning) << "ProcessQueue unknown state! " << State;
					break;
				}
			}
			break;
			case Device_SetSamplerState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Sampler = bit_cast<DWORD>(workItem->Argument1);
				D3DSAMPLERSTATETYPE Type = bit_cast<D3DSAMPLERSTATETYPE>(workItem->Argument2);
				DWORD Value = bit_cast<DWORD>(workItem->Argument3);

				DeviceState* state = NULL;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mSamplerStates[Sampler][Type] = Value;
			}
			break;
			case Device_SetScissorRect:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				RECT* pRect = bit_cast<RECT*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.m9Scissor = (*pRect);

					deviceState.mScissor.extent.width = deviceState.m9Scissor.right;
					deviceState.mScissor.extent.height = deviceState.m9Scissor.bottom;
					deviceState.mScissor.offset.x = deviceState.m9Scissor.left;
					deviceState.mScissor.offset.y = deviceState.m9Scissor.top;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.m9Scissor = (*pRect);

					deviceState.mScissor.extent.width = deviceState.m9Scissor.right;
					deviceState.mScissor.extent.height = deviceState.m9Scissor.bottom;
					deviceState.mScissor.offset.x = deviceState.m9Scissor.left;
					deviceState.mScissor.offset.y = deviceState.m9Scissor.top;
				}
			}
			break;
			case Device_SetStreamSource:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StreamNumber = bit_cast<UINT>(workItem->Argument1);
				IDirect3DVertexBuffer9* pStreamData = bit_cast<IDirect3DVertexBuffer9*>(workItem->Argument2);
				UINT OffsetInBytes = bit_cast<UINT>(workItem->Argument3);
				UINT Stride = bit_cast<UINT>(workItem->Argument4);

				CVertexBuffer9* streamData = (CVertexBuffer9*)pStreamData;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					realDevice->mCurrentStateRecording->mDeviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
				}
				else
				{
					realDevice->mDeviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
				}
			}
			break;
			case Device_SetTexture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Sampler = bit_cast<DWORD>(workItem->Argument1);
				IDirect3DBaseTexture9* pTexture = bit_cast<IDirect3DBaseTexture9*>(workItem->Argument2);

				DeviceState* state = NULL;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mTextures[Sampler] = pTexture;
			}
			break;
			case Device_SetTextureStageState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Stage = bit_cast<DWORD>(workItem->Argument1);
				D3DTEXTURESTAGESTATETYPE Type = bit_cast<D3DTEXTURESTAGESTATETYPE>(workItem->Argument2);
				DWORD Value = bit_cast<DWORD>(workItem->Argument3);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mIsShaderStateDirty = true;

				switch (Type)
				{
				case D3DTSS_COLOROP:
					state->mShaderState.mTextureStages[Stage].colorOperation = Value;
					break;
				case D3DTSS_COLORARG1:
					state->mShaderState.mTextureStages[Stage].colorArgument1 = Value;
					break;
				case D3DTSS_COLORARG2:
					state->mShaderState.mTextureStages[Stage].colorArgument2 = Value;
					break;
				case D3DTSS_ALPHAOP:
					state->mShaderState.mTextureStages[Stage].alphaOperation = Value;
					break;
				case D3DTSS_ALPHAARG1:
					state->mShaderState.mTextureStages[Stage].alphaArgument1 = Value;
					break;
				case D3DTSS_ALPHAARG2:
					state->mShaderState.mTextureStages[Stage].alphaArgument2 = Value;
					break;
				case D3DTSS_BUMPENVMAT00:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix00 = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVMAT01:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix01 = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVMAT10:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix10 = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVMAT11:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix11 = bit_cast(Value);
					break;
				case D3DTSS_TEXCOORDINDEX:
					state->mShaderState.mTextureStages[Stage].texureCoordinateIndex = Value;
					break;
				case D3DTSS_BUMPENVLSCALE:
					state->mShaderState.mTextureStages[Stage].bumpMapScale = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVLOFFSET:
					state->mShaderState.mTextureStages[Stage].bumpMapOffset = bit_cast(Value);
					break;
				case D3DTSS_TEXTURETRANSFORMFLAGS:
					state->mShaderState.mTextureStages[Stage].textureTransformationFlags = Value;
					break;
				case D3DTSS_COLORARG0:
					state->mShaderState.mTextureStages[Stage].colorArgument0 = Value;
					break;
				case D3DTSS_ALPHAARG0:
					state->mShaderState.mTextureStages[Stage].alphaArgument0 = Value;
					break;
				case D3DTSS_RESULTARG:
					state->mShaderState.mTextureStages[Stage].Result = Value;
					break;
				case D3DTSS_CONSTANT:
					state->mShaderState.mTextureStages[Stage].Constant = Value;
					break;
				default:
					break;
				}
			}
			break;
			case Device_SetTransform:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DTRANSFORMSTATETYPE State = bit_cast<D3DTRANSFORMSTATETYPE>(workItem->Argument1);
				D3DMATRIX* pMatrix = bit_cast<D3DMATRIX*>(workItem->Argument2);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mShaderState.mTextureStages[State].textureTransformationMatrix = (*pMatrix);
					deviceState.mIsShaderStateDirty = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mShaderState.mTextureStages[State].textureTransformationMatrix = (*pMatrix);
					deviceState.mIsShaderStateDirty = true;
				}
			}
			break;
			case Device_SetVertexDeclaration:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexDeclaration9* pDecl = bit_cast<IDirect3DVertexDeclaration9*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

					deviceState.mHasVertexDeclaration = true;
					deviceState.mHasFVF = false;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

					deviceState.mHasVertexDeclaration = true;
					deviceState.mHasFVF = false;
				}
			}
			break;
			case Device_SetVertexShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexShader9* pShader = bit_cast<IDirect3DVertexShader9*>(workItem->Argument1);

				if (pShader != nullptr)
				{
					pShader->AddRef();
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					//BOOST_LOG_TRIVIAL(info) << "Recorded VertexShader";
					realDevice->mCurrentStateRecording->mDeviceState.mVertexShader = (CVertexShader9*)pShader;
					realDevice->mCurrentStateRecording->mDeviceState.mHasVertexShader = true;
				}
				else
				{
					if (realDevice->mDeviceState.mVertexShader != nullptr)
					{
						realDevice->mDeviceState.mVertexShader->Release();
					}

					realDevice->mDeviceState.mVertexShader = (CVertexShader9*)pShader;
					realDevice->mDeviceState.mHasVertexShader = true;
				}
			}
			break;
			case Device_SetVertexShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mAreVertexShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					slots.BooleanConstants[StartRegister + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetVertexShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mAreVertexShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					if ((startIndex + i) < 128)
					{
						realDevice->mDeviceState.mPushConstants[startIndex + i] = pConstantData[i];
					}
					else
					{
						slots.FloatConstants[startIndex + i] = pConstantData[i];
					}
				}
			}
			break;
			case Device_SetVertexShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mAreVertexShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.IntegerConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetViewport:
			{
				auto& renderManager = commandStreamManager->mRenderManager;
				auto& realDevice = renderManager.mStateManager.mDevices[workItem->Id];

				D3DVIEWPORT9* pViewport = bit_cast<D3DVIEWPORT9*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.m9Viewport = (*pViewport);

					deviceState.mViewport.x = (float)deviceState.m9Viewport.X;
					deviceState.mViewport.y = (float)deviceState.m9Viewport.Y;
					deviceState.mViewport.width = (float)deviceState.m9Viewport.Width;
					deviceState.mViewport.height = (float)deviceState.m9Viewport.Height;
					deviceState.mViewport.minDepth = deviceState.m9Viewport.MinZ;
					deviceState.mViewport.maxDepth = deviceState.m9Viewport.MaxZ;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.m9Viewport = (*pViewport);

					deviceState.mViewport.x = (float)deviceState.m9Viewport.X;
					deviceState.mViewport.y = (float)deviceState.m9Viewport.Y;
					deviceState.mViewport.width = (float)deviceState.m9Viewport.Width;
					deviceState.mViewport.height = (float)deviceState.m9Viewport.Height;
					deviceState.mViewport.minDepth = deviceState.m9Viewport.MinZ;
					deviceState.mViewport.maxDepth = deviceState.m9Viewport.MaxZ;

				}
			}
			break;
			case Device_UpdateTexture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DBaseTexture9* pSourceTexture = bit_cast<IDirect3DBaseTexture9*>(workItem->Argument1);
				IDirect3DBaseTexture9* pDestinationTexture = bit_cast<IDirect3DBaseTexture9*>(workItem->Argument2);

				commandStreamManager->mRenderManager.UpdateTexture(realDevice, pSourceTexture, pDestinationTexture);
			}
			break;
			case Device_GetAvailableTextureMem:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CDevice9* device9 = (CDevice9*)workItem->Caller;
				//                                                                                            Bytes     KB      MB
				device9->mAvailableTextureMemory = (((realDevice->mEstimatedMemory - realDevice->mEstimatedMemoryUsed) / 1024) / 1024);
			}
			break;
			case Instance_GetAdapterIdentifier:
			{
				UINT Adapter = bit_cast<UINT>(workItem->Argument1);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument2);
				D3DADAPTER_IDENTIFIER9* pIdentifier = bit_cast<D3DADAPTER_IDENTIFIER9*>(workItem->Argument3);
				auto instance = commandStreamManager->mRenderManager.mStateManager.mInstances[workItem->Id];
				auto device = instance->mPhysicalDevices[Adapter];

				vk::PhysicalDeviceProperties properties;

				device.getProperties(&properties);

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
				UINT Adapter = bit_cast<UINT>(workItem->Argument1);
				D3DDEVTYPE DeviceType = bit_cast<D3DDEVTYPE>(workItem->Argument2);
				D3DCAPS9* pCaps = bit_cast<D3DCAPS9*>(workItem->Argument3);
				auto instance = commandStreamManager->mRenderManager.mStateManager.mInstances[workItem->Id];
				auto device = instance->mPhysicalDevices[Adapter];

				vk::PhysicalDeviceProperties properties;
				vk::PhysicalDeviceFeatures features;

				device.getProperties(&properties);
				device.getFeatures(&features);

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
				pCaps->MaxActiveLights = 8;  //Revsit should be infinite but games may not read it that way.
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
			case VertexBuffer_Lock:
			{
				auto& realVertexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mVertexBuffers[workItem->Id]);

				UINT OffsetToLock = bit_cast<UINT>(workItem->Argument1);
				UINT SizeToLock = bit_cast<UINT>(workItem->Argument2);
				VOID** ppbData = bit_cast<VOID**>(workItem->Argument3);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument4);
				size_t lastId = (size_t)workItem->Argument5;

				if (lastId != workItem->Id)
				{
					auto& oldRealVertexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mVertexBuffers[lastId]);

					realVertexBuffer.mRealDevice->CopyBuffer(oldRealVertexBuffer.mBuffer, realVertexBuffer.mBuffer, realVertexBuffer.mAllocationInfo.size);
				}

				if (realVertexBuffer.mData == nullptr)
				{
					vk::Result result = (vk::Result)vmaMapMemory(realVertexBuffer.mRealDevice->mAllocator, realVertexBuffer.mAllocation, &realVertexBuffer.mData);
					if (realVertexBuffer.mData == nullptr)
					{
						*ppbData = nullptr;
					}
					else
					{
						*ppbData = (char *)realVertexBuffer.mData + OffsetToLock;
					}
				}
				else
				{
					*ppbData = (char *)realVertexBuffer.mData + OffsetToLock;
				}
			}
			break;
			case VertexBuffer_Unlock:
			{
				auto& realVertexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mVertexBuffers[workItem->Id]);

				if (realVertexBuffer.mData != nullptr)
				{
					vmaFlushAllocation(realVertexBuffer.mRealDevice->mAllocator, realVertexBuffer.mAllocation, 0, VK_WHOLE_SIZE);
					vmaUnmapMemory(realVertexBuffer.mRealDevice->mAllocator, realVertexBuffer.mAllocation);
					realVertexBuffer.mData = nullptr;

					//if (realVertexBuffer.mRealDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
					//{
					//	vk::BufferMemoryBarrier uboBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, 0, 0, 0, VK_WHOLE_SIZE);
					//	uboBarrier.buffer = realVertexBuffer.mBuffer;

					//	auto& currentBuffer = realVertexBuffer.mRealDevice->mCommandBuffers[realVertexBuffer.mRealDevice->mCurrentCommandBuffer];
					//	currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);
					//}
				}
			}
			break;
			case IndexBuffer_Lock:
			{
				auto& realIndexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[workItem->Id]);
				UINT OffsetToLock = bit_cast<UINT>(workItem->Argument1);
				UINT SizeToLock = bit_cast<UINT>(workItem->Argument2);
				VOID** ppbData = bit_cast<VOID**>(workItem->Argument3);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument4);
				size_t lastId = (size_t)workItem->Argument5;

				if (lastId != workItem->Id)
				{
					auto& oldRealIndexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[lastId]);

					realIndexBuffer.mRealDevice->CopyBuffer(oldRealIndexBuffer.mBuffer, realIndexBuffer.mBuffer, realIndexBuffer.mAllocationInfo.size);
				}

				if (realIndexBuffer.mData == nullptr)
				{
					vk::Result result = (vk::Result)vmaMapMemory(realIndexBuffer.mRealDevice->mAllocator, realIndexBuffer.mAllocation, &realIndexBuffer.mData);
					if (realIndexBuffer.mData == nullptr)
					{
						(*ppbData) = nullptr;
					}
					else
					{
						(*ppbData) = (char *)realIndexBuffer.mData + OffsetToLock;
					}
				}
				else
				{
					(*ppbData) = (char *)realIndexBuffer.mData + OffsetToLock;
				}
			}
			break;
			case IndexBuffer_Unlock:
			{
				auto& realIndexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[workItem->Id]);

				if (realIndexBuffer.mData != nullptr)
				{
					vmaFlushAllocation(realIndexBuffer.mRealDevice->mAllocator, realIndexBuffer.mAllocation, 0, VK_WHOLE_SIZE);
					vmaUnmapMemory(realIndexBuffer.mRealDevice->mAllocator, realIndexBuffer.mAllocation);
					realIndexBuffer.mData = nullptr;

					//if (realIndexBuffer.mRealDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
					//{
					//	vk::BufferMemoryBarrier uboBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, 0, 0, 0, VK_WHOLE_SIZE);
					//	uboBarrier.buffer = realIndexBuffer.mBuffer;

					//	auto& currentBuffer = realIndexBuffer.mRealDevice->mCommandBuffers[realIndexBuffer.mRealDevice->mCurrentCommandBuffer];
					//	currentBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 1, &uboBarrier, 0, nullptr);
					//}
				}
			}
			break;
			case StateBlock_Create:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CStateBlock9* stateBlock = bit_cast<CStateBlock9*>(workItem->Argument1);

				MergeState(realDevice->mDeviceState, stateBlock->mDeviceState, stateBlock->mType, false);
			}
			break;
			case StateBlock_Capture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CStateBlock9* stateBlock = bit_cast<CStateBlock9*>(workItem->Argument1);

				/*
				Capture only captures the current state of state that has already been recorded (eg update not insert)
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb205890(v=vs.85).aspx
				*/
				MergeState(realDevice->mDeviceState, stateBlock->mDeviceState, stateBlock->mType, true);
			}
			break;
			case StateBlock_Apply:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CStateBlock9* stateBlock = bit_cast<CStateBlock9*>(workItem->Argument1);

				MergeState(stateBlock->mDeviceState, realDevice->mDeviceState, stateBlock->mType, false);

				if (realDevice->mRenderTargets.size())
				{
					auto& renderTarget = realDevice->mRenderTargets[0];
					if (renderTarget != nullptr)
					{
						auto& constants = realDevice->mDeviceState.mShaderState;
						auto surface = renderTarget->mColorSurface;
						if (surface != nullptr)
						{
							auto& extent = surface->mExtent;
							constants.screenWidth = extent.width;
							constants.screenHeight = extent.height;
						}
					}
				}

				if (stateBlock->mType == D3DSBT_ALL)
				{
					realDevice->mDeviceState.mHasTransformsChanged = true;
				}
			}
			break;
			case Texture_GenerateMipSubLevels:
			{
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[workItem->Id]);
				auto& realDevice = texture.mRealDevice;
				CTexture9* texture9 = bit_cast<CTexture9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				vk::Result result;
				vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::CommandBuffer commandBuffer;
				vk::Filter realFilter = ConvertFilter(texture9->mMipFilter);

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::ImageMemoryBarrier imageMemoryBarrier;
				//imageMemoryBarrier.srcAccessMask = 0;
				//imageMemoryBarrier.dstAccessMask = 0;

				vk::ImageSubresourceRange mipSubRange;
				mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				/*
				I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
				*/

				// Transition zero mip level to transfer source
				//mipSubRange.baseMipLevel = 0;

				//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				//imageMemoryBarrier.image = mImage;
				//imageMemoryBarrier.subresourceRange = mipSubRange;
				//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				for (UINT i = 1; i < texture9->mLevels; i++) //Changed to match mLevels datatype
				{
					vk::ImageBlit imageBlit;

					// Source
					imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.srcSubresource.layerCount = 1;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[1].x = int32_t(texture9->mWidth);
					imageBlit.srcOffsets[1].y = int32_t(texture9->mHeight);
					imageBlit.srcOffsets[1].z = 1;

					// Destination
					imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.dstSubresource.layerCount = 1;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(texture9->mWidth >> i);
					imageBlit.dstOffsets[1].y = int32_t(texture9->mHeight >> i);
					imageBlit.dstOffsets[1].z = 1;

					mipSubRange.baseMipLevel = i;

					// Transition current mip level to transfer dest
					imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = texture.mImage;
					imageMemoryBarrier.subresourceRange = mipSubRange;
					commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

					// Blit from zero level
					commandBuffer.blitImage(texture.mImage, vk::ImageLayout::eTransferSrcOptimal, texture.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, vk::Filter::eLinear);
				}

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
				commandBuffer = nullptr;
			}
			break;
			case CubeTexture_GenerateMipSubLevels:
			{
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[workItem->Id]);
				auto& realDevice = texture.mRealDevice;
				CCubeTexture9* texture9 = bit_cast<CCubeTexture9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				vk::Result result;
				vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::CommandBuffer commandBuffer;
				vk::Filter realFilter = ConvertFilter(texture9->mMipFilter);

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::ImageMemoryBarrier imageMemoryBarrier;
				//imageMemoryBarrier.srcAccessMask = 0;
				//imageMemoryBarrier.dstAccessMask = 0;

				vk::ImageSubresourceRange mipSubRange;
				mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				/*
				I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
				*/

				// Transition zero mip level to transfer source
				//mipSubRange.baseMipLevel = 0;

				//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				//imageMemoryBarrier.image = mImage;
				//imageMemoryBarrier.subresourceRange = mipSubRange;
				//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				for (UINT i = 1; i < texture9->mLevels; i++) //Changed to match mLevels datatype
				{
					vk::ImageBlit imageBlit;

					// Source
					imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.srcSubresource.layerCount = 6;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[1].x = int32_t(texture9->mEdgeLength);
					imageBlit.srcOffsets[1].y = int32_t(texture9->mEdgeLength);
					imageBlit.srcOffsets[1].z = 1;

					// Destination
					imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.dstSubresource.layerCount = 6;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(texture9->mEdgeLength >> i);
					imageBlit.dstOffsets[1].y = int32_t(texture9->mEdgeLength >> i);
					imageBlit.dstOffsets[1].z = 1;

					mipSubRange.baseMipLevel = i;

					// Transition current mip level to transfer dest
					imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = texture.mImage;
					imageMemoryBarrier.subresourceRange = mipSubRange;
					commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

					// Blit from zero level
					commandBuffer.blitImage(texture.mImage, vk::ImageLayout::eTransferSrcOptimal, texture.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, vk::Filter::eLinear);
				}

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
				commandBuffer = nullptr;
			}
			break;
			case VolumeTexture_GenerateMipSubLevels:
			{
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[workItem->Id]);
				auto& realDevice = texture.mRealDevice;
				CVolumeTexture9* texture9 = bit_cast<CVolumeTexture9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				vk::Result result;
				vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::CommandBuffer commandBuffer;
				vk::Filter realFilter = ConvertFilter(texture9->mMipFilter);

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::ImageMemoryBarrier imageMemoryBarrier;
				//imageMemoryBarrier.srcAccessMask = 0;
				//imageMemoryBarrier.dstAccessMask = 0;

				vk::ImageSubresourceRange mipSubRange;
				mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				/*
				I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
				*/

				// Transition zero mip level to transfer source
				//mipSubRange.baseMipLevel = 0;

				//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				//imageMemoryBarrier.image = mImage;
				//imageMemoryBarrier.subresourceRange = mipSubRange;
				//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				for (UINT i = 1; i < texture9->mLevels; i++) //Changed to match mLevels datatype
				{
					vk::ImageBlit imageBlit;

					// Source
					imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.srcSubresource.layerCount = 1;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[1].x = int32_t(texture9->mWidth);
					imageBlit.srcOffsets[1].y = int32_t(texture9->mHeight);
					imageBlit.srcOffsets[1].z = 1;

					// Destination
					imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.dstSubresource.layerCount = 1;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(texture9->mWidth >> i);
					imageBlit.dstOffsets[1].y = int32_t(texture9->mHeight >> i);
					imageBlit.dstOffsets[1].z = 1;

					mipSubRange.baseMipLevel = i;

					// Transition current mip level to transfer dest
					imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = texture.mImage;
					imageMemoryBarrier.subresourceRange = mipSubRange;
					commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

					// Blit from zero level
					commandBuffer.blitImage(texture.mImage, vk::ImageLayout::eTransferSrcOptimal, texture.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, vk::Filter::eLinear);
				}

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
				commandBuffer = nullptr;
			}
			break;
			case Surface_LockRect:
			{
				auto& surface = (*commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]);
				auto& realDevice = surface.mRealDevice;
				//CSurface9* surface9 = bit_cast<CSurface9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				D3DLOCKED_RECT* pLockedRect = bit_cast<D3DLOCKED_RECT*>(workItem->Argument1);
				RECT* pRect = bit_cast<RECT*>(workItem->Argument2);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument3);
				//size_t currentId = (size_t)workItem->Argument4;
				//size_t lastId = (size_t)workItem->Argument5;

				//if (currentId != -1 && lastId != -1)
				//{
				//	if (lastId != currentId)
				//	{
				//		auto& oldRealTexture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[lastId]);
				//		auto& newRealTexture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[currentId]);

				//		realDevice->CopyImage(oldRealTexture.mImage, newRealTexture.mImage, newRealTexture.mLevels, newRealTexture.mLayers, newRealTexture.mExtent.width, newRealTexture.mExtent.height, newRealTexture.mExtent.depth);
				//	}
				//}

				vk::Result result;
				char* bytes = nullptr;

				if (surface.mData == nullptr)
				{
					if (surface.mIsFlushed)
					{
						realDevice->SetImageLayout(surface.mStagingImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral); //VK_IMAGE_LAYOUT_PREINITIALIZED			
					}

					result = (vk::Result)vmaMapMemory(realDevice->mAllocator, surface.mImageAllocation, &surface.mData);
					if (result != vk::Result::eSuccess)
					{
						BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkMapMemory failed with return code of " << GetResultString((VkResult)result);
						break;
					}
				}

				bytes = (char*)surface.mData;

				if (surface.mLayouts[0].offset)
				{
					bytes += surface.mLayouts[0].offset;
				}

				std::array<vk::Offset3D, 2> dirtyRect;
				size_t pixelSize = SizeOf(surface.mRealFormat);

				if (pRect != nullptr)
				{
					bytes += (surface.mLayouts[0].rowPitch * pRect->top);
					bytes += (pixelSize * pRect->left);

					dirtyRect[0].x = pRect->left;
					dirtyRect[0].y = pRect->top;
					dirtyRect[0].z = 0;

					dirtyRect[1].x = pRect->right;
					dirtyRect[1].y = pRect->bottom;
					dirtyRect[1].z = 1;
				}
				else
				{
					dirtyRect[0] = vk::Offset3D(0, 0, 0);
					dirtyRect[1] = vk::Offset3D(surface.mExtent.width, surface.mExtent.height, 1);
				}

				pLockedRect->pBits = (void*)bytes;
				pLockedRect->Pitch = surface.mLayouts[0].rowPitch;

				if ((Flags & D3DLOCK_NO_DIRTY_UPDATE) == D3DLOCK_NO_DIRTY_UPDATE)
				{
					//TODO: revisit
				}
				else if ((Flags & D3DLOCK_READONLY) == D3DLOCK_READONLY)
				{
					//TODO: revisit
				}
				else
				{
					surface.mDirtyRects.push_back(dirtyRect);
					surface.mIsFlushed = false;
				}
			}
			break;
			case Surface_UnlockRect:
			{
				auto& surface = (*commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]);
				auto& realDevice = surface.mRealDevice;
				CSurface9* surface9 = bit_cast<CSurface9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				if (surface.mData != nullptr)
				{
					//This work around was replaced with component mapping.
					//if (surface9->mFormat == D3DFMT_X8R8G8B8)
					//{
					//	SetAlpha((char*)surface.mData, surface9->mHeight, surface9->mWidth, surface.mLayouts[0].rowPitch);
					//}
					vmaFlushAllocation(realDevice->mAllocator, surface.mImageAllocation, 0, VK_WHOLE_SIZE);
					vmaUnmapMemory(realDevice->mAllocator, surface.mImageAllocation);
					surface.mData = nullptr;
				}

				surface.mIsFlushed = false;
			}
			break;
			case Surface_Flush:
			{
				auto& surface = (*commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]);

				if (surface.mIsFlushed)
				{
					break;
				}

				auto& realDevice = surface.mRealDevice;
				CSurface9* surface9 = bit_cast<CSurface9*>(workItem->Argument1);
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[surface9->mTextureId]);
				auto& device = realDevice->mDevice;

				vk::CommandBuffer commandBuffer;
				vk::Result result;

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				ReallySetImageLayout(commandBuffer, surface.mStagingImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1); //eGeneral
				ReallySetImageLayout(commandBuffer, texture.mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, surface9->mMipIndex, surface9->mTargetLayer + 1);

				vk::ImageSubresourceLayers subResource1;
				subResource1.aspectMask = vk::ImageAspectFlagBits::eColor;
				subResource1.baseArrayLayer = 0;
				subResource1.mipLevel = 0;
				subResource1.layerCount = 1;

				vk::ImageSubresourceLayers subResource2;
				subResource2.aspectMask = vk::ImageAspectFlagBits::eColor;
				subResource2.baseArrayLayer = surface9->mTargetLayer;
				subResource2.mipLevel = surface9->mMipIndex;
				subResource2.layerCount = 1;

				vk::ImageBlit region;
				region.srcSubresource = subResource1;
				region.dstSubresource = subResource2;

				if (surface.mDirtyRects.size())
				{
					std::vector<vk::ImageBlit> regions;
					for (auto& dirtyRect : surface.mDirtyRects)
					{
						region.srcOffsets[0] = dirtyRect[0];
						region.srcOffsets[1] = dirtyRect[1];
						region.dstOffsets[0] = dirtyRect[0];
						region.dstOffsets[1] = dirtyRect[1];

						regions.push_back(region);

						commandBuffer.blitImage(
							surface.mStagingImage, vk::ImageLayout::eTransferSrcOptimal,
							texture.mImage, vk::ImageLayout::eTransferDstOptimal,
							1, &regions[regions.size() - 1], vk::Filter::eLinear);
					}
					surface.mDirtyRects.clear();
				}
				else
				{
					ReallyCopyImage(commandBuffer, surface.mStagingImage, texture.mImage, 0, 0, surface9->mWidth, surface9->mHeight, 1, 0, surface9->mMipIndex, 0, surface9->mTargetLayer);
				}

				ReallySetImageLayout(commandBuffer, texture.mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral, 1, surface9->mMipIndex, surface9->mTargetLayer + 1);

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);

				surface.mIsFlushed = true;
			}
			break;
			case Volume_LockRect:
			{
				auto& volume = (*commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]);
				auto& realDevice = volume.mRealDevice;
				auto& device = realDevice->mDevice;

				D3DLOCKED_BOX* pLockedVolume = bit_cast<D3DLOCKED_BOX*>(workItem->Argument1);
				D3DBOX* pBox = bit_cast<D3DBOX*>(workItem->Argument2);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument3);

				vk::Result result;
				char* bytes = nullptr;

				if (volume.mData == nullptr)
				{
					if (volume.mIsFlushed)
					{
						realDevice->SetImageLayout(volume.mStagingImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral); //VK_IMAGE_LAYOUT_PREINITIALIZED			
					}

					//result = device.mapMemory((vk::DeviceMemory)volume.mImageAllocationInfo.deviceMemory, 0, (vk::DeviceSize)volume.mImageAllocationInfo.size, vk::MemoryMapFlags(), &volume.mData);
					result = (vk::Result)vmaMapMemory(realDevice->mAllocator, volume.mImageAllocation, &volume.mData);
					if (result != vk::Result::eSuccess)
					{
						BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkMapMemory failed with return code of " << GetResultString((VkResult)result);
						break;
					}
				}

				bytes = (char*)volume.mData;

				if (volume.mLayouts[0].offset)
				{
					bytes += volume.mLayouts[0].offset;
				}

				std::array<vk::Offset3D, 2> dirtyRect;
				size_t pixelSize = SizeOf(volume.mRealFormat);

				if (pBox != nullptr)
				{
					bytes += (volume.mLayouts[0].rowPitch * pBox->Top);
					bytes += (volume.mLayouts[0].depthPitch * pBox->Front);
					bytes += (pixelSize * pBox->Left);

					dirtyRect[0].x = pBox->Left;
					dirtyRect[0].y = pBox->Top;
					dirtyRect[0].z = pBox->Front;

					dirtyRect[1].x = pBox->Right;
					dirtyRect[1].y = pBox->Bottom;
					dirtyRect[1].z = pBox->Back;
				}
				else
				{
					dirtyRect[0] = vk::Offset3D(0, 0, 0);
					dirtyRect[1] = vk::Offset3D(volume.mExtent.width, volume.mExtent.height, volume.mExtent.depth);
				}


				pLockedVolume->pBits = (void*)bytes;
				pLockedVolume->RowPitch = volume.mLayouts[0].rowPitch;
				pLockedVolume->SlicePitch = volume.mLayouts[0].depthPitch;

				if ((Flags & D3DLOCK_NO_DIRTY_UPDATE) == D3DLOCK_NO_DIRTY_UPDATE)
				{
					//TODO: revisit
				}
				else if ((Flags & D3DLOCK_READONLY) == D3DLOCK_READONLY)
				{
					//TODO: revisit
				}
				else
				{
					volume.mDirtyRects.push_back(dirtyRect);
					volume.mIsFlushed = false;
				}
			}
			break;
			case Volume_UnlockRect:
			{
				auto& volume = (*commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]);
				auto& realDevice = volume.mRealDevice;
				CVolume9* volume9 = bit_cast<CVolume9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				if (volume.mData != nullptr)
				{
					//This work around was replaced with component mapping.
					//if (Volume9->mFormat == D3DFMT_X8R8G8B8)
					//{
					//	SetAlpha((char*)Volume.mData, Volume9->mHeight, Volume9->mWidth, Volume.mLayouts[0].rowPitch);
					//}
					vmaFlushAllocation(realDevice->mAllocator, volume.mImageAllocation, 0, VK_WHOLE_SIZE);
					vmaUnmapMemory(realDevice->mAllocator, volume.mImageAllocation);
					volume.mData = nullptr;
				}

				volume.mIsFlushed = false;
			}
			break;
			case Volume_Flush:
			{
				auto& volume = (*commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]);

				if (volume.mIsFlushed)
				{
					break;
				}

				auto& realDevice = volume.mRealDevice;
				CVolume9* volume9 = bit_cast<CVolume9*>(workItem->Argument1);
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[volume9->mTextureId]);
				auto& device = realDevice->mDevice;

				vk::CommandBuffer commandBuffer;
				vk::Result result;

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				ReallySetImageLayout(commandBuffer, volume.mStagingImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, 1, 0, 1); //eGeneral
				ReallySetImageLayout(commandBuffer, texture.mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, volume9->mMipIndex, volume9->mTargetLayer + 1);

				vk::ImageSubresourceLayers subResource1;
				subResource1.aspectMask = vk::ImageAspectFlagBits::eColor;
				subResource1.baseArrayLayer = 0;
				subResource1.mipLevel = 0;
				subResource1.layerCount = 1;

				vk::ImageSubresourceLayers subResource2;
				subResource2.aspectMask = vk::ImageAspectFlagBits::eColor;
				subResource2.baseArrayLayer = volume9->mTargetLayer;
				subResource2.mipLevel = volume9->mMipIndex;
				subResource2.layerCount = 1;

				vk::ImageBlit region;
				region.srcSubresource = subResource1;
				region.dstSubresource = subResource2;

				if (volume.mDirtyRects.size())
				{
					std::vector<vk::ImageBlit> regions;
					for (auto& dirtyRect : volume.mDirtyRects)
					{
						region.srcOffsets[0] = dirtyRect[0];
						region.srcOffsets[1] = dirtyRect[1];
						region.dstOffsets[0] = dirtyRect[0];
						region.dstOffsets[1] = dirtyRect[1];

						regions.push_back(region);

						commandBuffer.blitImage(
							volume.mStagingImage, vk::ImageLayout::eTransferSrcOptimal,
							texture.mImage, vk::ImageLayout::eTransferDstOptimal,
							1, &regions[regions.size() - 1], vk::Filter::eLinear);
					}
					volume.mDirtyRects.clear();
				}
				else
				{
					ReallyCopyImage(commandBuffer, volume.mStagingImage, texture.mImage, 0, 0, volume9->mWidth, volume9->mHeight, volume9->mDepth, 0, volume9->mMipIndex, 0, volume9->mTargetLayer);
				}

				ReallySetImageLayout(commandBuffer, texture.mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral, 1, volume9->mMipIndex, volume9->mTargetLayer + 1);

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);

				volume.mIsFlushed = true;
			}
			break;
			case Query_Issue:
			{
				auto& realQuery = (*commandStreamManager->mRenderManager.mStateManager.mQueries[workItem->Id]);
				auto& realDevice = realQuery.mRealDevice;

				DWORD dwIssueFlags = bit_cast<DWORD>(workItem->Argument1);

				switch (dwIssueFlags)
				{
				case D3DISSUE_BEGIN:
					realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer].beginQuery(realQuery.mQueryPool, 0, vk::QueryControlFlags());
					break;
				case D3DISSUE_END:
					realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer].endQuery(realQuery.mQueryPool, 0);
					break;
				default:
					BOOST_LOG_TRIVIAL(error) << "ProcessQueue unknown query issue type " << dwIssueFlags;
					break;
				}

				//(*pViewport) = realDevice.mDeviceState.m9Viewport;
			}
			break;
			case Query_GetData:
			{
				auto& realQuery = (*commandStreamManager->mRenderManager.mStateManager.mQueries[workItem->Id]);
				auto& realDevice = realQuery.mRealDevice;

				void* pData = workItem->Argument1;
				DWORD dwSize = bit_cast<DWORD>(workItem->Argument2);
				DWORD dwGetDataFlags = bit_cast<DWORD>(workItem->Argument3);

				realDevice->mDevice.getQueryPoolResults(realQuery.mQueryPool, (uint32_t)0, (uint32_t)1, (size_t)dwSize, pData, (vk::DeviceSize)4, vk::QueryResultFlags());
			}
			break;
			case Query_GetDataSize:
			{
				BOOST_LOG_TRIVIAL(warning) << "ProcessQueue assuming 4 byte query result size.";
			}
			break;
			default:
			{
				BOOST_LOG_TRIVIAL(error) << "ProcessQueue unknown work item " << workItem->WorkItemType;
			}
			break;
			}
			//}
			//catch (const std::exception& ex)
			//{
			//	BOOST_LOG_TRIVIAL(warning) << "ProcessQueue - " << workItem->WorkItemType << " " << ex.what();
			//}
			//catch (...)
			//{
			//	BOOST_LOG_TRIVIAL(warning) << "ProcessQueue - " << workItem->WorkItemType;
			//}

			workItem->HasBeenProcessed = true;
			workItem->ConditionVariable.notify_all();

			if (workItem->Caller != nullptr)
			{
				workItem->Caller->Release();
				workItem->Caller = nullptr;
			}

			if (!commandStreamManager->mUnusedWorkItems.Push(workItem, count))
			{
				delete workItem;
			}
		}
	}
}