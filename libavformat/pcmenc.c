/*
 * RAW PCM muxers
 * Copyright (c) 2002 Fabrice Bellard
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

#include "avformat.h"
#include "rawenc.h"


AVOutputFormat ff_pcm_f64be_muxer = { "f64be", NULL_IF_CONFIG_SMALL("PCM 64-bit floating-point big-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_F64BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_f64le_muxer = { "f64le", NULL_IF_CONFIG_SMALL("PCM 64-bit floating-point little-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_F64LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_f32be_muxer = { "f32be", NULL_IF_CONFIG_SMALL("PCM 32-bit floating-point big-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_F32BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_f32le_muxer = { "f32le", NULL_IF_CONFIG_SMALL("PCM 32-bit floating-point little-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_F32LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s32be_muxer = { "s32be", NULL_IF_CONFIG_SMALL("PCM signed 32-bit big-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_S32BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s32le_muxer = { "s32le", NULL_IF_CONFIG_SMALL("PCM signed 32-bit little-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_S32LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s24be_muxer = { "s24be", NULL_IF_CONFIG_SMALL("PCM signed 24-bit big-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_S24BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s24le_muxer = { "s24le", NULL_IF_CONFIG_SMALL("PCM signed 24-bit little-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_S24LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s16be_muxer = { "s16be", NULL_IF_CONFIG_SMALL("PCM signed 16-bit big-endian"), 0, ("sw"), AV_CODEC_ID_PCM_S16BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s16le_muxer = { "s16le", NULL_IF_CONFIG_SMALL("PCM signed 16-bit little-endian"), 0, (((void *)0)), AV_CODEC_ID_PCM_S16LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_s8_muxer = { "s8", NULL_IF_CONFIG_SMALL("PCM signed 8-bit"), 0, "sb", AV_CODEC_ID_PCM_S8, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u32be_muxer = { "u32be", NULL_IF_CONFIG_SMALL("PCM unsigned 32-bit big-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_U32BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u32le_muxer = { "u32le", NULL_IF_CONFIG_SMALL("PCM unsigned 32-bit little-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_U32LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u24be_muxer = { "u24be", NULL_IF_CONFIG_SMALL("PCM unsigned 24-bit big-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_U24BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u24le_muxer = { "u24le", NULL_IF_CONFIG_SMALL("PCM unsigned 24-bit little-endian"), 0, ((void *)0), AV_CODEC_ID_PCM_U24LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u16be_muxer = { "u16be", NULL_IF_CONFIG_SMALL("PCM unsigned 16-bit big-endian"), 0, ("uw"), AV_CODEC_ID_PCM_U16BE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u16le_muxer = { "u16le", NULL_IF_CONFIG_SMALL("PCM unsigned 16-bit little-endian"), 0, (((void *)0)), AV_CODEC_ID_PCM_U16LE, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_u8_muxer = { "u8", NULL_IF_CONFIG_SMALL("PCM unsigned 8-bit"), 0, "ub", AV_CODEC_ID_PCM_U8, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_alaw_muxer = { "alaw", NULL_IF_CONFIG_SMALL("PCM A-law"), 0, "al", AV_CODEC_ID_PCM_ALAW, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };

AVOutputFormat ff_pcm_mulaw_muxer = { "mulaw", NULL_IF_CONFIG_SMALL("PCM mu-law"), 0, "ul", AV_CODEC_ID_PCM_MULAW, AV_CODEC_ID_NONE, 0, 0x0080, 0, 0, 0, 0, 0, ff_raw_write_packet, };
