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

#pragma once

/**
*	Simple RGB Shader
**/
const char * shader_vertex_common =
	"                                              "
    " struct VS_IN                                 "
    "                                              "
    " {                                            "
    "     float4 ObjPos   : POSITION;              "
    "     float2 Uv    : TEXCOORD0;                "
    " };                                           "
    "                                              "
    " struct VS_OUT                                "
    " {                                            "
    "     float4 ProjPos  : POSITION;              "
    "     float2 Uv    : TEXCOORD0;                "
    " };                                           "
    "                                              "
    " VS_OUT main( VS_IN In )                      "
    " {                                            "
    "     VS_OUT Out;                              "
    "     Out.ProjPos = In.ObjPos;				   " 
    "     Out.Uv = In.Uv;						   " 
    "     return Out;                              " 
    " }                                            ";


//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------
const char * shader_pixel_rgb =
	" sampler s : register(s0);                    "
	"											   "
    " struct PS_IN                                 "
    " {                                            "
    "     float2 Uv : TEXCOORD0;	               " 
    " };                                           " 
    "                                              "
    " float4 main( PS_IN In ) : COLOR              "
    " {                                            "
	"     return tex2D(s, In.Uv);                  " 
    " }                                            ";


//-------------------------------------------------------------------------------------
// Pixel shader
//-------------------------------------------------------------------------------------
const char* shader_pixel_yuv = 
" sampler2D  YTexture : register( s0 );			"
" sampler2D  UTexture : register( s1 );			"
" sampler2D  VTexture : register( s2 );			"
" struct PS_IN                                 "
" {                                            "
"     float2 Uv : TEXCOORD0;                    "  // Interpolated color from                      
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            " 
"												"
"		float4 Y_4D = tex2D( YTexture, In.Uv );  "
"		float4 U_4D = tex2D( UTexture, In.Uv );  "
"		float4 V_4D = tex2D( VTexture, In.Uv );  "
"                                             "
"		float R = 1.164 * ( Y_4D.r - 0.0625 ) + 1.596 * ( V_4D.r - 0.5 ); "
"		float G = 1.164 * ( Y_4D.r - 0.0625 ) - 0.391 * ( U_4D.r - 0.5 ) - 0.813 * ( V_4D.r - 0.5 ); "
"		float B = 1.164 * ( Y_4D.r - 0.0625 ) + 2.018 * ( U_4D.r - 0.5 );          "                
"                            "                 
"		float4 ARGB;     "                        
"		ARGB.a = 1.0;     "                       
"		ARGB.r = R;        "                      
"		ARGB.g = G;         "                     
"		ARGB.b = B;          "                    
"                            "                 
"		return ARGB;  "
//" return float4(1,1,1,1);"
"					"		
" }                                            "; 