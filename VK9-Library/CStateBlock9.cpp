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
	if (sourceState.mRenderStates.size())
	{
		BOOST_FOREACH(const auto& pair1, sourceState.mRenderStates)
		{
			if (!onlyIfExists || targetState.mRenderStates.count(pair1.first) > 0)
			{
				if 
				(
					(type == D3DSBT_ALL) || 
					(type == D3DSBT_VERTEXSTATE && 
						(
							pair1.first == D3DRS_CULLMODE || 
							pair1.first == D3DRS_FOGCOLOR || 
							pair1.first == D3DRS_FOGTABLEMODE ||
							pair1.first == D3DRS_FOGSTART ||
							pair1.first == D3DRS_FOGEND ||
							pair1.first == D3DRS_FOGDENSITY ||
							pair1.first == D3DRS_RANGEFOGENABLE ||
							pair1.first == D3DRS_AMBIENT ||
							pair1.first == D3DRS_COLORVERTEX ||
							pair1.first == D3DRS_FOGVERTEXMODE ||
							pair1.first == D3DRS_CLIPPING ||
							pair1.first == D3DRS_LIGHTING ||
							pair1.first == D3DRS_LOCALVIEWER ||
							pair1.first == D3DRS_EMISSIVEMATERIALSOURCE ||
							pair1.first == D3DRS_AMBIENTMATERIALSOURCE ||
							pair1.first == D3DRS_DIFFUSEMATERIALSOURCE ||
							pair1.first == D3DRS_SPECULARMATERIALSOURCE ||
							pair1.first == D3DRS_VERTEXBLEND ||
							pair1.first == D3DRS_CLIPPLANEENABLE ||
							pair1.first == D3DRS_POINTSIZE ||
							pair1.first == D3DRS_POINTSIZE_MIN ||
							pair1.first == D3DRS_POINTSPRITEENABLE ||
							pair1.first == D3DRS_POINTSCALEENABLE ||
							pair1.first == D3DRS_POINTSCALE_A ||
							pair1.first == D3DRS_POINTSCALE_B ||
							pair1.first == D3DRS_POINTSCALE_C ||
							pair1.first == D3DRS_MULTISAMPLEANTIALIAS ||
							pair1.first == D3DRS_MULTISAMPLEMASK ||
							pair1.first == D3DRS_PATCHEDGESTYLE ||
							pair1.first == D3DRS_POINTSIZE_MAX ||
							pair1.first == D3DRS_INDEXEDVERTEXBLENDENABLE ||
							pair1.first == D3DRS_TWEENFACTOR ||
							pair1.first == D3DRS_POSITIONDEGREE ||
							pair1.first == D3DRS_NORMALDEGREE ||
							pair1.first == D3DRS_MINTESSELLATIONLEVEL ||
							pair1.first == D3DRS_MAXTESSELLATIONLEVEL ||
							pair1.first == D3DRS_ADAPTIVETESS_X ||
							pair1.first == D3DRS_ADAPTIVETESS_Y ||
							pair1.first == D3DRS_ADAPTIVETESS_Z ||
							pair1.first == D3DRS_ADAPTIVETESS_W ||
							pair1.first == D3DRS_ENABLEADAPTIVETESSELLATION
						)) ||
					(type == D3DSBT_PIXELSTATE && 
						(
							pair1.first == D3DRS_ZENABLE ||
							pair1.first == D3DRS_SPECULARENABLE ||
							//pair1.first == D3DFILLMODE ||
							//pair1.first == D3DSHADEMODE ||
							pair1.first == D3DRS_ZWRITEENABLE ||
							pair1.first == D3DRS_ALPHATESTENABLE ||
							pair1.first == D3DRS_LASTPIXEL ||
							pair1.first == D3DRS_SRCBLEND ||
							pair1.first == D3DRS_DESTBLEND ||
							pair1.first == D3DRS_ZFUNC ||
							pair1.first == D3DRS_ALPHAREF ||
							pair1.first == D3DRS_ALPHAFUNC ||
							pair1.first == D3DRS_DITHERENABLE ||
							pair1.first == D3DRS_FOGSTART ||
							pair1.first == D3DRS_FOGEND ||
							pair1.first == D3DRS_FOGDENSITY ||
							pair1.first == D3DRS_ALPHABLENDENABLE ||
							pair1.first == D3DRS_DEPTHBIAS ||
							pair1.first == D3DRS_STENCILENABLE ||
							pair1.first == D3DRS_STENCILFAIL ||
							pair1.first == D3DRS_STENCILZFAIL ||
							pair1.first == D3DRS_STENCILPASS ||
							pair1.first == D3DRS_STENCILFUNC ||
							pair1.first == D3DRS_STENCILREF ||
							pair1.first == D3DRS_STENCILMASK ||
							pair1.first == D3DRS_STENCILWRITEMASK ||
							pair1.first == D3DRS_TEXTUREFACTOR ||
							pair1.first == D3DRS_WRAP0 ||
							pair1.first == D3DRS_WRAP1 ||
							pair1.first == D3DRS_WRAP2 ||
							pair1.first == D3DRS_WRAP3 ||
							pair1.first == D3DRS_WRAP4 ||
							pair1.first == D3DRS_WRAP5 ||
							pair1.first == D3DRS_WRAP6 ||
							pair1.first == D3DRS_WRAP7 ||
							pair1.first == D3DRS_WRAP8 ||
							pair1.first == D3DRS_WRAP9 ||
							pair1.first == D3DRS_WRAP10 ||
							pair1.first == D3DRS_WRAP11 ||
							pair1.first == D3DRS_WRAP12 ||
							pair1.first == D3DRS_WRAP13 ||
							pair1.first == D3DRS_WRAP14 ||
							pair1.first == D3DRS_WRAP15 ||
							pair1.first == D3DRS_LOCALVIEWER ||
							pair1.first == D3DRS_EMISSIVEMATERIALSOURCE ||
							pair1.first == D3DRS_AMBIENTMATERIALSOURCE ||
							pair1.first == D3DRS_DIFFUSEMATERIALSOURCE ||
							pair1.first == D3DRS_SPECULARMATERIALSOURCE ||
							pair1.first == D3DRS_COLORWRITEENABLE ||
							//pair1.first == D3DBLENDOP ||
							pair1.first == D3DRS_SCISSORTESTENABLE ||
							pair1.first == D3DRS_SLOPESCALEDEPTHBIAS ||
							pair1.first == D3DRS_ANTIALIASEDLINEENABLE ||
							pair1.first == D3DRS_TWOSIDEDSTENCILMODE ||
							pair1.first == D3DRS_CCW_STENCILFAIL ||
							pair1.first == D3DRS_CCW_STENCILZFAIL ||
							pair1.first == D3DRS_CCW_STENCILPASS ||
							pair1.first == D3DRS_CCW_STENCILFUNC ||
							pair1.first == D3DRS_COLORWRITEENABLE1 ||
							pair1.first == D3DRS_COLORWRITEENABLE2 ||
							pair1.first == D3DRS_COLORWRITEENABLE3 ||
							pair1.first == D3DRS_BLENDFACTOR ||
							pair1.first == D3DRS_SRGBWRITEENABLE ||
							pair1.first == D3DRS_SEPARATEALPHABLENDENABLE ||
							pair1.first == D3DRS_SRCBLENDALPHA ||
							pair1.first == D3DRS_DESTBLENDALPHA ||
							pair1.first == D3DRS_BLENDOPALPHA
						))
				)
				{
					targetState.mRenderStates[pair1.first] = pair1.second;
				}
			}	
		}
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