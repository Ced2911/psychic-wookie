/*
 * RAW PCM demuxers
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
#include "internal.h"
#include "pcm.h"
#include "libavutil/log.h"
#include "libavutil/opt.h"

#define RAW_SAMPLES     1024

typedef struct PCMAudioDemuxerContext {
    AVClass *class;
    int sample_rate;
    int channels;
} PCMAudioDemuxerContext;

static int pcm_read_header(AVFormatContext *s)
{
    PCMAudioDemuxerContext *s1 = s->priv_data;
    AVStream *st;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);


    st->codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id    = s->iformat->raw_codec_id;
    st->codec->sample_rate = s1->sample_rate;
    st->codec->channels    = s1->channels;

    st->codec->bits_per_coded_sample =
        av_get_bits_per_sample(st->codec->codec_id);

    assert(st->codec->bits_per_coded_sample > 0);

    st->codec->block_align =
        st->codec->bits_per_coded_sample * st->codec->channels / 8;

    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);
    return 0;
}

static int pcm_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    int ret, size, bps;
    //    AVStream *st = s->streams[0];

    size= RAW_SAMPLES*s->streams[0]->codec->block_align;

    ret= av_get_packet(s->pb, pkt, size);

    pkt->stream_index = 0;
    if (ret < 0)
        return ret;

    bps= av_get_bits_per_sample(s->streams[0]->codec->codec_id);
    assert(bps); // if false there IS a bug elsewhere (NOT in this function)
    pkt->dts=
    pkt->pts= pkt->pos*8 / (bps * s->streams[0]->codec->channels);

    return ret;
}

static const AVOption pcm_options[] = {
    { "sample_rate", "", offsetof(PCMAudioDemuxerContext, sample_rate), AV_OPT_TYPE_INT, {0}, 0, INT_MAX, AV_OPT_FLAG_DECODING_PARAM },
    { "channels",    "", offsetof(PCMAudioDemuxerContext, channels),    AV_OPT_TYPE_INT, {1}, 0, INT_MAX, AV_OPT_FLAG_DECODING_PARAM },
    { NULL },
};

static const AVClass f64be_demuxer_class = { "f64be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_f64be_demuxer = { "f64be", NULL_IF_CONFIG_SMALL("PCM 64-bit floating-point big-endian"), 0x0100, ((void *)0), 0, &f64be_demuxer_class, 0, AV_CODEC_ID_PCM_F64BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass f64le_demuxer_class = { "f64le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_f64le_demuxer = { "f64le", NULL_IF_CONFIG_SMALL("PCM 64-bit floating-point little-endian"), 0x0100, ((void *)0), 0, &f64le_demuxer_class, 0, AV_CODEC_ID_PCM_F64LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass f32be_demuxer_class = { "f32be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_f32be_demuxer = { "f32be", NULL_IF_CONFIG_SMALL("PCM 32-bit floating-point big-endian"), 0x0100, ((void *)0), 0, &f32be_demuxer_class, 0, AV_CODEC_ID_PCM_F32BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass f32le_demuxer_class = { "f32le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_f32le_demuxer = { "f32le", NULL_IF_CONFIG_SMALL("PCM 32-bit floating-point little-endian"), 0x0100, ((void *)0), 0, &f32le_demuxer_class, 0, AV_CODEC_ID_PCM_F32LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s32be_demuxer_class = { "s32be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s32be_demuxer = { "s32be", NULL_IF_CONFIG_SMALL("PCM signed 32-bit big-endian"), 0x0100, ((void *)0), 0, &s32be_demuxer_class, 0, AV_CODEC_ID_PCM_S32BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s32le_demuxer_class = { "s32le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s32le_demuxer = { "s32le", NULL_IF_CONFIG_SMALL("PCM signed 32-bit little-endian"), 0x0100, ((void *)0), 0, &s32le_demuxer_class, 0, AV_CODEC_ID_PCM_S32LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s24be_demuxer_class = { "s24be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s24be_demuxer = { "s24be", NULL_IF_CONFIG_SMALL("PCM signed 24-bit big-endian"), 0x0100, ((void *)0), 0, &s24be_demuxer_class, 0, AV_CODEC_ID_PCM_S24BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s24le_demuxer_class = { "s24le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s24le_demuxer = { "s24le", NULL_IF_CONFIG_SMALL("PCM signed 24-bit little-endian"), 0x0100, ((void *)0), 0, &s24le_demuxer_class, 0, AV_CODEC_ID_PCM_S24LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s16be_demuxer_class = { "s16be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s16be_demuxer = { "s16be", NULL_IF_CONFIG_SMALL("PCM signed 16-bit big-endian"), 0x0100, ("sw"), 0, &s16be_demuxer_class, 0, AV_CODEC_ID_PCM_S16BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s16le_demuxer_class = { "s16le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s16le_demuxer = { "s16le", NULL_IF_CONFIG_SMALL("PCM signed 16-bit little-endian"), 0x0100, (((void *)0)), 0, &s16le_demuxer_class, 0, AV_CODEC_ID_PCM_S16LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass s8_demuxer_class = { "s8" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_s8_demuxer = { "s8", NULL_IF_CONFIG_SMALL("PCM signed 8-bit"), 0x0100, "sb", 0, &s8_demuxer_class, 0, AV_CODEC_ID_PCM_S8, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u32be_demuxer_class = { "u32be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u32be_demuxer = { "u32be", NULL_IF_CONFIG_SMALL("PCM unsigned 32-bit big-endian"), 0x0100, ((void *)0), 0, &u32be_demuxer_class, 0, AV_CODEC_ID_PCM_U32BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u32le_demuxer_class = { "u32le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u32le_demuxer = { "u32le", NULL_IF_CONFIG_SMALL("PCM unsigned 32-bit little-endian"), 0x0100, ((void *)0), 0, &u32le_demuxer_class, 0, AV_CODEC_ID_PCM_U32LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u24be_demuxer_class = { "u24be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u24be_demuxer = { "u24be", NULL_IF_CONFIG_SMALL("PCM unsigned 24-bit big-endian"), 0x0100, ((void *)0), 0, &u24be_demuxer_class, 0, AV_CODEC_ID_PCM_U24BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u24le_demuxer_class = { "u24le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u24le_demuxer = { "u24le", NULL_IF_CONFIG_SMALL("PCM unsigned 24-bit little-endian"), 0x0100, ((void *)0), 0, &u24le_demuxer_class, 0, AV_CODEC_ID_PCM_U24LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u16be_demuxer_class = { "u16be" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u16be_demuxer = { "u16be", NULL_IF_CONFIG_SMALL("PCM unsigned 16-bit big-endian"), 0x0100, ("uw"), 0, &u16be_demuxer_class, 0, AV_CODEC_ID_PCM_U16BE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u16le_demuxer_class = { "u16le" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u16le_demuxer = { "u16le", NULL_IF_CONFIG_SMALL("PCM unsigned 16-bit little-endian"), 0x0100, (((void *)0)), 0, &u16le_demuxer_class, 0, AV_CODEC_ID_PCM_U16LE, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass u8_demuxer_class = { "u8" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_u8_demuxer = { "u8", NULL_IF_CONFIG_SMALL("PCM unsigned 8-bit"), 0x0100, "ub", 0, &u8_demuxer_class, 0, AV_CODEC_ID_PCM_U8, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass alaw_demuxer_class = { "alaw" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_alaw_demuxer = { "alaw", NULL_IF_CONFIG_SMALL("PCM A-law"), 0x0100, "al", 0, &alaw_demuxer_class, 0, AV_CODEC_ID_PCM_ALAW, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };

static const AVClass mulaw_demuxer_class = { "mulaw" " demuxer", av_default_item_name, pcm_options, LIBAVUTIL_VERSION_INT, }; AVInputFormat ff_pcm_mulaw_demuxer = { "mulaw", NULL_IF_CONFIG_SMALL("PCM mu-law"), 0x0100, "ul", 0, &mulaw_demuxer_class, 0, AV_CODEC_ID_PCM_MULAW, sizeof(PCMAudioDemuxerContext), 0, pcm_read_header, pcm_read_packet, 0, ff_pcm_read_seek, };
