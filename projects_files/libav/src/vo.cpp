/*
 * Copyright (c) 2013 Ced2911
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <xtl.h>
#include <stdio.h>
#include "shader.h"
#include "player.h"

//--------------------------------------------------------------------------------------
// Globals
//-------------------------------------------------------------------------------------
static IDirect3D9*         g_pD3D = NULL; // Used to create the D3DDevice
static IDirect3DDevice9*   g_pd3dDevice = NULL; // the rendering device
static D3DPRESENT_PARAMETERS d3dpp;

static IDirect3DVertexShader9* pVertexShader;
static IDirect3DPixelShader9* pPixelShader;

typedef struct
{
    float Position[3];
    float Uv[2];
} vertices_t;

vertices_t Vertices[] =
{
    { 
		1, 1, 0, 
		0, 0,
	},
	{ 
		1, -1, 0, 
		0, 1,
	},
	{ 
		-1, 1, 0, 
		1, 0,
	}
};

// Define the vertex elements.
static const D3DVERTEXELEMENT9 VertexElements[3] =
{
    { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

IDirect3DVertexDeclaration9* pVertexDecl;

//-------------------------------------------------------------------------------------
// Surface displayed on screen (updated each frame)
//-------------------------------------------------------------------------------------
IDirect3DTexture9 * pFrameU = NULL;
IDirect3DTexture9 * pFrameV = NULL;
IDirect3DTexture9 * pFrameY = NULL;

HRESULT InitYuvSurface(
	D3DTexture **pFrameY,D3DTexture **pFrameU,D3DTexture **pFrameV,
	int width, int height
){
	D3DXCreateTexture(
		g_pd3dDevice, width,
		height, D3DX_DEFAULT, 0, 
		D3DFMT_LIN_L8, D3DPOOL_MANAGED,
		pFrameY
	);
	D3DXCreateTexture(
		g_pd3dDevice, width/2,
		height/2, D3DX_DEFAULT, 0, 
		D3DFMT_LIN_L8, D3DPOOL_MANAGED,
		pFrameU
	);
	D3DXCreateTexture(
		g_pd3dDevice, width/2,
		height/2, D3DX_DEFAULT, 0, 
		D3DFMT_LIN_L8, D3DPOOL_MANAGED,
		pFrameV
	);

	if(pFrameV==NULL || pFrameU==NULL || pFrameY == NULL){
		return E_FAIL;
	}
	return S_OK;
};


static void compile_shaders() {
	// Buffers to hold compiled shaders and possible error messages
    ID3DXBuffer* pShaderCode = NULL;
    ID3DXBuffer* pErrorMsg = NULL;

    // Compile vertex shader.
    HRESULT hr = D3DXCompileShader( shader_vertex_common, ( UINT )strlen( shader_vertex_common ),
                                    NULL, NULL, "main", "vs_2_0", 0,
                                    &pShaderCode, &pErrorMsg, NULL );
    if( FAILED( hr ) )
    {
        DebugBreak();
        exit( 1 );
    }

    g_pd3dDevice->CreateVertexShader( ( DWORD* )pShaderCode->GetBufferPointer(),
                                      &pVertexShader );

    // Shader code is no longer required.
    pShaderCode->Release();
    pShaderCode = NULL;

    // Compile pixel shader.
	/*
    hr = D3DXCompileShader( shader_pixel_rgb, ( UINT )strlen( shader_pixel_rgb ),
                            NULL, NULL, "main", "ps_2_0", 0,
                            &pShaderCode, &pErrorMsg, NULL );
							*/
	hr = D3DXCompileShader( shader_pixel_yuv, ( UINT )strlen( shader_pixel_yuv ),
                            NULL, NULL, "main", "ps_2_0", 0,
                            &pShaderCode, &pErrorMsg, NULL );
    if( FAILED( hr ) )
    {
        DebugBreak();
        exit( 1 );
    }

    // Create pixel shader.
    g_pd3dDevice->CreatePixelShader( ( DWORD* )pShaderCode->GetBufferPointer(),
                                     &pPixelShader );

    // Shader code no longer required.
    pShaderCode->Release();
    pShaderCode = NULL;
}

void vo_init(int width, int height) {
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	// Set up the structure used to create the D3DDevice.
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.BackBufferWidth = 1280;
    d3dpp.BackBufferHeight = 720;
    d3dpp.BackBufferFormat =  ( D3DFORMAT )MAKESRGBFMT( D3DFMT_A8R8G8B8 );
    d3dpp.FrontBufferFormat = ( D3DFORMAT )MAKESRGBFMT( D3DFMT_LE_X8R8G8B8 );
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.BackBufferCount = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    // d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	IDirect3D9_CreateDevice(g_pD3D, 0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &d3dpp, &g_pd3dDevice);

	// Create shaders
	compile_shaders();

	// Create vb
    IDirect3DDevice9_CreateVertexDeclaration(g_pd3dDevice, VertexElements, &pVertexDecl);

	// Create yuv surf
	InitYuvSurface(&pFrameY, &pFrameU, &pFrameV, width, height);
}

void vo_update(AVFrame * pFrame) {
	// Refresh texture frame
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);
	g_pd3dDevice->SetTexture(2,NULL);

	AVPicture pict;
	memset(&pict, 0, sizeof(pict));

	//Lock texture to access data
	D3DLOCKED_RECT lockRectY;
	D3DLOCKED_RECT lockRectU;
	D3DLOCKED_RECT lockRectV;

	pFrameY->LockRect( 0, &lockRectY, NULL, 0 );
	pFrameU->LockRect( 0, &lockRectU, NULL, 0 );
	pFrameV->LockRect( 0, &lockRectV, NULL, 0 );

	pict.data[0] = (uint8_t*)lockRectY.pBits;
	pict.data[1] = (uint8_t*)lockRectU.pBits;
	pict.data[2] = (uint8_t*)lockRectV.pBits;

	pict.linesize[0] = lockRectY.Pitch;
	pict.linesize[1] = lockRectU.Pitch;
	pict.linesize[2] = lockRectV.Pitch;

	//Scale it ..
	sws_scale(
		player_context.sws_context, pFrame->data, 
		pFrame->linesize, 0, 
		player_context.video_stream->codec->height, 
		pict.data, pict.linesize
	);

	//Release lock on texture
	pFrameY->UnlockRect(0);
	pFrameU->UnlockRect(0);
	pFrameV->UnlockRect(0);


	// Display
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                            0xff000000, 1.0f, 0L );

    g_pd3dDevice->SetVertexShader( pVertexShader );
    g_pd3dDevice->SetPixelShader( pPixelShader );
    g_pd3dDevice->SetVertexDeclaration( pVertexDecl );

	g_pd3dDevice->SetTexture(0, pFrameY);
	g_pd3dDevice->SetTexture(1, pFrameU);
	g_pd3dDevice->SetTexture(2, pFrameV);

	g_pd3dDevice->DrawPrimitiveUP( D3DPT_RECTLIST, 1, Vertices, sizeof( vertices_t ) );
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}