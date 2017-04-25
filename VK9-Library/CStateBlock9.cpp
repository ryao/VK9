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
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "CStateBlock9.h"
#include "CDevice9.h"
#include "CTexture9.h"

#include "Utilities.h"

CStateBlock9::CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type)
	: mDevice(device),
	mType(Type)
{
	//BOOST_LOG_TRIVIAL(info) << "CStateBlock9::CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type)";
	
	//for (int32_t i = 0; i < 16; i++)
	//{
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_BORDERCOLOR] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MAGFILTER] = D3DTEXF_POINT;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MINFILTER] = D3DTEXF_POINT;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MIPMAPLODBIAS] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MAXMIPLEVEL] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MAXANISOTROPY] = 1;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_SRGBTEXTURE] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ELEMENTINDEX] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_DMAPOFFSET] = 0;
	//}

	MergeState(this->mDevice->mDeviceState, mDeviceState, mType, false);
}

CStateBlock9::CStateBlock9(CDevice9* device)
	: mDevice(device)
{
	//BOOST_LOG_TRIVIAL(info) << "CStateBlock9::CStateBlock9(CDevice9* device)";

	//for (int32_t i = 0; i < 16; i++)
	//{
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_BORDERCOLOR] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MAGFILTER] = D3DTEXF_POINT;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MINFILTER] = D3DTEXF_POINT;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MIPMAPLODBIAS] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MAXMIPLEVEL] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_MAXANISOTROPY] = 1;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_SRGBTEXTURE] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_ELEMENTINDEX] = 0;
	//	mDeviceState.mSamplerStates[i][D3DSAMP_DMAPOFFSET] = 0;
	//}
}

CStateBlock9::~CStateBlock9()
{

}

