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

#include "CStateBlock9.h"
#include "CDevice9.h"

#include "Utilities.h"

CStateBlock9::CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type)
	: mDevice(device),
	mType(Type)
{
	BOOST_LOG_TRIVIAL(info) << "CStateBlock9::CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type)";
	this->Capture();
}

CStateBlock9::CStateBlock9(CDevice9* device)
	: mDevice(device)
{
	BOOST_LOG_TRIVIAL(info) << "CStateBlock9::CStateBlock9(CDevice9* device)";
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
	BOOST_LOG_TRIVIAL(info) << "CStateBlock9::Capture";
	//Print(this->mDevice->mDeviceState);

	MergeState(this->mDevice->mDeviceState, mDeviceState, mType);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Apply()
{
	BOOST_LOG_TRIVIAL(info) << "CStateBlock9::Apply";

	MergeState(mDeviceState, this->mDevice->mDeviceState,mType);

	if (mDeviceState.mTransforms.size())
	{
		this->mDevice->mBufferManager->UpdateUniformBuffer(true); //Have to push the updated UBO into memory buffer.
	}

	//Print(this->mDevice->mDeviceState);

	return S_OK;
}

void MergeState(const DeviceState& sourceState, DeviceState& targetState, D3DSTATEBLOCKTYPE type)
{
	if (type != D3DSBT_ALL)
	{
		BOOST_LOG_TRIVIAL(warning) << "MergeState unsupported block type " << type;
	}

	//IDirect3DDevice9::LightEnable
	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette
	//IDirect3DDevice9::SetFVF
	if (sourceState.mFVF != -1)
	{
		targetState.mFVF = sourceState.mFVF;

		targetState.mFVFHasPosition = sourceState.mFVFHasPosition;
		targetState.mFVFHasNormal = sourceState.mFVFHasNormal;
		targetState.mFVFHasColor = sourceState.mFVFHasColor;
		targetState.mFVFTextureCount = sourceState.mFVFTextureCount;

		//If this is set it will be reset later.
		targetState.mVertexDeclaration = nullptr;
		targetState.mHasVertexDeclaration = true;
	}

	//IDirect3DDevice9::SetIndices
	if (sourceState.mHasIndexBuffer)
	{
		targetState.mIndexBuffer = sourceState.mIndexBuffer;
		targetState.mHasIndexBuffer = true;
	}

	//IDirect3DDevice9::SetLight
	//IDirect3DDevice9::SetMaterial
	//IDirect3DDevice9::SetNPatchMode
	if (sourceState.mNSegments != -1)
	{
		targetState.mNSegments = sourceState.mNSegments; //Doesn't matter anyway.
	}

	//IDirect3DDevice9::SetPixelShader
	if (sourceState.mHasPixelShader)
	{
		//TODO: may leak
		targetState.mPixelShader = sourceState.mPixelShader;
		targetState.mHasPixelShader = true;
	}

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	if (sourceState.mRenderStates.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mRenderStates)
		{
			targetState.mRenderStates[pair1.first] = pair1.second;
		}
	}

	//IDirect3DDevice9::SetSamplerState
	if (sourceState.mSamplerStates.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mSamplerStates)
		{
			BOOST_FOREACH(const auto& pair2, pair1.second)
			{
				targetState.mSamplerStates[pair1.first][pair2.first] = pair2.second;
			}
		}
	}

	//IDirect3DDevice9::SetScissorRect
	if (sourceState.m9Scissor.right != 0 || sourceState.m9Scissor.left != 0)
	{
		targetState.m9Scissor = sourceState.m9Scissor;
		targetState.mScissor = sourceState.mScissor;
	}

	//IDirect3DDevice9::SetStreamSource
	if (sourceState.mStreamSources.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mStreamSources)
		{
			targetState.mStreamSources[pair1.first] = pair1.second;
		}
	}

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	for (size_t i = 0; i < 16; i++)
	{
		const VkDescriptorImageInfo& sourceSampler = sourceState.mDescriptorImageInfo[i];
		if (sourceSampler.sampler != VK_NULL_HANDLE)
		{
			VkDescriptorImageInfo& targetSampler = targetState.mDescriptorImageInfo[i];

			targetSampler.imageLayout = sourceSampler.imageLayout;
			targetSampler.imageView = sourceSampler.imageView;
			targetSampler.sampler = sourceSampler.sampler;
		}
	}

	//IDirect3DDevice9::SetTextureStageState
	if (sourceState.mTextureStageStates.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mTextureStageStates)
		{
			BOOST_FOREACH(const auto& pair2, pair1.second)
			{
				targetState.mTextureStageStates[pair1.first][pair2.first] = pair2.second;
			}
		}
	}

	//IDirect3DDevice9::SetTransform
	if (sourceState.mTransforms.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mTransforms)
		{
			targetState.mTransforms[pair1.first] = pair1.second;
		}
	}

	//IDirect3DDevice9::SetViewport
	if (sourceState.m9Viewport.Width != 0)
	{
		targetState.m9Viewport = sourceState.m9Viewport;
		targetState.mViewport = sourceState.mViewport;
	}

	//IDirect3DDevice9::SetVertexDeclaration
	if (sourceState.mHasVertexDeclaration)
	{
		targetState.mVertexDeclaration = sourceState.mVertexDeclaration;
		targetState.mHasVertexDeclaration = true;
	}

	//IDirect3DDevice9::SetVertexShader
	if (sourceState.mHasVertexShader)
	{
		targetState.mVertexShader = sourceState.mVertexShader;
		targetState.mHasVertexShader = true;
	}

	//IDirect3DDevice9::SetVertexShaderConstantB
	//IDirect3DDevice9::SetVertexShaderConstantF
	//IDirect3DDevice9::SetVertexShaderConstantI
}