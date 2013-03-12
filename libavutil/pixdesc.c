/*
 * pixel format descriptor
 * Copyright (c) 2009 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "pixfmt.h"
#include "pixdesc.h"

#include "intreadwrite.h"

void av_read_image_line(uint16_t *dst,
                        const uint8_t *data[4], const int linesize[4],
                        const AVPixFmtDescriptor *desc,
                        int x, int y, int c, int w,
                        int read_pal_component)
{
    AVComponentDescriptor comp = desc->comp[c];
    int plane = comp.plane;
    int depth = comp.depth_minus1 + 1;
    int mask  = (1 << depth) - 1;
    int shift = comp.shift;
    int step  = comp.step_minus1 + 1;
    int flags = desc->flags;

    if (flags & PIX_FMT_BITSTREAM) {
        int skip = x * step + comp.offset_plus1 - 1;
        const uint8_t *p = data[plane] + y * linesize[plane] + (skip >> 3);
        int shift = 8 - depth - (skip & 7);

        while (w--) {
            int val = (*p >> shift) & mask;
            if (read_pal_component)
                val = data[1][4*val + c];
            shift -= step;
            p -= shift >> 3;
            shift &= 7;
            *dst++ = val;
        }
    } else {
        const uint8_t *p = data[plane] + y * linesize[plane] +
                           x * step + comp.offset_plus1 - 1;
        int is_8bit = shift + depth <= 8;

        if (is_8bit)
            p += !!(flags & PIX_FMT_BE);

        while (w--) {
            int val = is_8bit ? *p :
                flags & PIX_FMT_BE ? AV_RB16(p) : AV_RL16(p);
            val = (val >> shift) & mask;
            if (read_pal_component)
                val = data[1][4 * val + c];
            p += step;
            *dst++ = val;
        }
    }
}

void av_write_image_line(const uint16_t *src,
                         uint8_t *data[4], const int linesize[4],
                         const AVPixFmtDescriptor *desc,
                         int x, int y, int c, int w)
{
    AVComponentDescriptor comp = desc->comp[c];
    int plane = comp.plane;
    int depth = comp.depth_minus1 + 1;
    int step  = comp.step_minus1 + 1;
    int flags = desc->flags;

    if (flags & PIX_FMT_BITSTREAM) {
        int skip = x * step + comp.offset_plus1 - 1;
        uint8_t *p = data[plane] + y * linesize[plane] + (skip >> 3);
        int shift = 8 - depth - (skip & 7);

        while (w--) {
            *p |= *src++ << shift;
            shift -= step;
            p -= shift >> 3;
            shift &= 7;
        }
    } else {
        int shift = comp.shift;
        uint8_t *p = data[plane] + y * linesize[plane] +
                     x * step + comp.offset_plus1 - 1;

        if (shift + depth <= 8) {
            p += !!(flags & PIX_FMT_BE);
            while (w--) {
                *p |= (*src++ << shift);
                p += step;
            }
        } else {
            while (w--) {
                if (flags & PIX_FMT_BE) {
                    uint16_t val = AV_RB16(p) | (*src++ << shift);
                    AV_WB16(p, val);
                } else {
                    uint16_t val = AV_RL16(p) | (*src++ << shift);
                    AV_WL16(p, val);
                }
                p += step;
            }
        }
    }
}

#if !FF_API_PIX_FMT_DESC
static
#endif
const AVPixFmtDescriptor av_pix_fmt_descriptors[AV_PIX_FMT_NB] = {
    {
        "yuv420p",
        3,
        1,
        1,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuyv422",
        3,
        1,
        0,
        0, {
            { 0, 1, 1, 0, 7 },        /* Y */
            { 0, 3, 2, 0, 7 },        /* U */
            { 0, 3, 4, 0, 7 },        /* V */
        },
    },
    {
        "rgb24",
        3,
        0,
        0,
        32,
        {
            { 0, 2, 1, 0, 7 },        /* R */
            { 0, 2, 2, 0, 7 },        /* G */
            { 0, 2, 3, 0, 7 },        /* B */
        },
    },
    {
        "bgr24",
        3,
        0,
        0,
        32,
        {
            { 0, 2, 1, 0, 7 },        /* B */
            { 0, 2, 2, 0, 7 },        /* G */
            { 0, 2, 3, 0, 7 },        /* R */
        },
    },
    {
        "yuv422p",
        3,
        1,
        0,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuv444p",
        3,
        0,
        0,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuv410p",
        3,
        2,
        2,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuv411p",
        3,
        2,
        0,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "gray",
        1,
        0,
        0,
        0, {
            { 0, 0, 1, 0, 7 },        /* Y */
        },
    },
    {
        "monow",
        1,
        0,
        0,
        4,
        {
            { 0, 0, 1, 0, 0 },        /* Y */
        },
    },
    {
        "monob",
        1,
        0,
        0,
        4,
        {
            { 0, 0, 1, 7, 0 },        /* Y */
        },
    },
    {
        "pal8",
        1,
        0,
        0,
        2,
        {
            { 0, 0, 1, 0, 7 },
        },
    },
    {
        "yuvj420p",
        3,
        1,
        1,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuvj422p",
        3,
        1,
        0,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuvj444p",
        3,
        0,
        0,
        16,
        {
            {0, 0, 1, 0, 7},        /* Y */
            {1, 0, 1, 0, 7},        /* U */
            {2, 0, 1, 0, 7},        /* V */
        },
    },
    {
        "xvmcmc",
        0, 0, 0, 8,
    },
    {
        "xvmcidct",
        0, 0, 0, 8,
    },
    {
        "uyvy422",
        3,
        1,
        0,
        0, {
            { 0, 1, 2, 0, 7 },        /* Y */
            { 0, 3, 1, 0, 7 },        /* U */
            { 0, 3, 3, 0, 7 },        /* V */
        },
    },
    {
        "uyyvyy411",
        3,
        2,
        0,
        0, {
            { 0, 3, 2, 0, 7 },        /* Y */
            { 0, 5, 1, 0, 7 },        /* U */
            { 0, 5, 4, 0, 7 },        /* V */
        },
    },
    {
        "bgr8",
        3,
        0,
        0,
        32 | 64,
        {
            { 0, 0, 1, 6, 1 },        /* B */
            { 0, 0, 1, 3, 2 },        /* G */
            { 0, 0, 1, 0, 2 },        /* R */
        },
    },
    {
        "bgr4",
        3,
        0,
        0,
        4 | 32,
        {
            { 0, 3, 1, 0, 0 },        /* B */
            { 0, 3, 2, 0, 1 },        /* G */
            { 0, 3, 4, 0, 0 },        /* R */
        },
    },
    {
        "bgr4_byte",
        3,
        0,
        0,
        32 | 64,
        {
            { 0, 0, 1, 3, 0 },        /* B */
            { 0, 0, 1, 1, 1 },        /* G */
            { 0, 0, 1, 0, 0 },        /* R */
        },
    },
    {
        "rgb8",
        3,
        0,
        0,
        32 | 64,
        {
            { 0, 0, 1, 6, 1 },        /* R */
            { 0, 0, 1, 3, 2 },        /* G */
            { 0, 0, 1, 0, 2 },        /* B */
        },
    },
    {
        "rgb4",
        3,
        0,
        0,
        4 | 32,
        {
            { 0, 3, 1, 0, 0 },        /* R */
            { 0, 3, 2, 0, 1 },        /* G */
            { 0, 3, 4, 0, 0 },        /* B */
        },
    },
    {
        "rgb4_byte",
        3,
        0,
        0,
        32 | 64,
        {
            { 0, 0, 1, 3, 0 },        /* R */
            { 0, 0, 1, 1, 1 },        /* G */
            { 0, 0, 1, 0, 0 },        /* B */
        },
    },
    {
        "nv12",
        3,
        1,
        1,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 1, 1, 0, 7 },        /* U */
            { 1, 1, 2, 0, 7 },        /* V */
        },
    },
    {
        "nv21",
        3,
        1,
        1,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 1, 1, 0, 7 },        /* V */
            { 1, 1, 2, 0, 7 },        /* U */
        },
    },
    {
        "argb",
        4,
        0,
        0,
        32 | 128,
        {
            { 0, 3, 1, 0, 7 },        /* A */
            { 0, 3, 2, 0, 7 },        /* R */
            { 0, 3, 3, 0, 7 },        /* G */
            { 0, 3, 4, 0, 7 },        /* B */
        },
    },
    {
        "rgba",
        4,
        0,
        0,
        32 | 128,
        {
            { 0, 3, 1, 0, 7 },        /* R */
            { 0, 3, 2, 0, 7 },        /* G */
            { 0, 3, 3, 0, 7 },        /* B */
            { 0, 3, 4, 0, 7 },        /* A */
        },
    },
    {
        "abgr",
        4,
        0,
        0,
        32 | 128,
        {
            { 0, 3, 1, 0, 7 },        /* A */
            { 0, 3, 2, 0, 7 },        /* B */
            { 0, 3, 3, 0, 7 },        /* G */
            { 0, 3, 4, 0, 7 },        /* R */
        },
    },
    {
        "bgra",
        4,
        0,
        0,
        32 | 128,
        {
            { 0, 3, 1, 0, 7 },        /* B */
            { 0, 3, 2, 0, 7 },        /* G */
            { 0, 3, 3, 0, 7 },        /* R */
            { 0, 3, 4, 0, 7 },        /* A */
        },
    },
    {
        "gray16be",
        1,
        0,
        0,
        1,
        {
            { 0, 1, 1, 0, 15 },       /* Y */
        },
    },
    {
        "gray16le",
        1,
        0,
        0,
        0, {
            { 0, 1, 1, 0, 15 },       /* Y */
        },
    },
    {
        "yuv440p",
        3,
        0,
        1,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuvj440p",
        3,
        0,
        1,
        16,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
        },
    },
    {
        "yuva420p",
        4,
        1,
        1,
        16 | 128,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
            { 3, 0, 1, 0, 7 },        /* A */
        },
    },
        {
        "vdpau_h264",
        0, 1,
        1,
        8,
    },
    {
        "vdpau_mpeg1",
        0, 1,
        1,
        8,
    },
    {
        "vdpau_mpeg2",
        0, 1,
        1,
        8,
    },
    {
        "vdpau_wmv3",
        0, 1,
        1,
        8,
    },
    {
        "vdpau_vc1",
        0, 1,
        1,
        8,
    },
    {
        "rgb48be",
        3,
        0,
        0,
        32 | 1,
        {
            { 0, 5, 1, 0, 15 },       /* R */
            { 0, 5, 3, 0, 15 },       /* G */
            { 0, 5, 5, 0, 15 },       /* B */
        },
    },
    {
        "rgb48le",
        3,
        0,
        0,
        32,
        {
            { 0, 5, 1, 0, 15 },       /* R */
            { 0, 5, 3, 0, 15 },       /* G */
            { 0, 5, 5, 0, 15 },       /* B */
        },
    },
    {
        "rgb565be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 1, 0, 3, 4 },        /* R */
            { 0, 1, 1, 5, 5 },        /* G */
            { 0, 1, 1, 0, 4 },        /* B */
        },
    },
    {
        "rgb565le",
        3,
        0,
        0,
        32,
        {
            { 0, 1, 2, 3, 4 },        /* R */
            { 0, 1, 1, 5, 5 },        /* G */
            { 0, 1, 1, 0, 4 },        /* B */
        },
    },
    {
        "rgb555be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 1, 0, 2, 4 },        /* R */
            { 0, 1, 1, 5, 4 },        /* G */
            { 0, 1, 1, 0, 4 },        /* B */
        },
    },
    {
        "rgb555le",
        3,
        0,
        0,
        32,
        {
            { 0, 1, 2, 2, 4 },        /* R */
            { 0, 1, 1, 5, 4 },        /* G */
            { 0, 1, 1, 0, 4 },        /* B */
        },
    },
    {
        "bgr565be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 1, 0, 3, 4 },        /* B */
            { 0, 1, 1, 5, 5 },        /* G */
            { 0, 1, 1, 0, 4 },        /* R */
        },
    },
    {
        "bgr565le",
        3,
        0,
        0,
        32,
        {
            { 0, 1, 2, 3, 4 },        /* B */
            { 0, 1, 1, 5, 5 },        /* G */
            { 0, 1, 1, 0, 4 },        /* R */
        },
    },
    {
        "bgr555be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 1, 0, 2, 4 },       /* B */
            { 0, 1, 1, 5, 4 },       /* G */
            { 0, 1, 1, 0, 4 },       /* R */
        },
     },
    {
        "bgr555le",
        3,
        0,
        0,
        32,
        {
            { 0, 1, 2, 2, 4 },        /* B */
            { 0, 1, 1, 5, 4 },        /* G */
            { 0, 1, 1, 0, 4 },        /* R */
        },
    },
    {
        "vaapi_moco",
        0, 1,
        1,
        8,
    },
    {
        "vaapi_idct",
        0, 1,
        1,
        8,
    },
    {
        "vaapi_vld",
        0, 1,
        1,
        8,
    },
    {
        "yuv420p16le",
        3,
        1,
        1,
        16,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
        },
    },
    {
        "yuv420p16be",
        3,
        1,
        1,
        1 | 16,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
        },
    },
    {
        "yuv422p16le",
        3,
        1,
        0,
        16,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
        },
    },
    {
        "yuv422p16be",
        3,
        1,
        0,
        1 | 16,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
        },
    },
    {
        "yuv444p16le",
        3,
        0,
        0,
        16,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
        },
    },
    {
        "yuv444p16be",
        3,
        0,
        0,
        1 | 16,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
        },
    },
    {
        "vdpau_mpeg4",
        0, 1,
        1,
        8,
    },
    {
        "dxva2_vld",
        0, 1,
        1,
        8,
    },
    {
        "rgb444le",
        3,
        0,
        0,
        32,
        {
            { 0, 1, 2, 0, 3 },        /* R */
            { 0, 1, 1, 4, 3 },        /* G */
            { 0, 1, 1, 0, 3 },        /* B */
        },
    },
    {
        "rgb444be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 1, 0, 0, 3 },        /* R */
            { 0, 1, 1, 4, 3 },        /* G */
            { 0, 1, 1, 0, 3 },        /* B */
        },
    },
    {
        "bgr444le",
        3,
        0,
        0,
        32,
        {
            { 0, 1, 2, 0, 3 },        /* B */
            { 0, 1, 1, 4, 3 },        /* G */
            { 0, 1, 1, 0, 3 },        /* R */
        },
    },
    {
        "bgr444be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 1, 0, 0, 3 },       /* B */
            { 0, 1, 1, 4, 3 },       /* G */
            { 0, 1, 1, 0, 3 },       /* R */
        },
     },
    {
        "y400a",
        2,
        0, 0, 128,
        {
            { 0, 1, 1, 0, 7 },        /* Y */
            { 0, 1, 2, 0, 7 },        /* A */
        },
    },
    {
        "bgr48be",
        3,
        0,
        0,
        1 | 32,
        {
            { 0, 5, 1, 0, 15 },       /* B */
            { 0, 5, 3, 0, 15 },       /* G */
            { 0, 5, 5, 0, 15 },       /* R */
        },
    },
    {
        "bgr48le",
        3,
        0,
        0,
        32,
        {
            { 0, 5, 1, 0, 15 },       /* B */
            { 0, 5, 3, 0, 15 },       /* G */
            { 0, 5, 5, 0, 15 },       /* R */
        },
    },
    {
        "yuv420p9be",
        3,
        1,
        1,
        1 | 16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
        },
    },
    {
        "yuv420p9le",
        3,
        1,
        1,
        16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
        },
    },
    {
        "yuv420p10be",
        3,
        1,
        1,
        1 | 16,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
        },
    },
    {
        "yuv420p10le",
        3,
        1,
        1,
        16,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
        },
    },
    {
        "yuv422p10be",
        3,
        1,
        0,
        1 | 16,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
        },
    },
    {
        "yuv422p10le",
        3,
        1,
        0,
        16,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
        },
    },
    {
        "yuv444p9be",
        3,
        0,
        0,
        1 | 16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
        },
    },
    {
        "yuv444p9le",
        3,
        0,
        0,
        16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
        },
    },
    {
        "yuv444p10be",
        3,
        0,
        0,
        1 | 16,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
        },
    },
    {
        "yuv444p10le",
        3,
        0,
        0,
        16,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
        },
    },
    {
        "yuv422p9be",
        3,
        1,
        0,
        1 | 16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
        },
    },
    {
        "yuv422p9le",
        3,
        1,
        0,
        16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
        },
    },
    {
        "vda_vld",
        0, 1,
        1,
        8,
    },
    {
        "gbrp",
        3,
        0,
        0,
        16 | 32,
        {
            { 0, 0, 1, 0, 7 },        /* G */
            { 1, 0, 1, 0, 7 },        /* B */
            { 2, 0, 1, 0, 7 },        /* R */
        },
    },
    {
        "gbrp9be",
        3,
        0,
        0,
        1 | 16 | 32,
        {
            { 0, 1, 1, 0, 8 },        /* G */
            { 1, 1, 1, 0, 8 },        /* B */
            { 2, 1, 1, 0, 8 },        /* R */
        },
    },
    {
        "gbrp9le",
        3,
        0,
        0,
        16 | 32,
        {
            { 0, 1, 1, 0, 8 },        /* G */
            { 1, 1, 1, 0, 8 },        /* B */
            { 2, 1, 1, 0, 8 },        /* R */
        },
    },
    {
        "gbrp10be",
        3,
        0,
        0,
        1 | 16 | 32,
        {
            { 0, 1, 1, 0, 9 },        /* G */
            { 1, 1, 1, 0, 9 },        /* B */
            { 2, 1, 1, 0, 9 },        /* R */
        },
    },
    {
        "gbrp10le",
        3,
        0,
        0,
        16 | 32,
        {
            { 0, 1, 1, 0, 9 },        /* G */
            { 1, 1, 1, 0, 9 },        /* B */
            { 2, 1, 1, 0, 9 },        /* R */
        },
    },
    {
        "gbrp16be",
        3,
        0,
        0,
        1 | 16 | 32,
        {
            { 0, 1, 1, 0, 15 },       /* G */
            { 1, 1, 1, 0, 15 },       /* B */
            { 2, 1, 1, 0, 15 },       /* R */
        },
    },
    {
        "gbrp16le",
        3,
        0,
        0,
        16 | 32,
        {
            { 0, 1, 1, 0, 15 },       /* G */
            { 1, 1, 1, 0, 15 },       /* B */
            { 2, 1, 1, 0, 15 },       /* R */
        },
    },
    {
        "yuva422p",
        4,
        1,
        0,
        16 | 128,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
            { 3, 0, 1, 0, 7 },        /* A */
        },
    },
    {
        "yuva444p",
        4,
        0,
        0,
        16 | 128,
        {
            { 0, 0, 1, 0, 7 },        /* Y */
            { 1, 0, 1, 0, 7 },        /* U */
            { 2, 0, 1, 0, 7 },        /* V */
            { 3, 0, 1, 0, 7 },        /* A */
        },
    },
    {
        "yuva420p9be",
        4,
        1,
        1,
        1 | 16,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
            { 3, 1, 1, 0, 8 },        /* A */
        },
    },
    {
        "yuva420p9le",
        4,
        1,
        1,
        16 | 128,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
            { 3, 1, 1, 0, 8 },        /* A */
        },
    },
    {
        "yuva422p9be",
        4,
        1,
        0,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
            { 3, 1, 1, 0, 8 },        /* A */
        },
    },
    {
        "yuva422p9le",
        4,
        1,
        0,
        16 | 128,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
            { 3, 1, 1, 0, 8 },        /* A */
        },
    },
    {
        "yuva444p9be",
        4,
        0,
        0,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
            { 3, 1, 1, 0, 8 },        /* A */
        },
    },
    {
        "yuva444p9le",
        4,
        0,
        0,
        16 | 128,
        {
            { 0, 1, 1, 0, 8 },        /* Y */
            { 1, 1, 1, 0, 8 },        /* U */
            { 2, 1, 1, 0, 8 },        /* V */
            { 3, 1, 1, 0, 8 },        /* A */
        },
    },
    {
        "yuva420p10be",
        4,
        1,
        1,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
            { 3, 1, 1, 0, 9 },        /* A */
        },
    },
    {
        "yuva420p10le",
        4,
        1,
        1,
        16 | 128,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
            { 3, 1, 1, 0, 9 },        /* A */
        },
    },
    {
        "yuva422p10be",
        4,
        1,
        0,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
            { 3, 1, 1, 0, 9 },        /* A */
        },
    },
    {
        "yuva422p10le",
        4,
        1,
        0,
        16 | 128,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
            { 3, 1, 1, 0, 9 },        /* A */
        },
    },
    {
        "yuva444p10be",
        4,
        0,
        0,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
            { 3, 1, 1, 0, 9 },        /* A */
        },
    },
    {
        "yuva444p10le",
        4,
        0,
        0,
        16 | 128,
        {
            { 0, 1, 1, 0, 9 },        /* Y */
            { 1, 1, 1, 0, 9 },        /* U */
            { 2, 1, 1, 0, 9 },        /* V */
            { 3, 1, 1, 0, 9 },        /* A */
        },
    },
    {
        "yuva420p16be",
        4,
        1,
        1,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
            { 3, 1, 1, 0, 15 },        /* A */
        },
    },
    {
        "yuva420p16le",
        4,
        1,
        1,
        16 | 128,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
            { 3, 1, 1, 0, 15 },        /* A */
        },
    },
    {
        "yuva422p16be",
        4,
        1,
        0,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
            { 3, 1, 1, 0, 15 },        /* A */
        },
    },
    {
        "yuva422p16le",
        4,
        1,
        0,
        16 | 128,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
            { 3, 1, 1, 0, 15 },        /* A */
        },
    },
    {
        "yuva444p16be",
        4,
        0,
        0,
        1 | 16 | 128,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
            { 3, 1, 1, 0, 15 },        /* A */
        },
    },
    {
        "yuva444p16le",
        4,
        0,
        0,
        16 | 128,
        {
            { 0, 1, 1, 0, 15 },        /* Y */
            { 1, 1, 1, 0, 15 },        /* U */
            { 2, 1, 1, 0, 15 },        /* V */
            { 3, 1, 1, 0, 15 },        /* A */
        },
    },
    {
        "vdpau",
        0, 1,
        1,
        8,
    },
};