ULONG STDMETHODCALLTYPE CStateBlock9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CStateBlock9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DStateBlock9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CStateBlock9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Capture()
{
	/*
	Capture only captures the current state of state that has already been recorded (eg update not insert)
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb205890(v=vs.85).aspx
	*/
	MergeState(this->mDevice->mDeviceState, mDeviceState, mType, true);

	//BOOST_LOG_TRIVIAL(info) << "CStateBlock9::Capture " << this;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Apply()
{
	MergeState(mDeviceState, this->mDevice->mDeviceState,mType);	
	
	//if (mDeviceState.mTransforms.size())
	//{
		if (mType == D3DSBT_ALL)
		{
			this->mDevice->mDeviceState.mHasTransformsChanged = true;
		}
	//}

	//BOOST_LOG_TRIVIAL(info) << "CStateBlock9::Apply " << this;

	//Print(this->mDevice->mDeviceState);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

void MergeState(const DeviceState& sourceState, DeviceState& targetState, D3DSTATEBLOCKTYPE type, BOOL onlyIfExists)
{
	/*
	Pixel https://msdn.microsoft.com/en-us/library/windows/desktop/bb147351(v=vs.85).aspx
	Vertex https://msdn.microsoft.com/en-us/library/windows/desktop/bb147353(v=vs.85).aspx#Vertex_Pipeline_Render_State
	All https://msdn.microsoft.com/en-us/library/windows/desktop/bb147350(v=vs.85).aspx
	*/

	//IDirect3DDevice9::LightEnable
	if ((type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE))
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mLightSettings)
		{
			if (!onlyIfExists || targetState.mLightSettings.count(pair1.first) > 0)
			{
				targetState.mLightSettings[pair1.first] = pair1.second;
			}
		}
	}

	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette

	//IDirect3DDevice9::SetVertexDeclaration
	//IDirect3DDevice9::SetFVF
	if ((sourceState.mHasVertexDeclaration || sourceState.mHasFVF) && (!onlyIfExists || targetState.mHasFVF || targetState.mHasVertexDeclaration) && (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE))
	{
		targetState.mFVF = sourceState.mFVF;
		targetState.mHasFVF = sourceState.mHasFVF;

		targetState.mVertexDeclaration = sourceState.mVertexDeclaration;
		targetState.mHasVertexDeclaration = sourceState.mHasVertexDeclaration;		
	}

	//IDirect3DDevice9::SetIndices
	if (sourceState.mHasIndexBuffer && (!onlyIfExists || targetState.mHasIndexBuffer) && (type == D3DSBT_ALL))
	{
		targetState.mIndexBuffer = sourceState.mIndexBuffer;
		targetState.mHasIndexBuffer = true;
	}
	
	if ((type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE))
	{
		//IDirect3DDevice9::SetLight
		for (size_t i = 0; i < sourceState.mLights.size(); i++)
		{
			if (i < targetState.mLights.size())
			{
				targetState.mLights[i] = sourceState.mLights[i];
			}
			else if (!onlyIfExists)
			{
				targetState.mLights.push_back(sourceState.mLights[i]);
			}
		}
		targetState.mAreLightsDirty = true;

		//IDirect3DDevice9::SetMaterial
		targetState.mMaterial = sourceState.mMaterial;
		targetState.mIsMaterialDirty = true;
	}

	//IDirect3DDevice9::SetNPatchMode
	if (sourceState.mNSegments != -1 && (!onlyIfExists || targetState.mNSegments != -1) && (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE))
	{
		targetState.mNSegments = sourceState.mNSegments; //Doesn't matter anyway.
	}

	//IDirect3DDevice9::SetPixelShader
	if (sourceState.mHasPixelShader && (!onlyIfExists || targetState.mHasPixelShader) && (type == D3DSBT_ALL || type == D3DSBT_PIXELSTATE))
	{
		//if (targetState.mPixelShader != nullptr)
		//{
		//	targetState.mPixelShader->Release();
		//}

		//TODO: may leak
		targetState.mPixelShader = sourceState.mPixelShader;
		targetState.mHasPixelShader = true;
	}

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	if (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE)
	{
		targetState.mSpecializationConstants.cullMode = sourceState.mSpecializationConstants.cullMode;
		targetState.mSpecializationConstants.fogColor = sourceState.mSpecializationConstants.fogColor;
		targetState.mSpecializationConstants.fogTableMode = sourceState.mSpecializationConstants.fogTableMode;
		targetState.mSpecializationConstants.fogStart = sourceState.mSpecializationConstants.fogStart;
		targetState.mSpecializationConstants.fogEnd = sourceState.mSpecializationConstants.fogEnd;
		targetState.mSpecializationConstants.fogDensity = sourceState.mSpecializationConstants.fogDensity;
		targetState.mSpecializationConstants.rangeFogEnable = sourceState.mSpecializationConstants.rangeFogEnable;
		targetState.mSpecializationConstants.ambient = sourceState.mSpecializationConstants.ambient;
		targetState.mSpecializationConstants.colorVertex = sourceState.mSpecializationConstants.colorVertex;
		targetState.mSpecializationConstants.fogVertexMode = sourceState.mSpecializationConstants.fogVertexMode;
		targetState.mSpecializationConstants.clipping = sourceState.mSpecializationConstants.clipping;
		targetState.mSpecializationConstants.lighting = sourceState.mSpecializationConstants.lighting;
		targetState.mSpecializationConstants.localViewer = sourceState.mSpecializationConstants.localViewer;
		targetState.mSpecializationConstants.emissiveMaterialSource = sourceState.mSpecializationConstants.emissiveMaterialSource;
		targetState.mSpecializationConstants.ambientMaterialSource = sourceState.mSpecializationConstants.ambientMaterialSource;
		targetState.mSpecializationConstants.diffuseMaterialSource = sourceState.mSpecializationConstants.diffuseMaterialSource;
		targetState.mSpecializationConstants.specularMaterialSource = sourceState.mSpecializationConstants.specularMaterialSource;
		targetState.mSpecializationConstants.vertexBlend = sourceState.mSpecializationConstants.vertexBlend;
		targetState.mSpecializationConstants.clipPlaneEnable = sourceState.mSpecializationConstants.clipPlaneEnable;
		targetState.mSpecializationConstants.pointSize = sourceState.mSpecializationConstants.pointSize;
		targetState.mSpecializationConstants.pointSizeMinimum = sourceState.mSpecializationConstants.pointSizeMinimum;
		targetState.mSpecializationConstants.pointSpriteEnable = sourceState.mSpecializationConstants.pointSpriteEnable;
		targetState.mSpecializationConstants.pointScaleEnable = sourceState.mSpecializationConstants.pointScaleEnable;
		targetState.mSpecializationConstants.pointScaleA = sourceState.mSpecializationConstants.pointScaleA;
		targetState.mSpecializationConstants.pointScaleB = sourceState.mSpecializationConstants.pointScaleB;
		targetState.mSpecializationConstants.pointScaleC = sourceState.mSpecializationConstants.pointScaleC;
		targetState.mSpecializationConstants.multisampleAntiAlias = sourceState.mSpecializationConstants.multisampleAntiAlias;
		targetState.mSpecializationConstants.multisampleMask = sourceState.mSpecializationConstants.multisampleMask;
		targetState.mSpecializationConstants.patchEdgeStyle = sourceState.mSpecializationConstants.patchEdgeStyle;
		targetState.mSpecializationConstants.pointSizeMaximum = sourceState.mSpecializationConstants.pointSizeMaximum;
		targetState.mSpecializationConstants.indexedVertexBlendEnable = sourceState.mSpecializationConstants.indexedVertexBlendEnable;
		targetState.mSpecializationConstants.tweenFactor = sourceState.mSpecializationConstants.tweenFactor;
		targetState.mSpecializationConstants.positionDegree = sourceState.mSpecializationConstants.positionDegree;
		targetState.mSpecializationConstants.normalDegree = sourceState.mSpecializationConstants.normalDegree;
		targetState.mSpecializationConstants.minimumTessellationLevel = sourceState.mSpecializationConstants.minimumTessellationLevel;
		targetState.mSpecializationConstants.maximumTessellationLevel = sourceState.mSpecializationConstants.maximumTessellationLevel;
		targetState.mSpecializationConstants.adaptivetessX = sourceState.mSpecializationConstants.adaptivetessX;
		targetState.mSpecializationConstants.adaptivetessY = sourceState.mSpecializationConstants.adaptivetessY;
		targetState.mSpecializationConstants.adaptivetessZ = sourceState.mSpecializationConstants.adaptivetessZ;
		targetState.mSpecializationConstants.adaptivetessW = sourceState.mSpecializationConstants.adaptivetessW;
		targetState.mSpecializationConstants.enableAdaptiveTessellation = sourceState.mSpecializationConstants.enableAdaptiveTessellation;

	}

	if (type == D3DSBT_ALL || type == D3DSBT_PIXELSTATE)
	{
		targetState.mSpecializationConstants.zEnable = sourceState.mSpecializationConstants.zEnable;
		targetState.mSpecializationConstants.specularEnable = sourceState.mSpecializationConstants.specularEnable;
		targetState.mSpecializationConstants.fillMode = sourceState.mSpecializationConstants.fillMode;
		targetState.mSpecializationConstants.shadeMode = sourceState.mSpecializationConstants.shadeMode;
		targetState.mSpecializationConstants.zWriteEnable = sourceState.mSpecializationConstants.zWriteEnable;
		targetState.mSpecializationConstants.alphaTestEnable = sourceState.mSpecializationConstants.alphaTestEnable;
		targetState.mSpecializationConstants.lastPixel = sourceState.mSpecializationConstants.lastPixel;
		targetState.mSpecializationConstants.sourceBlend = sourceState.mSpecializationConstants.sourceBlend;
		targetState.mSpecializationConstants.destinationBlend = sourceState.mSpecializationConstants.destinationBlend;
		targetState.mSpecializationConstants.zFunction = sourceState.mSpecializationConstants.zFunction;
		targetState.mSpecializationConstants.alphaReference = sourceState.mSpecializationConstants.alphaReference;
		targetState.mSpecializationConstants.alphaFunction = sourceState.mSpecializationConstants.alphaFunction;
		targetState.mSpecializationConstants.ditherEnable = sourceState.mSpecializationConstants.ditherEnable;
		targetState.mSpecializationConstants.fogStart = sourceState.mSpecializationConstants.fogStart;
		targetState.mSpecializationConstants.fogEnd = sourceState.mSpecializationConstants.fogEnd;
		targetState.mSpecializationConstants.fogDensity = sourceState.mSpecializationConstants.fogDensity;
		targetState.mSpecializationConstants.alphaBlendEnable = sourceState.mSpecializationConstants.alphaBlendEnable;
		targetState.mSpecializationConstants.depthBias = sourceState.mSpecializationConstants.depthBias;
		targetState.mSpecializationConstants.stencilEnable = sourceState.mSpecializationConstants.stencilEnable;
		targetState.mSpecializationConstants.stencilFail = sourceState.mSpecializationConstants.stencilFail;
		targetState.mSpecializationConstants.stencilZFail = sourceState.mSpecializationConstants.stencilZFail;
		targetState.mSpecializationConstants.stencilPass = sourceState.mSpecializationConstants.stencilPass;
		targetState.mSpecializationConstants.stencilFunction = sourceState.mSpecializationConstants.stencilFunction;
		targetState.mSpecializationConstants.stencilReference = sourceState.mSpecializationConstants.stencilReference;
		targetState.mSpecializationConstants.stencilMask = sourceState.mSpecializationConstants.stencilMask;
		targetState.mSpecializationConstants.stencilWriteMask = sourceState.mSpecializationConstants.stencilWriteMask;
		targetState.mSpecializationConstants.textureFactor = sourceState.mSpecializationConstants.textureFactor;
		targetState.mSpecializationConstants.wrap0 = sourceState.mSpecializationConstants.wrap0;
		targetState.mSpecializationConstants.wrap1 = sourceState.mSpecializationConstants.wrap1;
		targetState.mSpecializationConstants.wrap2 = sourceState.mSpecializationConstants.wrap2;
		targetState.mSpecializationConstants.wrap3 = sourceState.mSpecializationConstants.wrap3;
		targetState.mSpecializationConstants.wrap4 = sourceState.mSpecializationConstants.wrap4;
		targetState.mSpecializationConstants.wrap5 = sourceState.mSpecializationConstants.wrap5;
		targetState.mSpecializationConstants.wrap6 = sourceState.mSpecializationConstants.wrap6;
		targetState.mSpecializationConstants.wrap7 = sourceState.mSpecializationConstants.wrap7;
		targetState.mSpecializationConstants.wrap8 = sourceState.mSpecializationConstants.wrap8;
		targetState.mSpecializationConstants.wrap9 = sourceState.mSpecializationConstants.wrap9;
		targetState.mSpecializationConstants.wrap10 = sourceState.mSpecializationConstants.wrap10;
		targetState.mSpecializationConstants.wrap11 = sourceState.mSpecializationConstants.wrap11;
		targetState.mSpecializationConstants.wrap12 = sourceState.mSpecializationConstants.wrap12;
		targetState.mSpecializationConstants.wrap13 = sourceState.mSpecializationConstants.wrap13;
		targetState.mSpecializationConstants.wrap14 = sourceState.mSpecializationConstants.wrap14;
		targetState.mSpecializationConstants.wrap15 = sourceState.mSpecializationConstants.wrap15;
		targetState.mSpecializationConstants.localViewer = sourceState.mSpecializationConstants.localViewer;
		targetState.mSpecializationConstants.emissiveMaterialSource = sourceState.mSpecializationConstants.emissiveMaterialSource;
		targetState.mSpecializationConstants.ambientMaterialSource = sourceState.mSpecializationConstants.ambientMaterialSource;
		targetState.mSpecializationConstants.diffuseMaterialSource = sourceState.mSpecializationConstants.diffuseMaterialSource;
		targetState.mSpecializationConstants.specularMaterialSource = sourceState.mSpecializationConstants.specularMaterialSource;
		targetState.mSpecializationConstants.colorWriteEnable = sourceState.mSpecializationConstants.colorWriteEnable;
		targetState.mSpecializationConstants.blendOperation = sourceState.mSpecializationConstants.blendOperation;
		targetState.mSpecializationConstants.scissorTestEnable = sourceState.mSpecializationConstants.scissorTestEnable;
		targetState.mSpecializationConstants.slopeScaleDepthBias = sourceState.mSpecializationConstants.slopeScaleDepthBias;
		targetState.mSpecializationConstants.antiAliasedLineEnable = sourceState.mSpecializationConstants.antiAliasedLineEnable;
		targetState.mSpecializationConstants.twoSidedStencilMode = sourceState.mSpecializationConstants.twoSidedStencilMode;
		targetState.mSpecializationConstants.ccwStencilFail = sourceState.mSpecializationConstants.ccwStencilFail;
		targetState.mSpecializationConstants.ccwStencilZFail = sourceState.mSpecializationConstants.ccwStencilZFail;
		targetState.mSpecializationConstants.ccwStencilPass = sourceState.mSpecializationConstants.ccwStencilPass;
		targetState.mSpecializationConstants.ccwStencilFunction = sourceState.mSpecializationConstants.ccwStencilFunction;
		targetState.mSpecializationConstants.colorWriteEnable1 = sourceState.mSpecializationConstants.colorWriteEnable1;
		targetState.mSpecializationConstants.colorWriteEnable2 = sourceState.mSpecializationConstants.colorWriteEnable2;
		targetState.mSpecializationConstants.colorWriteEnable3 = sourceState.mSpecializationConstants.colorWriteEnable3;
		targetState.mSpecializationConstants.blendFactor = sourceState.mSpecializationConstants.blendFactor;
		targetState.mSpecializationConstants.srgbWriteEnable = sourceState.mSpecializationConstants.srgbWriteEnable;
		targetState.mSpecializationConstants.separateAlphaBlendEnable = sourceState.mSpecializationConstants.separateAlphaBlendEnable;
		targetState.mSpecializationConstants.sourceBlendAlpha = sourceState.mSpecializationConstants.sourceBlendAlpha;
		targetState.mSpecializationConstants.destinationBlendAlpha = sourceState.mSpecializationConstants.destinationBlendAlpha;
		targetState.mSpecializationConstants.blendOperationAlpha = sourceState.mSpecializationConstants.blendOperationAlpha;
	}

	//IDirect3DDevice9::SetSamplerState
	if (sourceState.mSamplerStates.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mSamplerStates)
		{
			BOOST_FOREACH(const auto& pair2, pair1.second)
			{
				if (!onlyIfExists || (targetState.mSamplerStates.count(pair1.first) > 0 && targetState.mSamplerStates[pair1.first].count(pair2.first) > 0))
				{
					if
						(
						(type == D3DSBT_ALL) ||
							(type == D3DSBT_VERTEXSTATE &&
							(
								pair2.first == D3DSAMP_DMAPOFFSET

								)) ||
								(type == D3DSBT_PIXELSTATE &&
							(
								pair2.first == D3DSAMP_ADDRESSU ||
								pair2.first == D3DSAMP_ADDRESSV ||
								pair2.first == D3DSAMP_ADDRESSW ||
								pair2.first == D3DSAMP_BORDERCOLOR ||
								pair2.first == D3DSAMP_MAGFILTER ||
								pair2.first == D3DSAMP_MINFILTER ||
								pair2.first == D3DSAMP_MIPFILTER ||
								pair2.first == D3DSAMP_MIPMAPLODBIAS ||
								pair2.first == D3DSAMP_MAXMIPLEVEL ||
								pair2.first == D3DSAMP_MAXANISOTROPY ||
								pair2.first == D3DSAMP_SRGBTEXTURE ||
								pair2.first == D3DSAMP_ELEMENTINDEX
								))
							)
					{
						targetState.mSamplerStates[pair1.first][pair2.first] = pair2.second;
					}
				}
			}
		}
	}
	//targetState.mSamplerStates = sourceState.mSamplerStates;

	//IDirect3DDevice9::SetScissorRect
	if ((sourceState.m9Scissor.right != 0 || sourceState.m9Scissor.left != 0) && (!onlyIfExists || targetState.mHasIndexBuffer) && (type == D3DSBT_ALL))
	{
		targetState.m9Scissor = sourceState.m9Scissor;
		targetState.mScissor = sourceState.mScissor;
	}

	//IDirect3DDevice9::SetStreamSource
	if (sourceState.mStreamSources.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mStreamSources)
		{
			if (type == D3DSBT_ALL && (!onlyIfExists || targetState.mStreamSources.count(pair1.first) > 0))
			{
				targetState.mStreamSources[pair1.first] = pair1.second;
			}
		}
	}

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	BOOST_FOREACH(const auto& pair1, sourceState.mTextures)
	{
		if ((type == D3DSBT_ALL))
		{
			targetState.mTextures[pair1.first] = pair1.second;
		}
	}

	//IDirect3DDevice9::SetTextureStageState
	if (type == D3DSBT_VERTEXSTATE || type == D3DSBT_ALL)
	{
		targetState.mSpecializationConstants.texureCoordinateIndex_0 = sourceState.mSpecializationConstants.texureCoordinateIndex_0;
		targetState.mSpecializationConstants.texureCoordinateIndex_1 = sourceState.mSpecializationConstants.texureCoordinateIndex_1;
		targetState.mSpecializationConstants.texureCoordinateIndex_2 = sourceState.mSpecializationConstants.texureCoordinateIndex_2;
		targetState.mSpecializationConstants.texureCoordinateIndex_3 = sourceState.mSpecializationConstants.texureCoordinateIndex_3;
		targetState.mSpecializationConstants.texureCoordinateIndex_4 = sourceState.mSpecializationConstants.texureCoordinateIndex_4;
		targetState.mSpecializationConstants.texureCoordinateIndex_5 = sourceState.mSpecializationConstants.texureCoordinateIndex_5;
		targetState.mSpecializationConstants.texureCoordinateIndex_6 = sourceState.mSpecializationConstants.texureCoordinateIndex_6;
		targetState.mSpecializationConstants.texureCoordinateIndex_7 = sourceState.mSpecializationConstants.texureCoordinateIndex_7;

		targetState.mSpecializationConstants.textureTransformationFlags_0 = sourceState.mSpecializationConstants.textureTransformationFlags_0;
		targetState.mSpecializationConstants.textureTransformationFlags_1 = sourceState.mSpecializationConstants.textureTransformationFlags_1;
		targetState.mSpecializationConstants.textureTransformationFlags_2 = sourceState.mSpecializationConstants.textureTransformationFlags_2;
		targetState.mSpecializationConstants.textureTransformationFlags_3 = sourceState.mSpecializationConstants.textureTransformationFlags_3;
		targetState.mSpecializationConstants.textureTransformationFlags_4 = sourceState.mSpecializationConstants.textureTransformationFlags_4;
		targetState.mSpecializationConstants.textureTransformationFlags_5 = sourceState.mSpecializationConstants.textureTransformationFlags_5;
		targetState.mSpecializationConstants.textureTransformationFlags_6 = sourceState.mSpecializationConstants.textureTransformationFlags_6;
		targetState.mSpecializationConstants.textureTransformationFlags_7 = sourceState.mSpecializationConstants.textureTransformationFlags_7;
	}

	if (type == D3DSBT_PIXELSTATE || type == D3DSBT_ALL)
	{
		targetState.mSpecializationConstants.colorOperation_0 = sourceState.mSpecializationConstants.colorOperation_0;
		targetState.mSpecializationConstants.colorOperation_1 = sourceState.mSpecializationConstants.colorOperation_1;
		targetState.mSpecializationConstants.colorOperation_2 = sourceState.mSpecializationConstants.colorOperation_2;
		targetState.mSpecializationConstants.colorOperation_3 = sourceState.mSpecializationConstants.colorOperation_3;
		targetState.mSpecializationConstants.colorOperation_4 = sourceState.mSpecializationConstants.colorOperation_4;
		targetState.mSpecializationConstants.colorOperation_5 = sourceState.mSpecializationConstants.colorOperation_5;
		targetState.mSpecializationConstants.colorOperation_6 = sourceState.mSpecializationConstants.colorOperation_6;
		targetState.mSpecializationConstants.colorOperation_7 = sourceState.mSpecializationConstants.colorOperation_7;

		targetState.mSpecializationConstants.colorArgument1_0 = sourceState.mSpecializationConstants.colorArgument1_0;
		targetState.mSpecializationConstants.colorArgument1_1 = sourceState.mSpecializationConstants.colorArgument1_1;
		targetState.mSpecializationConstants.colorArgument1_2 = sourceState.mSpecializationConstants.colorArgument1_2;
		targetState.mSpecializationConstants.colorArgument1_3 = sourceState.mSpecializationConstants.colorArgument1_3;
		targetState.mSpecializationConstants.colorArgument1_4 = sourceState.mSpecializationConstants.colorArgument1_4;
		targetState.mSpecializationConstants.colorArgument1_5 = sourceState.mSpecializationConstants.colorArgument1_5;
		targetState.mSpecializationConstants.colorArgument1_6 = sourceState.mSpecializationConstants.colorArgument1_6;
		targetState.mSpecializationConstants.colorArgument1_7 = sourceState.mSpecializationConstants.colorArgument1_7;

		targetState.mSpecializationConstants.colorArgument2_0 = sourceState.mSpecializationConstants.colorArgument2_0;
		targetState.mSpecializationConstants.colorArgument2_1 = sourceState.mSpecializationConstants.colorArgument2_1;
		targetState.mSpecializationConstants.colorArgument2_2 = sourceState.mSpecializationConstants.colorArgument2_2;
		targetState.mSpecializationConstants.colorArgument2_3 = sourceState.mSpecializationConstants.colorArgument2_3;
		targetState.mSpecializationConstants.colorArgument2_4 = sourceState.mSpecializationConstants.colorArgument2_4;
		targetState.mSpecializationConstants.colorArgument2_5 = sourceState.mSpecializationConstants.colorArgument2_5;
		targetState.mSpecializationConstants.colorArgument2_6 = sourceState.mSpecializationConstants.colorArgument2_6;
		targetState.mSpecializationConstants.colorArgument2_7 = sourceState.mSpecializationConstants.colorArgument2_7;

		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;

		targetState.mSpecializationConstants.alphaOperation_0 = sourceState.mSpecializationConstants.alphaOperation_0;
		targetState.mSpecializationConstants.alphaOperation_1 = sourceState.mSpecializationConstants.alphaOperation_1;
		targetState.mSpecializationConstants.alphaOperation_2 = sourceState.mSpecializationConstants.alphaOperation_2;
		targetState.mSpecializationConstants.alphaOperation_3 = sourceState.mSpecializationConstants.alphaOperation_3;
		targetState.mSpecializationConstants.alphaOperation_4 = sourceState.mSpecializationConstants.alphaOperation_4;
		targetState.mSpecializationConstants.alphaOperation_5 = sourceState.mSpecializationConstants.alphaOperation_5;
		targetState.mSpecializationConstants.alphaOperation_6 = sourceState.mSpecializationConstants.alphaOperation_6;
		targetState.mSpecializationConstants.alphaOperation_7 = sourceState.mSpecializationConstants.alphaOperation_7;

		targetState.mSpecializationConstants.alphaArgument1_0 = sourceState.mSpecializationConstants.alphaArgument1_0;
		targetState.mSpecializationConstants.alphaArgument1_1 = sourceState.mSpecializationConstants.alphaArgument1_1;
		targetState.mSpecializationConstants.alphaArgument1_2 = sourceState.mSpecializationConstants.alphaArgument1_2;
		targetState.mSpecializationConstants.alphaArgument1_3 = sourceState.mSpecializationConstants.alphaArgument1_3;
		targetState.mSpecializationConstants.alphaArgument1_4 = sourceState.mSpecializationConstants.alphaArgument1_4;
		targetState.mSpecializationConstants.alphaArgument1_5 = sourceState.mSpecializationConstants.alphaArgument1_5;
		targetState.mSpecializationConstants.alphaArgument1_6 = sourceState.mSpecializationConstants.alphaArgument1_6;
		targetState.mSpecializationConstants.alphaArgument1_7 = sourceState.mSpecializationConstants.alphaArgument1_7;

		targetState.mSpecializationConstants.alphaArgument2_0 = sourceState.mSpecializationConstants.alphaArgument2_0;
		targetState.mSpecializationConstants.alphaArgument2_1 = sourceState.mSpecializationConstants.alphaArgument2_1;
		targetState.mSpecializationConstants.alphaArgument2_2 = sourceState.mSpecializationConstants.alphaArgument2_2;
		targetState.mSpecializationConstants.alphaArgument2_3 = sourceState.mSpecializationConstants.alphaArgument2_3;
		targetState.mSpecializationConstants.alphaArgument2_4 = sourceState.mSpecializationConstants.alphaArgument2_4;
		targetState.mSpecializationConstants.alphaArgument2_5 = sourceState.mSpecializationConstants.alphaArgument2_5;
		targetState.mSpecializationConstants.alphaArgument2_6 = sourceState.mSpecializationConstants.alphaArgument2_6;
		targetState.mSpecializationConstants.alphaArgument2_7 = sourceState.mSpecializationConstants.alphaArgument2_7;

		targetState.mSpecializationConstants.bumpMapMatrix00_0 = sourceState.mSpecializationConstants.bumpMapMatrix00_0;
		targetState.mSpecializationConstants.bumpMapMatrix00_1 = sourceState.mSpecializationConstants.bumpMapMatrix00_1;
		targetState.mSpecializationConstants.bumpMapMatrix00_2 = sourceState.mSpecializationConstants.bumpMapMatrix00_2;
		targetState.mSpecializationConstants.bumpMapMatrix00_3 = sourceState.mSpecializationConstants.bumpMapMatrix00_3;
		targetState.mSpecializationConstants.bumpMapMatrix00_4 = sourceState.mSpecializationConstants.bumpMapMatrix00_4;
		targetState.mSpecializationConstants.bumpMapMatrix00_5 = sourceState.mSpecializationConstants.bumpMapMatrix00_5;
		targetState.mSpecializationConstants.bumpMapMatrix00_6 = sourceState.mSpecializationConstants.bumpMapMatrix00_6;
		targetState.mSpecializationConstants.bumpMapMatrix00_7 = sourceState.mSpecializationConstants.bumpMapMatrix00_7;

		targetState.mSpecializationConstants.bumpMapMatrix01_0 = sourceState.mSpecializationConstants.bumpMapMatrix01_0;
		targetState.mSpecializationConstants.bumpMapMatrix01_1 = sourceState.mSpecializationConstants.bumpMapMatrix01_1;
		targetState.mSpecializationConstants.bumpMapMatrix01_2 = sourceState.mSpecializationConstants.bumpMapMatrix01_2;
		targetState.mSpecializationConstants.bumpMapMatrix01_3 = sourceState.mSpecializationConstants.bumpMapMatrix01_3;
		targetState.mSpecializationConstants.bumpMapMatrix01_4 = sourceState.mSpecializationConstants.bumpMapMatrix01_4;
		targetState.mSpecializationConstants.bumpMapMatrix01_5 = sourceState.mSpecializationConstants.bumpMapMatrix01_5;
		targetState.mSpecializationConstants.bumpMapMatrix01_6 = sourceState.mSpecializationConstants.bumpMapMatrix01_6;
		targetState.mSpecializationConstants.bumpMapMatrix01_7 = sourceState.mSpecializationConstants.bumpMapMatrix01_7;

		targetState.mSpecializationConstants.bumpMapMatrix10_0 = sourceState.mSpecializationConstants.bumpMapMatrix10_0;
		targetState.mSpecializationConstants.bumpMapMatrix10_1 = sourceState.mSpecializationConstants.bumpMapMatrix10_1;
		targetState.mSpecializationConstants.bumpMapMatrix10_2 = sourceState.mSpecializationConstants.bumpMapMatrix10_2;
		targetState.mSpecializationConstants.bumpMapMatrix10_3 = sourceState.mSpecializationConstants.bumpMapMatrix10_3;
		targetState.mSpecializationConstants.bumpMapMatrix10_4 = sourceState.mSpecializationConstants.bumpMapMatrix10_4;
		targetState.mSpecializationConstants.bumpMapMatrix10_5 = sourceState.mSpecializationConstants.bumpMapMatrix10_5;
		targetState.mSpecializationConstants.bumpMapMatrix10_6 = sourceState.mSpecializationConstants.bumpMapMatrix10_6;
		targetState.mSpecializationConstants.bumpMapMatrix10_7 = sourceState.mSpecializationConstants.bumpMapMatrix10_7;

		targetState.mSpecializationConstants.bumpMapMatrix11_0 = sourceState.mSpecializationConstants.bumpMapMatrix11_0;
		targetState.mSpecializationConstants.bumpMapMatrix11_1 = sourceState.mSpecializationConstants.bumpMapMatrix11_1;
		targetState.mSpecializationConstants.bumpMapMatrix11_2 = sourceState.mSpecializationConstants.bumpMapMatrix11_2;
		targetState.mSpecializationConstants.bumpMapMatrix11_3 = sourceState.mSpecializationConstants.bumpMapMatrix11_3;
		targetState.mSpecializationConstants.bumpMapMatrix11_4 = sourceState.mSpecializationConstants.bumpMapMatrix11_4;
		targetState.mSpecializationConstants.bumpMapMatrix11_5 = sourceState.mSpecializationConstants.bumpMapMatrix11_5;
		targetState.mSpecializationConstants.bumpMapMatrix11_6 = sourceState.mSpecializationConstants.bumpMapMatrix11_6;
		targetState.mSpecializationConstants.bumpMapMatrix11_7 = sourceState.mSpecializationConstants.bumpMapMatrix11_7;

		targetState.mSpecializationConstants.texureCoordinateIndex_0 = sourceState.mSpecializationConstants.texureCoordinateIndex_0;
		targetState.mSpecializationConstants.texureCoordinateIndex_1 = sourceState.mSpecializationConstants.texureCoordinateIndex_1;
		targetState.mSpecializationConstants.texureCoordinateIndex_2 = sourceState.mSpecializationConstants.texureCoordinateIndex_2;
		targetState.mSpecializationConstants.texureCoordinateIndex_3 = sourceState.mSpecializationConstants.texureCoordinateIndex_3;
		targetState.mSpecializationConstants.texureCoordinateIndex_4 = sourceState.mSpecializationConstants.texureCoordinateIndex_4;
		targetState.mSpecializationConstants.texureCoordinateIndex_5 = sourceState.mSpecializationConstants.texureCoordinateIndex_5;
		targetState.mSpecializationConstants.texureCoordinateIndex_6 = sourceState.mSpecializationConstants.texureCoordinateIndex_6;
		targetState.mSpecializationConstants.texureCoordinateIndex_7 = sourceState.mSpecializationConstants.texureCoordinateIndex_7;

		targetState.mSpecializationConstants.bumpMapScale_0 = sourceState.mSpecializationConstants.bumpMapScale_0;
		targetState.mSpecializationConstants.bumpMapScale_1 = sourceState.mSpecializationConstants.bumpMapScale_1;
		targetState.mSpecializationConstants.bumpMapScale_2 = sourceState.mSpecializationConstants.bumpMapScale_2;
		targetState.mSpecializationConstants.bumpMapScale_3 = sourceState.mSpecializationConstants.bumpMapScale_3;
		targetState.mSpecializationConstants.bumpMapScale_4 = sourceState.mSpecializationConstants.bumpMapScale_4;
		targetState.mSpecializationConstants.bumpMapScale_5 = sourceState.mSpecializationConstants.bumpMapScale_5;
		targetState.mSpecializationConstants.bumpMapScale_6 = sourceState.mSpecializationConstants.bumpMapScale_6;
		targetState.mSpecializationConstants.bumpMapScale_7 = sourceState.mSpecializationConstants.bumpMapScale_7;

		targetState.mSpecializationConstants.bumpMapOffset_0 = sourceState.mSpecializationConstants.bumpMapOffset_0;
		targetState.mSpecializationConstants.bumpMapOffset_1 = sourceState.mSpecializationConstants.bumpMapOffset_1;
		targetState.mSpecializationConstants.bumpMapOffset_2 = sourceState.mSpecializationConstants.bumpMapOffset_2;
		targetState.mSpecializationConstants.bumpMapOffset_3 = sourceState.mSpecializationConstants.bumpMapOffset_3;
		targetState.mSpecializationConstants.bumpMapOffset_4 = sourceState.mSpecializationConstants.bumpMapOffset_4;
		targetState.mSpecializationConstants.bumpMapOffset_5 = sourceState.mSpecializationConstants.bumpMapOffset_5;
		targetState.mSpecializationConstants.bumpMapOffset_6 = sourceState.mSpecializationConstants.bumpMapOffset_6;
		targetState.mSpecializationConstants.bumpMapOffset_7 = sourceState.mSpecializationConstants.bumpMapOffset_7;

		targetState.mSpecializationConstants.textureTransformationFlags_0 = sourceState.mSpecializationConstants.textureTransformationFlags_0;
		targetState.mSpecializationConstants.textureTransformationFlags_1 = sourceState.mSpecializationConstants.textureTransformationFlags_1;
		targetState.mSpecializationConstants.textureTransformationFlags_2 = sourceState.mSpecializationConstants.textureTransformationFlags_2;
		targetState.mSpecializationConstants.textureTransformationFlags_3 = sourceState.mSpecializationConstants.textureTransformationFlags_3;
		targetState.mSpecializationConstants.textureTransformationFlags_4 = sourceState.mSpecializationConstants.textureTransformationFlags_4;
		targetState.mSpecializationConstants.textureTransformationFlags_5 = sourceState.mSpecializationConstants.textureTransformationFlags_5;
		targetState.mSpecializationConstants.textureTransformationFlags_6 = sourceState.mSpecializationConstants.textureTransformationFlags_6;
		targetState.mSpecializationConstants.textureTransformationFlags_7 = sourceState.mSpecializationConstants.textureTransformationFlags_7;

		targetState.mSpecializationConstants.colorArgument0_0 = sourceState.mSpecializationConstants.colorArgument0_0;
		targetState.mSpecializationConstants.colorArgument0_1 = sourceState.mSpecializationConstants.colorArgument0_1;
		targetState.mSpecializationConstants.colorArgument0_2 = sourceState.mSpecializationConstants.colorArgument0_2;
		targetState.mSpecializationConstants.colorArgument0_3 = sourceState.mSpecializationConstants.colorArgument0_3;
		targetState.mSpecializationConstants.colorArgument0_4 = sourceState.mSpecializationConstants.colorArgument0_4;
		targetState.mSpecializationConstants.colorArgument0_5 = sourceState.mSpecializationConstants.colorArgument0_5;
		targetState.mSpecializationConstants.colorArgument0_6 = sourceState.mSpecializationConstants.colorArgument0_6;
		targetState.mSpecializationConstants.colorArgument0_7 = sourceState.mSpecializationConstants.colorArgument0_7;

		targetState.mSpecializationConstants.alphaArgument0_0 = sourceState.mSpecializationConstants.alphaArgument0_0;
		targetState.mSpecializationConstants.alphaArgument0_1 = sourceState.mSpecializationConstants.alphaArgument0_1;
		targetState.mSpecializationConstants.alphaArgument0_2 = sourceState.mSpecializationConstants.alphaArgument0_2;
		targetState.mSpecializationConstants.alphaArgument0_3 = sourceState.mSpecializationConstants.alphaArgument0_3;
		targetState.mSpecializationConstants.alphaArgument0_4 = sourceState.mSpecializationConstants.alphaArgument0_4;
		targetState.mSpecializationConstants.alphaArgument0_5 = sourceState.mSpecializationConstants.alphaArgument0_5;
		targetState.mSpecializationConstants.alphaArgument0_6 = sourceState.mSpecializationConstants.alphaArgument0_6;
		targetState.mSpecializationConstants.alphaArgument0_7 = sourceState.mSpecializationConstants.alphaArgument0_7;

		targetState.mSpecializationConstants.Result_0 = sourceState.mSpecializationConstants.Result_0;
		targetState.mSpecializationConstants.Result_1 = sourceState.mSpecializationConstants.Result_1;
		targetState.mSpecializationConstants.Result_2 = sourceState.mSpecializationConstants.Result_2;
		targetState.mSpecializationConstants.Result_3 = sourceState.mSpecializationConstants.Result_3;
		targetState.mSpecializationConstants.Result_4 = sourceState.mSpecializationConstants.Result_4;
		targetState.mSpecializationConstants.Result_5 = sourceState.mSpecializationConstants.Result_5;
		targetState.mSpecializationConstants.Result_6 = sourceState.mSpecializationConstants.Result_6;
		targetState.mSpecializationConstants.Result_7 = sourceState.mSpecializationConstants.Result_7;
	}

	//IDirect3DDevice9::SetTransform
	if (type == D3DSBT_ALL)
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mTransforms)
		{
			if (!onlyIfExists || targetState.mTransforms.count(pair1.first) > 0)
			{
				targetState.mTransforms[pair1.first] = pair1.second;
			}		
		}
	}

	//IDirect3DDevice9::SetViewport
	if ((sourceState.m9Viewport.Width != 0) && (!onlyIfExists || targetState.m9Viewport.Width != 0) && (type == D3DSBT_ALL))
	{
		targetState.m9Viewport = sourceState.m9Viewport;
		targetState.mViewport = sourceState.mViewport;
	}

	//IDirect3DDevice9::SetVertexShader
	if (sourceState.mHasVertexShader && (!onlyIfExists || targetState.mHasVertexShader) && (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE))
	{
		//if (targetState.mVertexShader != nullptr)
		//{
		//	targetState.mVertexShader->Release();
		//}

		targetState.mVertexShader = sourceState.mVertexShader;
		targetState.mHasVertexShader = true;
	}

	//IDirect3DDevice9::SetVertexShaderConstantB
	//IDirect3DDevice9::SetVertexShaderConstantF
	//IDirect3DDevice9::SetVertexShaderConstantI
}