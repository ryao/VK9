// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "Utilities.h"

#ifdef _MSC_VER
#include "winres.h"
#endif
#include <stdio.h>
#include <stdlib.h>

void MergeState(const DeviceState& sourceState, DeviceState& targetState, D3DSTATEBLOCKTYPE type, BOOL onlyIfExists)
{
	/*
	Pixel https://msdn.microsoft.com/en-us/library/windows/desktop/bb147351(v=vs.85).aspx
	Vertex https://msdn.microsoft.com/en-us/library/windows/desktop/bb147353(v=vs.85).aspx#Vertex_Pipeline_Render_State
	All https://msdn.microsoft.com/en-us/library/windows/desktop/bb147350(v=vs.85).aspx
	*/

	targetState.mIsRenderStateDirty = true;

	//if (type == D3DSBT_ALL)
	//{
	//	onlyIfExists = false;
	//}

	//I don't see in render target nor depth buffer in docs but applications use them as if they are included in state blocks so I guess I'll roll with it.
	
	//&& (!onlyIfExists || targetState.mRenderTarget != nullptr)
	if (sourceState.mRenderTarget != nullptr  && (type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD))
	{
		targetState.mRenderTarget = sourceState.mRenderTarget;
	}

	//IDirect3DDevice9::LightEnable
	//if ((type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE))
	//{
	//	for(const auto& pair1 : sourceState.mLightSettings)
	//	{
	//		if (!onlyIfExists || targetState.mLightSettings.count(pair1.first) > 0)
	//		{
	//			targetState.mLightSettings[pair1.first] = pair1.second;
	//		}
	//	}
	//}

	//IDirect3DDevice9::SetClipPlane
	//IDirect3DDevice9::SetCurrentTexturePalette

	//IDirect3DDevice9::SetVertexDeclaration
	//IDirect3DDevice9::SetFVF
	if ((sourceState.mHasVertexDeclaration || sourceState.mHasFVF) && (!onlyIfExists || targetState.mHasFVF || targetState.mHasVertexDeclaration) && (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE || type == D3DSBT_FORCE_DWORD))
	{
		targetState.mFVF = sourceState.mFVF;
		targetState.mHasFVF = sourceState.mHasFVF;

		targetState.mVertexDeclaration = sourceState.mVertexDeclaration;
		targetState.mHasVertexDeclaration = sourceState.mHasVertexDeclaration;
	}

	//IDirect3DDevice9::SetIndices
	if (sourceState.mHasIndexBuffer && (!onlyIfExists || targetState.mHasIndexBuffer) && (type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD))
	{
		targetState.mIndexBuffer = sourceState.mIndexBuffer;
		targetState.mHasIndexBuffer = true;
	}

	if ((type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE || type == D3DSBT_FORCE_DWORD))
	{
		//IDirect3DDevice9::SetLight
		for (size_t i = 0; i < 8; i++)
		{
			targetState.mShaderState.mLights[i] = sourceState.mShaderState.mLights[i];
		}

		targetState.mAreLightsDirty = true;

		//IDirect3DDevice9::SetMaterial
		targetState.mShaderState.mMaterial = sourceState.mShaderState.mMaterial;
	
		targetState.mIsMaterialDirty = true;
	}

	//IDirect3DDevice9::SetNPatchMode
	if (sourceState.mNSegments != -1 && (!onlyIfExists || targetState.mNSegments != -1) && (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE || type == D3DSBT_FORCE_DWORD))
	{
		targetState.mNSegments = sourceState.mNSegments; //Doesn't matter anyway.
	}

	//IDirect3DDevice9::SetPixelShader
	if (sourceState.mHasPixelShader && (!onlyIfExists || targetState.mHasPixelShader) && (type == D3DSBT_ALL || type == D3DSBT_PIXELSTATE || type == D3DSBT_FORCE_DWORD))
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
	if (type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD)
	{
		if ((sourceState.wasFixedFunctionOtherGroupModified || sourceState.wasFogGroupModified || sourceState.wasPixelShaderConstantGroupModified)
		&& (targetState.wasFixedFunctionOtherGroupModified || targetState.wasFogGroupModified || targetState.wasPixelShaderConstantGroupModified || !onlyIfExists))
		{
			targetState.wasFixedFunctionOtherGroupModified = true;
			targetState.wasFogGroupModified = true;
			targetState.wasPixelShaderConstantGroupModified = true;
			targetState.mShaderState.mRenderState.fogEnable = sourceState.mShaderState.mRenderState.fogEnable;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.normalizeNormals = sourceState.mShaderState.mRenderState.normalizeNormals;
		}
		if (sourceState.hasDebugMonitorToken && (targetState.hasDebugMonitorToken || !onlyIfExists)) {
			targetState.hasDebugMonitorToken = true; 
			targetState.mShaderState.mRenderState.debugMonitorToken = sourceState.mShaderState.mRenderState.debugMonitorToken;
		}
	}

	if (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE || type == D3DSBT_FORCE_DWORD)
	{
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.cullMode = sourceState.mShaderState.mRenderState.cullMode;
		}

		if ((sourceState.wasFixedFunctionOtherGroupModified || sourceState.wasPixelShaderConstantGroupModified)
			&& (targetState.wasFixedFunctionOtherGroupModified || targetState.wasPixelShaderConstantGroupModified || !onlyIfExists))
		{
			targetState.wasFixedFunctionOtherGroupModified = true;
			targetState.wasPixelShaderConstantGroupModified = true;
			targetState.mShaderState.mRenderState.fogColor = sourceState.mShaderState.mRenderState.fogColor;
		}

		if ((sourceState.wasFixedFunctionOtherGroupModified || sourceState.wasFogGroupModified || sourceState.wasPixelShaderConstantGroupModified)
			&& (targetState.wasFixedFunctionOtherGroupModified || targetState.wasFogGroupModified || targetState.wasPixelShaderConstantGroupModified || !onlyIfExists))
		{
			targetState.wasFixedFunctionOtherGroupModified = true;
			targetState.wasFogGroupModified = true;
			targetState.wasPixelShaderConstantGroupModified = true;
			targetState.mShaderState.mRenderState.fogTableMode = sourceState.mShaderState.mRenderState.fogTableMode;
		}

		if ((sourceState.wasFixedFunctionOtherGroupModified || sourceState.wasPixelShaderConstantGroupModified)
			&& (targetState.wasFixedFunctionOtherGroupModified || targetState.wasPixelShaderConstantGroupModified || !onlyIfExists))
		{
			targetState.wasFixedFunctionOtherGroupModified = true;
			targetState.wasPixelShaderConstantGroupModified = true;
			targetState.mShaderState.mRenderState.fogStart = sourceState.mShaderState.mRenderState.fogStart;
		}

		if ((sourceState.wasFixedFunctionOtherGroupModified || sourceState.wasPixelShaderConstantGroupModified)
			&& (targetState.wasFixedFunctionOtherGroupModified || targetState.wasPixelShaderConstantGroupModified || !onlyIfExists))
		{
			targetState.wasFixedFunctionOtherGroupModified = true;
			targetState.wasPixelShaderConstantGroupModified = true;
			targetState.mShaderState.mRenderState.fogEnd = sourceState.mShaderState.mRenderState.fogEnd;
		}

		if ((sourceState.wasFixedFunctionOtherGroupModified || sourceState.wasPixelShaderConstantGroupModified)
			&& (targetState.wasFixedFunctionOtherGroupModified || targetState.wasPixelShaderConstantGroupModified || !onlyIfExists))
		{
			targetState.wasFixedFunctionOtherGroupModified = true;
			targetState.wasPixelShaderConstantGroupModified = true;
			targetState.mShaderState.mRenderState.fogDensity = sourceState.mShaderState.mRenderState.fogDensity;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.rangeFogEnable = sourceState.mShaderState.mRenderState.rangeFogEnable;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.ambient = sourceState.mShaderState.mRenderState.ambient;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true; 
			targetState.mShaderState.mRenderState.colorVertex = sourceState.mShaderState.mRenderState.colorVertex;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true;  
			targetState.mShaderState.mRenderState.fogVertexMode = sourceState.mShaderState.mRenderState.fogVertexMode;
		}
		if (sourceState.hasClipping && (targetState.hasClipping || !onlyIfExists)) {
			targetState.hasClipping = true;  
			targetState.mShaderState.mRenderState.clipping = sourceState.mShaderState.mRenderState.clipping;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.lighting = sourceState.mShaderState.mRenderState.lighting;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.localViewer = sourceState.mShaderState.mRenderState.localViewer;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.emissiveMaterialSource = sourceState.mShaderState.mRenderState.emissiveMaterialSource;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.ambientMaterialSource = sourceState.mShaderState.mRenderState.ambientMaterialSource;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.diffuseMaterialSource = sourceState.mShaderState.mRenderState.diffuseMaterialSource;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true; 
			targetState.mShaderState.mRenderState.specularMaterialSource = sourceState.mShaderState.mRenderState.specularMaterialSource;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true;  
			targetState.mShaderState.mRenderState.vertexBlend = sourceState.mShaderState.mRenderState.vertexBlend;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.clipPlaneEnable = sourceState.mShaderState.mRenderState.clipPlaneEnable;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.pointSize = sourceState.mShaderState.mRenderState.pointSize;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.pointSizeMinimum = sourceState.mShaderState.mRenderState.pointSizeMinimum;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.pointSpriteEnable = sourceState.mShaderState.mRenderState.pointSpriteEnable;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.pointScaleEnable = sourceState.mShaderState.mRenderState.pointScaleEnable;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.pointScaleA = sourceState.mShaderState.mRenderState.pointScaleA;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.pointScaleB = sourceState.mShaderState.mRenderState.pointScaleB;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.pointScaleC = sourceState.mShaderState.mRenderState.pointScaleC;
		}
		if (sourceState.wasMultisampleGroupModified && (targetState.wasMultisampleGroupModified || !onlyIfExists)) {
			targetState.wasMultisampleGroupModified = true; 
			targetState.mShaderState.mRenderState.multisampleAntiAlias = sourceState.mShaderState.mRenderState.multisampleAntiAlias;
		}
		if (sourceState.hasMultisampleMask && (targetState.hasMultisampleMask || !onlyIfExists)) {
			targetState.hasMultisampleMask = true;  
			targetState.mShaderState.mRenderState.multisampleMask = sourceState.mShaderState.mRenderState.multisampleMask;
		}
		if (sourceState.hasPatchEdgeStyle && (targetState.hasPatchEdgeStyle || !onlyIfExists)) {
			targetState.hasPatchEdgeStyle = true;  
			targetState.mShaderState.mRenderState.patchEdgeStyle = sourceState.mShaderState.mRenderState.patchEdgeStyle;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true; 
			targetState.mShaderState.mRenderState.pointSizeMaximum = sourceState.mShaderState.mRenderState.pointSizeMaximum;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.indexedVertexBlendEnable = sourceState.mShaderState.mRenderState.indexedVertexBlendEnable;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true;  
			targetState.mShaderState.mRenderState.tweenFactor = sourceState.mShaderState.mRenderState.tweenFactor;
		}
		if (sourceState.hasPositionDegree && (targetState.hasPositionDegree || !onlyIfExists)) {
			targetState.hasPositionDegree = true; 
			targetState.mShaderState.mRenderState.positionDegree = sourceState.mShaderState.mRenderState.positionDegree;
		}
		if (sourceState.hasNormalDegree && (targetState.hasNormalDegree || !onlyIfExists)) {
			targetState.hasNormalDegree = true;  
			targetState.mShaderState.mRenderState.normalDegree = sourceState.mShaderState.mRenderState.normalDegree;
		}
		if (sourceState.hasMinimumTessellationLevel && (targetState.hasMinimumTessellationLevel || !onlyIfExists)) {
			targetState.hasMinimumTessellationLevel = true; 
			targetState.mShaderState.mRenderState.minimumTessellationLevel = sourceState.mShaderState.mRenderState.minimumTessellationLevel;
		}
		if (sourceState.hasMaximumTessellationLevel && (targetState.hasMaximumTessellationLevel || !onlyIfExists)) {
			targetState.hasMaximumTessellationLevel = true; 
			targetState.mShaderState.mRenderState.maximumTessellationLevel = sourceState.mShaderState.mRenderState.maximumTessellationLevel;
		}
		if (sourceState.hasAdaptivetessX && (targetState.hasAdaptivetessX || !onlyIfExists)) {
			targetState.hasAdaptivetessX = true; 
			targetState.mShaderState.mRenderState.adaptivetessX = sourceState.mShaderState.mRenderState.adaptivetessX;
		}
		if (sourceState.hasAdaptivetessY && (targetState.hasAdaptivetessY || !onlyIfExists)) {
			targetState.hasAdaptivetessY = true;  
			targetState.mShaderState.mRenderState.adaptivetessY = sourceState.mShaderState.mRenderState.adaptivetessY;
		}
		if (sourceState.hasAdaptivetessZ && (targetState.hasAdaptivetessZ || !onlyIfExists)) {
			targetState.hasAdaptivetessZ = true;  
			targetState.mShaderState.mRenderState.adaptivetessZ = sourceState.mShaderState.mRenderState.adaptivetessZ;
		}
		if (sourceState.hasAdaptivetessW && (targetState.hasAdaptivetessW || !onlyIfExists)) {
			targetState.hasAdaptivetessW = true; 
			targetState.mShaderState.mRenderState.adaptivetessW = sourceState.mShaderState.mRenderState.adaptivetessW;
		}
		if (sourceState.hasEnableAdaptiveTessellation && (targetState.hasEnableAdaptiveTessellation || !onlyIfExists)) {
			targetState.hasEnableAdaptiveTessellation = true;  
			targetState.mShaderState.mRenderState.enableAdaptiveTessellation = sourceState.mShaderState.mRenderState.enableAdaptiveTessellation;
		}
	}

	if (type == D3DSBT_ALL || type == D3DSBT_PIXELSTATE || type == D3DSBT_FORCE_DWORD)
	{
		if ((sourceState.wasDsaGroupModified || sourceState.wasMultisampleGroupModified) && (targetState.wasDsaGroupModified || targetState.wasMultisampleGroupModified || !onlyIfExists))
		{
			targetState.wasDsaGroupModified = true;
			targetState.wasMultisampleGroupModified = true;
			targetState.mShaderState.mRenderState.zEnable = sourceState.mShaderState.mRenderState.zEnable;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.specularEnable = sourceState.mShaderState.mRenderState.specularEnable;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.fillMode = sourceState.mShaderState.mRenderState.fillMode;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.shadeMode = sourceState.mShaderState.mRenderState.shadeMode;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.zWriteEnable = sourceState.mShaderState.mRenderState.zWriteEnable;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.alphaTestEnable = sourceState.mShaderState.mRenderState.alphaTestEnable;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true; 
			targetState.mShaderState.mRenderState.lastPixel = sourceState.mShaderState.mRenderState.lastPixel;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.sourceBlend = sourceState.mShaderState.mRenderState.sourceBlend;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true; 
			targetState.mShaderState.mRenderState.destinationBlend = sourceState.mShaderState.mRenderState.destinationBlend;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.zFunction = sourceState.mShaderState.mRenderState.zFunction;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.alphaReference = sourceState.mShaderState.mRenderState.alphaReference;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.alphaFunction = sourceState.mShaderState.mRenderState.alphaFunction;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.ditherEnable = sourceState.mShaderState.mRenderState.ditherEnable;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.fogStart = sourceState.mShaderState.mRenderState.fogStart;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true;  
			targetState.mShaderState.mRenderState.fogEnd = sourceState.mShaderState.mRenderState.fogEnd;
		}
		if (sourceState.wasFixedFunctionOtherGroupModified && (targetState.wasFixedFunctionOtherGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionOtherGroupModified = true; 
			targetState.mShaderState.mRenderState.fogDensity = sourceState.mShaderState.mRenderState.fogDensity;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.alphaBlendEnable = sourceState.mShaderState.mRenderState.alphaBlendEnable;
		}

		//targetState.mShaderState.alphaBlendEnable = sourceState.mShaderState.alphaBlendEnable;

		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.depthBias = sourceState.mShaderState.mRenderState.depthBias;
		}
		if ((sourceState.wasDsaGroupModified || sourceState.wasMultisampleGroupModified) && (targetState.wasDsaGroupModified || targetState.wasMultisampleGroupModified || !onlyIfExists))
		{
			targetState.wasDsaGroupModified = true;  
			targetState.wasMultisampleGroupModified = true;
			targetState.mShaderState.mRenderState.stencilEnable = sourceState.mShaderState.mRenderState.stencilEnable;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.stencilFail = sourceState.mShaderState.mRenderState.stencilFail;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.stencilZFail = sourceState.mShaderState.mRenderState.stencilZFail;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.stencilPass = sourceState.mShaderState.mRenderState.stencilPass;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.stencilFunction = sourceState.mShaderState.mRenderState.stencilFunction;
		}
		if (sourceState.hasStencilReference && (targetState.hasStencilReference || !onlyIfExists)) {
			targetState.hasStencilReference = true; 
			targetState.mShaderState.mRenderState.stencilReference = sourceState.mShaderState.mRenderState.stencilReference;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.stencilMask = sourceState.mShaderState.mRenderState.stencilMask;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.stencilWriteMask = sourceState.mShaderState.mRenderState.stencilWriteMask;
		}
		if (sourceState.hasTextureFactor && (targetState.hasTextureFactor || !onlyIfExists)) {
			targetState.hasTextureFactor = true;  
			targetState.mShaderState.mRenderState.textureFactor = sourceState.mShaderState.mRenderState.textureFactor;
		}
		if (sourceState.hasWrap0 && (targetState.hasWrap0 || !onlyIfExists)) {
			targetState.hasWrap0 = true;  
			targetState.mShaderState.mRenderState.wrap0 = sourceState.mShaderState.mRenderState.wrap0;
		}
		if (sourceState.hasWrap1 && (targetState.hasWrap1 || !onlyIfExists)) {
			targetState.hasWrap1 = true; 
			targetState.mShaderState.mRenderState.wrap1 = sourceState.mShaderState.mRenderState.wrap1;
		}
		if (sourceState.hasWrap2 && (targetState.hasWrap2 || !onlyIfExists)) {
			targetState.hasWrap2 = true;  
			targetState.mShaderState.mRenderState.wrap2 = sourceState.mShaderState.mRenderState.wrap2;
		}
		if (sourceState.hasWrap3 && (targetState.hasWrap3 || !onlyIfExists)) {
			targetState.hasWrap3 = true;  
			targetState.mShaderState.mRenderState.wrap3 = sourceState.mShaderState.mRenderState.wrap3;
		}
		if (sourceState.hasWrap4 && (targetState.hasWrap4 || !onlyIfExists)) {
			targetState.hasWrap4 = true;  
			targetState.mShaderState.mRenderState.wrap4 = sourceState.mShaderState.mRenderState.wrap4;
		}
		if (sourceState.hasWrap5 && (targetState.hasWrap5 || !onlyIfExists)) {
			targetState.hasWrap5 = true; 
			targetState.mShaderState.mRenderState.wrap5 = sourceState.mShaderState.mRenderState.wrap5;
		}
		if (sourceState.hasWrap6 && (targetState.hasWrap6 || !onlyIfExists)) {
			targetState.hasWrap6 = true; 
			targetState.mShaderState.mRenderState.wrap6 = sourceState.mShaderState.mRenderState.wrap6;
		}
		if (sourceState.hasWrap7 && (targetState.hasWrap7 || !onlyIfExists)) {
			targetState.hasWrap7 = true; 
			targetState.mShaderState.mRenderState.wrap7 = sourceState.mShaderState.mRenderState.wrap7;
		}
		if (sourceState.hasWrap8 && (targetState.hasWrap8 || !onlyIfExists)) {
			targetState.hasWrap8 = true;  
			targetState.mShaderState.mRenderState.wrap8 = sourceState.mShaderState.mRenderState.wrap8;
		}
		if (sourceState.hasWrap9 && (targetState.hasWrap9 || !onlyIfExists)) {
			targetState.hasWrap9 = true; 
			targetState.mShaderState.mRenderState.wrap9 = sourceState.mShaderState.mRenderState.wrap9;
		}
		if (sourceState.hasWrap10 && (targetState.hasWrap10 || !onlyIfExists)) {
			targetState.hasWrap10 = true; 
			targetState.mShaderState.mRenderState.wrap10 = sourceState.mShaderState.mRenderState.wrap10;
		}
		if (sourceState.hasWrap11 && (targetState.hasWrap11 || !onlyIfExists)) {
			targetState.hasWrap11 = true;  
			targetState.mShaderState.mRenderState.wrap11 = sourceState.mShaderState.mRenderState.wrap11;
		}
		if (sourceState.hasWrap12 && (targetState.hasWrap12 || !onlyIfExists)) {
			targetState.hasWrap12 = true;  
			targetState.mShaderState.mRenderState.wrap12 = sourceState.mShaderState.mRenderState.wrap12;
		}
		if (sourceState.hasWrap13 && (targetState.hasWrap13 || !onlyIfExists)) {
			targetState.hasWrap13 = true; 
			targetState.mShaderState.mRenderState.wrap13 = sourceState.mShaderState.mRenderState.wrap13;
		}
		if (sourceState.hasWrap14 && (targetState.hasWrap14 || !onlyIfExists)) {
			targetState.hasWrap14 = true; 
			targetState.mShaderState.mRenderState.wrap14 = sourceState.mShaderState.mRenderState.wrap14;
		}
		if (sourceState.hasWrap15 && (targetState.hasWrap15 || !onlyIfExists)) {
			targetState.hasWrap15 = true;  
			targetState.mShaderState.mRenderState.wrap15 = sourceState.mShaderState.mRenderState.wrap15;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.localViewer = sourceState.mShaderState.mRenderState.localViewer;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.emissiveMaterialSource = sourceState.mShaderState.mRenderState.emissiveMaterialSource;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.ambientMaterialSource = sourceState.mShaderState.mRenderState.ambientMaterialSource;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true;  
			targetState.mShaderState.mRenderState.diffuseMaterialSource = sourceState.mShaderState.mRenderState.diffuseMaterialSource;
		}
		if (sourceState.wasFixedFunctionLightingGroupModified && (targetState.wasFixedFunctionLightingGroupModified || !onlyIfExists)) {
			targetState.wasFixedFunctionLightingGroupModified = true; 
			targetState.mShaderState.mRenderState.specularMaterialSource = sourceState.mShaderState.mRenderState.specularMaterialSource;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true; 
			targetState.mShaderState.mRenderState.colorWriteEnable = sourceState.mShaderState.mRenderState.colorWriteEnable;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true; 
			targetState.mShaderState.mRenderState.blendOperation = sourceState.mShaderState.mRenderState.blendOperation;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true; 
			targetState.mShaderState.mRenderState.scissorTestEnable = sourceState.mShaderState.mRenderState.scissorTestEnable;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.slopeScaleDepthBias = sourceState.mShaderState.mRenderState.slopeScaleDepthBias;
		}
		if (sourceState.wasRasterizerGroupModified && (targetState.wasRasterizerGroupModified || !onlyIfExists)) {
			targetState.wasRasterizerGroupModified = true;  
			targetState.mShaderState.mRenderState.antiAliasedLineEnable = sourceState.mShaderState.mRenderState.antiAliasedLineEnable;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.twoSidedStencilMode = sourceState.mShaderState.mRenderState.twoSidedStencilMode;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.ccwStencilFail = sourceState.mShaderState.mRenderState.ccwStencilFail;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.ccwStencilZFail = sourceState.mShaderState.mRenderState.ccwStencilZFail;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true;  
			targetState.mShaderState.mRenderState.ccwStencilPass = sourceState.mShaderState.mRenderState.ccwStencilPass;
		}
		if (sourceState.wasDsaGroupModified && (targetState.wasDsaGroupModified || !onlyIfExists)) {
			targetState.wasDsaGroupModified = true; 
			targetState.mShaderState.mRenderState.ccwStencilFunction = sourceState.mShaderState.mRenderState.ccwStencilFunction;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true; 
			targetState.mShaderState.mRenderState.colorWriteEnable1 = sourceState.mShaderState.mRenderState.colorWriteEnable1;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true; 
			targetState.mShaderState.mRenderState.colorWriteEnable2 = sourceState.mShaderState.mRenderState.colorWriteEnable2;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.colorWriteEnable3 = sourceState.mShaderState.mRenderState.colorWriteEnable3;
		}
		if (sourceState.hasBlendFactor && (targetState.hasBlendFactor || !onlyIfExists)) {
			targetState.hasBlendFactor = true;  
			targetState.mShaderState.mRenderState.blendFactor = sourceState.mShaderState.mRenderState.blendFactor;
		}
		if (sourceState.hasSrgbWriteEnable && (targetState.hasSrgbWriteEnable || !onlyIfExists)) {
			targetState.hasSrgbWriteEnable = true;  
			targetState.mShaderState.mRenderState.srgbWriteEnable = sourceState.mShaderState.mRenderState.srgbWriteEnable;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true; 
			targetState.mShaderState.mRenderState.separateAlphaBlendEnable = sourceState.mShaderState.mRenderState.separateAlphaBlendEnable;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.sourceBlendAlpha = sourceState.mShaderState.mRenderState.sourceBlendAlpha;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.destinationBlendAlpha = sourceState.mShaderState.mRenderState.destinationBlendAlpha;
		}
		if (sourceState.wasBlendGroupModified && (targetState.wasBlendGroupModified || !onlyIfExists)) {
			targetState.wasBlendGroupModified = true;  
			targetState.mShaderState.mRenderState.blendOperationAlpha = sourceState.mShaderState.mRenderState.blendOperationAlpha;
		}
	}

	//IDirect3DDevice9::SetSamplerState
	if (sourceState.mSamplerStates.size())
	{
		for(const auto& pair1 : sourceState.mSamplerStates)
		{
			for(const auto& pair2 : pair1.second)
			{
				if (!onlyIfExists || (targetState.mSamplerStates.count(pair1.first) > 0 && targetState.mSamplerStates[pair1.first].count(pair2.first) > 0))
				{
					if
						(
						(type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD) ||
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
	if ((sourceState.m9Scissor.right != 0 || sourceState.m9Scissor.left != 0) && (!onlyIfExists || targetState.mHasIndexBuffer) && (type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD))
	{
		targetState.m9Scissor = sourceState.m9Scissor;
		targetState.mScissor = sourceState.mScissor;
	}

	//IDirect3DDevice9::SetStreamSource
	if (sourceState.mStreamSources.size())
	{
		for(const auto& pair1 : sourceState.mStreamSources)
		{
			if ((type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD) && (!onlyIfExists || targetState.mStreamSources.count(pair1.first) > 0))
			{
				targetState.mStreamSources[pair1.first] = pair1.second;
			}
		}
	}

	//IDirect3DDevice9::SetStreamSourceFreq
	//IDirect3DDevice9::SetTexture
	//for(const auto& pair1 : sourceState.mTextures)
	//{
	//	if ((type == D3DSBT_ALL))
	//	{
	//		targetState.mTextures[pair1.first] = pair1.second;
	//	}
	//}

	for (size_t i = 0; i < 16; i++)
	{
		if ((type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD))
		{
			targetState.mTextures[i] = sourceState.mTextures[i];
		}
	}

	//IDirect3DDevice9::SetTextureStageState
	if (type == D3DSBT_VERTEXSTATE || type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD)
	{
		targetState.mAreTextureStagesDirty = true;

		for (size_t i = 0; i < 9; i++)
		{
			targetState.mShaderState.mTextureStages[i].texureCoordinateIndex = sourceState.mShaderState.mTextureStages[i].texureCoordinateIndex;
			targetState.mShaderState.mTextureStages[i].textureTransformationFlags = sourceState.mShaderState.mTextureStages[i].textureTransformationFlags;
		}
	}

	if (type == D3DSBT_PIXELSTATE || type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD)
	{
		targetState.mAreTextureStagesDirty = true;

		for (size_t i = 0; i < 9; i++)
		{
			targetState.mShaderState.mTextureStages[i].colorOperation = sourceState.mShaderState.mTextureStages[i].colorOperation;
			targetState.mShaderState.mTextureStages[i].colorArgument1 = sourceState.mShaderState.mTextureStages[i].colorArgument1;
			targetState.mShaderState.mTextureStages[i].colorArgument2 = sourceState.mShaderState.mTextureStages[i].colorArgument2;
			targetState.mShaderState.mTextureStages[i].alphaOperation = sourceState.mShaderState.mTextureStages[i].alphaOperation;
			targetState.mShaderState.mTextureStages[i].alphaArgument1 = sourceState.mShaderState.mTextureStages[i].alphaArgument1;
			targetState.mShaderState.mTextureStages[i].alphaArgument2 = sourceState.mShaderState.mTextureStages[i].alphaArgument2;
			targetState.mShaderState.mTextureStages[i].bumpMapMatrix00 = sourceState.mShaderState.mTextureStages[i].bumpMapMatrix00;
			targetState.mShaderState.mTextureStages[i].bumpMapMatrix01 = sourceState.mShaderState.mTextureStages[i].bumpMapMatrix01;
			targetState.mShaderState.mTextureStages[i].bumpMapMatrix10 = sourceState.mShaderState.mTextureStages[i].bumpMapMatrix10;
			targetState.mShaderState.mTextureStages[i].bumpMapMatrix11 = sourceState.mShaderState.mTextureStages[i].bumpMapMatrix11;
			targetState.mShaderState.mTextureStages[i].texureCoordinateIndex = sourceState.mShaderState.mTextureStages[i].texureCoordinateIndex;
			targetState.mShaderState.mTextureStages[i].bumpMapScale = sourceState.mShaderState.mTextureStages[i].bumpMapScale;
			targetState.mShaderState.mTextureStages[i].bumpMapOffset = sourceState.mShaderState.mTextureStages[i].bumpMapOffset;
			targetState.mShaderState.mTextureStages[i].textureTransformationFlags = sourceState.mShaderState.mTextureStages[i].textureTransformationFlags;
			targetState.mShaderState.mTextureStages[i].colorArgument0 = sourceState.mShaderState.mTextureStages[i].colorArgument0;
			targetState.mShaderState.mTextureStages[i].alphaArgument0 = sourceState.mShaderState.mTextureStages[i].alphaArgument0;
			targetState.mShaderState.mTextureStages[i].Result = sourceState.mShaderState.mTextureStages[i].Result;
		}
	}

	//IDirect3DDevice9::SetTransform
	if (type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD)
	{
		for(const auto& pair1 : sourceState.mTransforms)
		{
			if (!onlyIfExists || targetState.mTransforms.count(pair1.first) > 0)
			{
				targetState.mTransforms[pair1.first] = pair1.second;
			}
		}
	}

	//IDirect3DDevice9::SetViewport
	if ((sourceState.m9Viewport.Width != 0) && (!onlyIfExists || targetState.m9Viewport.Width != 0) && (type == D3DSBT_ALL || type == D3DSBT_FORCE_DWORD))
	{
		targetState.m9Viewport = sourceState.m9Viewport;
		targetState.mViewport = sourceState.mViewport;
	}

	//IDirect3DDevice9::SetVertexShader
	if (sourceState.mHasVertexShader && (!onlyIfExists || targetState.mHasVertexShader) && (type == D3DSBT_ALL || type == D3DSBT_VERTEXSTATE || type == D3DSBT_FORCE_DWORD))
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

HMODULE GetModule(HMODULE module)
{
	static HMODULE dllModule = 0;

	if (module != 0)
	{
		dllModule = module;
	}

	return dllModule;
}

vk::ShaderModule LoadShaderFromFile(vk::Device device, const char *filename)
{
	vk::ShaderModuleCreateInfo moduleCreateInfo;
	vk::ShaderModule module;
	vk::Result result;
	FILE *fp = fopen(filename, "rb");
	if (fp != nullptr)
	{
		fseek(fp, 0L, SEEK_END);
		int32_t dataSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		void* data = (uint32_t*)malloc(dataSize);
		if (data != nullptr && fread(data, dataSize, 1, fp))
		{
			moduleCreateInfo.codeSize = dataSize;
			moduleCreateInfo.pCode = (uint32_t*)data; //Why is this uint32_t* if the size is in bytes?

			result = device.createShaderModule(&moduleCreateInfo, nullptr, &module);
			if (result != vk::Result::eSuccess)
			{
				BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromFile vkCreateShaderModule failed with return code of " << GetResultString((VkResult)result);
			}
			else
			{
				BOOST_LOG_TRIVIAL(info) << "LoadShaderFromFile vkCreateShaderModule succeeded. " << filename;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromFile unable to read file. " << filename;
		}
		free(data);
		fclose(fp);
	}
	else
	{
		BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromFile unable to open file. " << filename;
	}

	return module;
}

vk::ShaderModule LoadShaderFromResource(vk::Device device, WORD resource)
{
	vk::ShaderModuleCreateInfo moduleCreateInfo;
	vk::ShaderModule module;
	vk::Result result;
	HMODULE dllModule = NULL;

	//dllModule = GetModule();
	BOOL res = GetModuleHandleEx(0, TEXT("d3d9"), &dllModule);

	if (res == FALSE)
	{
		BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource dllModule is null.";
	}
	else
	{
		HRSRC hRes = FindResource(dllModule, MAKEINTRESOURCE(resource), TEXT("Shader"));
		if (NULL != hRes)
		{
			HGLOBAL hData = LoadResource(dllModule, hRes);
			if (NULL != hData)
			{
				int32_t dataSize = SizeofResource(dllModule, hRes);
				uint32_t* data = (uint32_t*)LockResource(hData);

				moduleCreateInfo.codeSize = dataSize;
				moduleCreateInfo.pCode = data; //Why is this uint32_t* if the size is in bytes?

				result = device.createShaderModule(&moduleCreateInfo, nullptr, &module);
				if (result != vk::Result::eSuccess)
				{
					BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource vkCreateShaderModule failed with return code of " << GetResultString((VkResult)result);
				}
				else
				{
					BOOST_LOG_TRIVIAL(info) << "LoadShaderFromResource vkCreateShaderModule succeeded.";
				}
			}
			else
			{
				BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource resource data is null.";
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(fatal) << "LoadShaderFromResource resource not found.";
		}

		FreeLibrary(dllModule);
	}

	return module;
}

void ReallyCopyImage(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::Image dstImage, int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t depth, uint32_t srcMip, uint32_t dstMip, uint32_t srcLayer, uint32_t dstLayer)
{
	//vk::Result result;

	vk::ImageSubresourceLayers subResource1;
	subResource1.aspectMask = vk::ImageAspectFlagBits::eColor;
	subResource1.baseArrayLayer = srcLayer;
	subResource1.mipLevel = srcMip;
	subResource1.layerCount = 1;

	vk::ImageSubresourceLayers subResource2;
	subResource2.aspectMask = vk::ImageAspectFlagBits::eColor;
	subResource2.baseArrayLayer = dstLayer;
	subResource2.mipLevel = dstMip;
	subResource2.layerCount = 1;

	//vk::ImageCopy region;
	//region.srcSubresource = subResource1;
	//region.dstSubresource = subResource2;
	//region.srcOffset = vk::Offset3D(x, y, 0);
	//region.dstOffset = vk::Offset3D(x, y, 0);
	//region.extent.width = width;
	//region.extent.height = height;
	//region.extent.depth = depth;

	//commandBuffer.copyImage(
	//	srcImage, vk::ImageLayout::eTransferSrcOptimal,
	//	dstImage, vk::ImageLayout::eTransferDstOptimal,
	//	1, &region);

	vk::ImageBlit region;
	region.srcSubresource = subResource1;
	region.dstSubresource = subResource2;
	region.srcOffsets[1] = vk::Offset3D(width, height, depth);
	region.dstOffsets[1] = vk::Offset3D(width, height, depth);

	commandBuffer.blitImage(
		srcImage, vk::ImageLayout::eTransferSrcOptimal,
		dstImage, vk::ImageLayout::eTransferDstOptimal,
		1, &region, vk::Filter::eLinear);
}

void ReallySetImageLayout(vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, uint32_t levelCount, uint32_t mipIndex, uint32_t layerCount)
{
	//VkResult result;
	vk::PipelineStageFlags sourceStages; //VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
	vk::PipelineStageFlags destinationStages; //VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT

	if (aspectMask == vk::ImageAspectFlags()) //0
	{
		aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	vk::ImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = mipIndex;
	imageMemoryBarrier.subresourceRange.levelCount = levelCount;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	switch (oldImageLayout)
	{
	case vk::ImageLayout::eUndefined:
		break;
	case vk::ImageLayout::eGeneral:
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		sourceStages |= vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eColorAttachmentOptimal:
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite; //Added based on validation layer complaints.
		sourceStages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		break;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
		break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		break;
	case vk::ImageLayout::eTransferSrcOptimal:
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		sourceStages  |= vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		sourceStages |= vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::ePreinitialized:
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		sourceStages |= vk::PipelineStageFlagBits::eHost;
		break;
	case vk::ImageLayout::ePresentSrcKHR:
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
		break;
	default:
		break;
	}

	switch (newImageLayout)
	{
	case vk::ImageLayout::eUndefined:
		break;
	case vk::ImageLayout::eGeneral:
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
		destinationStages |= vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eColorAttachmentOptimal:
		imageMemoryBarrier.srcAccessMask |= vk::AccessFlagBits::eMemoryRead;
		imageMemoryBarrier.dstAccessMask |= vk::AccessFlagBits::eColorAttachmentWrite;
		destinationStages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		imageMemoryBarrier.dstAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		destinationStages |= vk::PipelineStageFlagBits::eLateFragmentTests;
		break;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
		break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		imageMemoryBarrier.dstAccessMask |= vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eInputAttachmentRead;
		destinationStages |= vk::PipelineStageFlagBits::eFragmentShader;
		break;
	case vk::ImageLayout::eTransferSrcOptimal:
		imageMemoryBarrier.dstAccessMask |= vk::AccessFlagBits::eTransferRead;
		destinationStages |= vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
		imageMemoryBarrier.dstAccessMask |= vk::AccessFlagBits::eTransferWrite;
		destinationStages |= vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::ePreinitialized:
		imageMemoryBarrier.dstAccessMask |= vk::AccessFlagBits::eHostWrite;
		destinationStages |= vk::PipelineStageFlagBits::eHost;
		break;
	case vk::ImageLayout::ePresentSrcKHR:
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
		break;
	default:
		break;
	}

	if (sourceStages == vk::PipelineStageFlags())
	{
		sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
	}

	if (destinationStages == vk::PipelineStageFlags())
	{
		destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
	}

	commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void IUnknownDeleter(IUnknown* ptr) 
{ 
	if (ptr != nullptr) 
	{ 
		ptr->Release(); 
	} 
}