static enum AVPixelFormat get_pix_fmt_internal(const char *name)
{
    enum AVPixelFormat pix_fmt;

    for (pix_fmt = 0; pix_fmt < AV_PIX_FMT_NB; pix_fmt++)
        if (av_pix_fmt_descriptors[pix_fmt].name &&
            !strcmp(av_pix_fmt_descriptors[pix_fmt].name, name))
            return pix_fmt;

    return AV_PIX_FMT_NONE;
}

const char *av_get_pix_fmt_name(enum AVPixelFormat pix_fmt)
{
    return (unsigned)pix_fmt < AV_PIX_FMT_NB ?
        av_pix_fmt_descriptors[pix_fmt].name : NULL;
}

#if HAVE_BIGENDIAN
#   define X_NE(be, le) be
#else
#   define X_NE(be, le) le
#endif

enum AVPixelFormat av_get_pix_fmt(const char *name)
{
    enum AVPixelFormat pix_fmt;

    if (!strcmp(name, "rgb32"))
        name = X_NE("argb", "bgra");
    else if (!strcmp(name, "bgr32"))
        name = X_NE("abgr", "rgba");

    pix_fmt = get_pix_fmt_internal(name);
    if (pix_fmt == AV_PIX_FMT_NONE) {
        char name2[32];

        snprintf(name2, sizeof(name2), "%s%s", name, X_NE("be", "le"));
        pix_fmt = get_pix_fmt_internal(name2);
    }
    return pix_fmt;
}

