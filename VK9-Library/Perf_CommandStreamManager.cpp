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