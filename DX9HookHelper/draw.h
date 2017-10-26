#pragma once

struct iPosDim
{
	int x, y, w, h;
};

struct CUSTOM_D3DVERTEX
{
	float x, y, z, rhw;
	DWORD dwColour;
};

class Draw
{
public:
	static void FilledRect(LPDIRECT3DDEVICE9 pDevice, int x, int y, int w, int h, D3DCOLOR color)
	{
		D3DRECT rect = { x, y, x + w, y + h };
		pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0.0f, 0);
	}

	static void CreateFont(LPDIRECT3DDEVICE9 pDevice, int iFontSize, LPD3DXFONT & pFont)
	{
		D3DXCreateFont(pDevice, iFontSize, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &pFont);
	}

	static void DrawText(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT & pFont, int iFontSize, int x, int y, D3DCOLOR color, const TCHAR * text)
	{
		if (!pFont)
			CreateFont(pDevice, iFontSize, pFont);
		RECT rect;
		SetRect(&rect, x, y, x, y);
		pFont->DrawText(NULL, text, -1, &rect, DT_NOCLIP | DT_LEFT, color);
	}

	struct LVertex
	{
		FLOAT    x, y, z;
		D3DCOLOR specular, diffuse;
	};

	static const DWORD VertexFVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE |
		D3DFVF_SPECULAR);

	static void RectUP(LPDIRECT3DDEVICE9 pDevice, float x, float y, float w, float h, D3DCOLOR color)
	{
		IDirect3DVertexBuffer9 * pBigSquareVB;
		pDevice->CreateVertexBuffer(4 * sizeof(LVertex),
			D3DUSAGE_WRITEONLY, VertexFVF, D3DPOOL_DEFAULT, &pBigSquareVB, NULL);

		LVertex * v;
		pBigSquareVB->Lock(0, 0, (void**)&v, 0);

		v[0].x = 0.0f;  v[0].y = 10.0;  v[0].z = -1.0f;
		v[0].diffuse = 0xffff0000;
		v[0].specular = 0xff00ff00;

		v[1].x = 0.0f;  v[1].y = 0.0f;  v[1].z = -1.0f;
		v[1].diffuse = 0xff00ff00;
		v[1].specular = 0xff00ffff;

		v[2].x = 10.0f; v[2].y = 10.0f; v[2].z = -1.0f;
		v[2].diffuse = 0xffff00ff;
		v[2].specular = 0xff000000;

		v[3].x = 0.0f; v[3].y = 10.0f;  v[3].z = -1.0f;
		v[3].diffuse = 0xffffff00;
		v[3].specular = 0xffff0000;

		pBigSquareVB->Unlock();

		pDevice->SetFVF(VertexFVF);
		pDevice->SetStreamSource(0, pBigSquareVB, 0, sizeof(LVertex));
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//CUSTOM_D3DVERTEX verts[3];
		//verts[0] = { x, y, 0.0f, 1.0f, color };
		//verts[1] = { x + w, y + h, 0.0f, 1.0f, color };
		//verts[2] = { (x + w) / 2, y, 0.0f, 1.0f, color };
		//pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);
		//pDevice->DrawPrimitiveUP(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST, 1, verts, sizeof(verts));
	}
};