int av_get_bits_per_pixel(const AVPixFmtDescriptor *pixdesc)
{
    int c, bits = 0;
    int log2_pixels = pixdesc->log2_chroma_w + pixdesc->log2_chroma_h;

    for (c = 0; c < pixdesc->nb_components; c++) {
        int s = c == 1 || c == 2 ? 0 : log2_pixels;
        bits += (pixdesc->comp[c].depth_minus1 + 1) << s;
    }

    return bits >> log2_pixels;
}

char *av_get_pix_fmt_string (char *buf, int buf_size, enum AVPixelFormat pix_fmt)
{
    /* print header */
    if (pix_fmt < 0) {
       snprintf (buf, buf_size, "name" " nb_components" " nb_bits");
    } else {
        const AVPixFmtDescriptor *pixdesc = &av_pix_fmt_descriptors[pix_fmt];
        snprintf(buf, buf_size, "%-11s %7d %10d", pixdesc->name,
                 pixdesc->nb_components, av_get_bits_per_pixel(pixdesc));
    }

    return buf;
}

const AVPixFmtDescriptor *av_pix_fmt_desc_get(enum AVPixelFormat pix_fmt)
{
    if (pix_fmt < 0 || pix_fmt >= AV_PIX_FMT_NB)
        return NULL;
    return &av_pix_fmt_descriptors[pix_fmt];
}

const AVPixFmtDescriptor *av_pix_fmt_desc_next(const AVPixFmtDescriptor *prev)
{
    if (!prev)
        return &av_pix_fmt_descriptors[0];
    if (prev - av_pix_fmt_descriptors < FF_ARRAY_ELEMS(av_pix_fmt_descriptors) - 1)
        return prev + 1;
    return NULL;
}

enum AVPixelFormat av_pix_fmt_desc_get_id(const AVPixFmtDescriptor *desc)
{
    if (desc < av_pix_fmt_descriptors ||
        desc >= av_pix_fmt_descriptors + FF_ARRAY_ELEMS(av_pix_fmt_descriptors))
        return AV_PIX_FMT_NONE;

    return desc - av_pix_fmt_descriptors;
}

int av_pix_fmt_get_chroma_sub_sample(enum AVPixelFormat pix_fmt,
                                     int *h_shift, int *v_shift)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    if (!desc)
        return AVERROR(ENOSYS);
    *h_shift = desc->log2_chroma_w;
    *v_shift = desc->log2_chroma_h;

    return 0;
}
