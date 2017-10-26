#pragma once
#define IdxEndScene 0x2a
#define IdxGetNPatchMode 0x50
#define IdxSetVertexShaderConstantF 0x5e

enum d3dDeviceFuncs
{
	EndScene = 0x2a,
	GetNPatchNode = 0x50,
	DrawPrimitive,
	DrawIndexedPrimitive,
	DrawPrimitiveUP,
	DrawIndexedPrimitiveUP,
	ProcessVertices,
	CreateVertexDeclaration,
	SetVertexDeclaration,
	GetVertexDeclaration,
	SetFVF_FP,
	GetFVF,
	CreateVertexShader,
	SetVertexShader,
	GetVertexShader,
	SetVertexShaderConstantF,
	GetVertexShaderConstantF,
	SetVertexShaderConstantI,
	GetVertexShaderConstantI,
	SetVertexShaderConstantB,
	GetVertexShaderConstantB,
	SetStreamSource,
	GetStreamSource,
	SetStreamSourceFreq,
	GetStreamSourceFreq,
	SetIndices,
	GetIndices,
	CreatePixelShader,
	SetPixelShader,
	GetPixelShader,
	SetPixelShaderConstantF,
	GetPixelShaderConstantF,
	SetPixelShaderConstantI,
	GetPixelShaderConstantI,
	SetPixelShaderConstantB,
	GetPixelShaderConstantB,
	DrawRectPatch,
	DrawTriPatch,
	DeletePatch,
	CreateQuery
};