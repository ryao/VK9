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
	this->Capture();
}

CStateBlock9::CStateBlock9(CDevice9* device)
	: mDevice(device)
{

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
	switch (mType)
	{
	case D3DSBT_ALL:
		this->mDeviceState = mDevice->mDeviceState;
		break;
	default:
		//TODO: copy state based on type.
		BOOST_LOG_TRIVIAL(warning) << "CStateBlock9::CStateBlock9 unsupported block type " << mType;
		break;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Apply()
{
	//IDirect3DDevice9::LightEnable
	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette
	//IDirect3DDevice9::SetFVF
	if (mDeviceState.mFVF != 0)
	{
		this->mDevice->mDeviceState.mFVF = mDeviceState.mFVF;
		this->mDevice->mDeviceState.mFVFHasPosition = mDeviceState.mFVFHasPosition;
		this->mDevice->mDeviceState.mFVFHasNormal = mDeviceState.mFVFHasNormal;
		this->mDevice->mDeviceState.mFVFHasColor = mDeviceState.mFVFHasColor;
		this->mDevice->mDeviceState.mFVFTextureCount = mDeviceState.mFVFTextureCount;
	}

	//IDirect3DDevice9::SetIndices
	if (mDeviceState.mIndexBuffer != nullptr)
	{
		this->mDevice->mDeviceState.mIndexBuffer = mDeviceState.mIndexBuffer;
	}

	//IDirect3DDevice9::SetLight
	//IDirect3DDevice9::SetMaterial
	//IDirect3DDevice9::SetNPatchMode
	this->mDevice->mDeviceState.mNSegments = mDeviceState.mNSegments; //Doesn't matter anyway.

	//IDirect3DDevice9::SetPixelShader
	if (mDeviceState.mPixelShader != nullptr)
	{
		this->mDevice->mDeviceState.mPixelShader = mDeviceState.mPixelShader;
	}

	//IDirect3DDevice9::SetPixelShaderConstantB
	//IDirect3DDevice9::SetPixelShaderConstantF
	//IDirect3DDevice9::SetPixelShaderConstantI
	//IDirect3DDevice9::SetRenderState
	if (mDeviceState.mRenderStates.size())
	{
		BOOST_FOREACH(const auto& pair1, mDeviceState.mRenderStates)
		{
			this->mDevice->mDeviceState.mRenderStates[pair1.first] = pair1.second;
		}
	}

	//IDirect3DDevice9::SetSamplerState
	if (mDeviceState.mSamplerStates.size())
	{
		BOOST_FOREACH(const auto& pair1, mDeviceState.mSamplerStates)
		{
			BOOST_FOREACH(const auto& pair2, pair1.second)
			{
				this->mDevice->mDeviceState.mSamplerStates[pair1.first][pair2.first] = pair2.second;
			}
		}
	}

	//IDirect3DDevice9::SetScissorRect
	if (mDeviceState.m9Scissor.right != 0 || mDeviceState.m9Scissor.left != 0)
	{
		this->mDevice->mDeviceState.m9Scissor = mDeviceState.m9Scissor;
		this->mDevice->mDeviceState.mScissor = mDeviceState.mScissor;
	}

	//IDirect3DDevice9::SetStreamSource
	if (mDeviceState.mStreamSources.size())
	{
		BOOST_FOREACH(const auto& pair1, mDeviceState.mStreamSources)
		{
			this->mDevice->mDeviceState.mStreamSources[pair1.first] = pair1.second;
		}
	}

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	for (size_t i = 0; i < 16; i++)
	{
		if (mDeviceState.mDescriptorImageInfo[i].sampler != VK_NULL_HANDLE)
		{
			this->mDevice->mDeviceState.mDescriptorImageInfo[i] = mDeviceState.mDescriptorImageInfo[i];
		}
	}

	//IDirect3DDevice9::SetTextureStageState
	if (mDeviceState.mTextureStageStates.size())
	{
		BOOST_FOREACH(const auto& pair1, mDeviceState.mTextureStageStates)
		{
			BOOST_FOREACH(const auto& pair2, pair1.second)
			{
				this->mDevice->mDeviceState.mTextureStageStates[pair1.first][pair2.first] = pair2.second;
			}
		}		
	}

	//IDirect3DDevice9::SetTransform
	if (mDeviceState.mTransforms.size())
	{
		BOOST_FOREACH(const auto& pair1, mDeviceState.mTransforms)
		{
			this->mDevice->mDeviceState.mTransforms[pair1.first] = pair1.second;
		}

		this->mDevice->mBufferManager->UpdateUniformBuffer(); //Have to push the updated UBO into memory buffer.
	}

	//IDirect3DDevice9::SetViewport
	if (mDeviceState.m9Viewport.Width != 0)
	{
		this->mDevice->mDeviceState.m9Viewport = mDeviceState.m9Viewport;
		this->mDevice->mDeviceState.mViewport = mDeviceState.mViewport;
	}

	//IDirect3DDevice9::SetVertexDeclaration
	if (mDeviceState.mVertexDeclaration != nullptr)
	{
		this->mDevice->mDeviceState.mVertexDeclaration = mDeviceState.mVertexDeclaration;
	}

	//IDirect3DDevice9::SetVertexShader
	if (mDeviceState.mVertexShader != nullptr)
	{
		this->mDevice->mDeviceState.mVertexShader = mDeviceState.mVertexShader;
	}

	//IDirect3DDevice9::SetVertexShaderConstantB
	//IDirect3DDevice9::SetVertexShaderConstantF
	//IDirect3DDevice9::SetVertexShaderConstantI

	return S_OK;
}