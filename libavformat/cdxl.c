/*
 * CDXL demuxer
 * Copyright (c) 2011-2012 Paul B Mahol
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

#include "libavutil/channel_layout.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/parseutils.h"
#include "libavutil/opt.h"
#include "avformat.h"
#include "internal.h"

#define CDXL_HEADER_SIZE 32

typedef struct CDXLDemuxContext {
    AVClass     *class;
    int         sample_rate;
    char        *framerate;
    AVRational  fps;
    int         read_chunk;
    uint8_t     header[CDXL_HEADER_SIZE];
    int         video_stream_index;
    int         audio_stream_index;
} CDXLDemuxContext;

static int cdxl_read_header(AVFormatContext *s)
{
    CDXLDemuxContext *cdxl = s->priv_data;
    int ret;

    if (cdxl->framerate && (ret = av_parse_video_rate(&cdxl->fps, cdxl->framerate)) < 0) {
        av_log(s, AV_LOG_ERROR,
               "Could not parse framerate: %s.\n", cdxl->framerate);
        return ret;
    }

    cdxl->read_chunk         =  0;
    cdxl->video_stream_index = -1;
    cdxl->audio_stream_index = -1;

    s->ctx_flags |= AVFMTCTX_NOHEADER;

    return 0;
}

static int cdxl_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    CDXLDemuxContext *cdxl = s->priv_data;
    AVIOContext *pb = s->pb;
    uint32_t current_size, video_size, image_size;
    uint16_t audio_size, palette_size, width, height;
    int64_t  pos;
    int      ret;

    if (pb->eof_reached)
        return AVERROR_EOF;

    pos = avio_tell(pb);
    if (!cdxl->read_chunk &&
        avio_read(pb, cdxl->header, CDXL_HEADER_SIZE) != CDXL_HEADER_SIZE)
        return AVERROR_EOF;
    if (cdxl->header[0] != 1) {
        av_log(s, AV_LOG_ERROR, "non-standard cdxl file\n");
        return AVERROR_INVALIDDATA;
    }

    current_size = AV_RB32(&cdxl->header[2]);
    width        = AV_RB16(&cdxl->header[14]);
    height       = AV_RB16(&cdxl->header[16]);
    palette_size = AV_RB16(&cdxl->header[20]);
    audio_size   = AV_RB16(&cdxl->header[22]);
    image_size   = FFALIGN(width, 16) * height * cdxl->header[19] / 8;
    video_size   = palette_size + image_size;

    if (palette_size > 512)
        return AVERROR_INVALIDDATA;
    if (current_size < (uint64_t)audio_size + video_size + CDXL_HEADER_SIZE)
        return AVERROR_INVALIDDATA;

    if (cdxl->read_chunk && audio_size) {
        if (cdxl->audio_stream_index == -1) {
            AVStream *st = avformat_new_stream(s, NULL);
            if (!st)
                return AVERROR(ENOMEM);

            st->codec->codec_type    = AVMEDIA_TYPE_AUDIO;
            st->codec->codec_tag     = 0;
            st->codec->codec_id      = AV_CODEC_ID_PCM_S8;
            if (cdxl->header[1] & 0x10) {
                st->codec->channels       = 2;
                st->codec->channel_layout = AV_CH_LAYOUT_STEREO;
            } else {
                st->codec->channels       = 1;
                st->codec->channel_layout = AV_CH_LAYOUT_MONO;
            }
            st->codec->sample_rate   = cdxl->sample_rate;
            st->start_time           = 0;
            cdxl->audio_stream_index = st->index;
            avpriv_set_pts_info(st, 64, 1, cdxl->sample_rate);
        }

        ret = av_get_packet(pb, pkt, audio_size);
        if (ret < 0)
            return ret;
        pkt->stream_index = cdxl->audio_stream_index;
        pkt->pos          = pos;
        pkt->duration     = audio_size;
        cdxl->read_chunk  = 0;
    } else {
        if (cdxl->video_stream_index == -1) {
            AVStream *st = avformat_new_stream(s, NULL);
            if (!st)
                return AVERROR(ENOMEM);

            st->codec->codec_type    = AVMEDIA_TYPE_VIDEO;
            st->codec->codec_tag     = 0;
            st->codec->codec_id      = AV_CODEC_ID_CDXL;
            st->codec->width         = width;
            st->codec->height        = height;
            st->start_time           = 0;
            cdxl->video_stream_index = st->index;
            if (cdxl->framerate)
                avpriv_set_pts_info(st, 64, cdxl->fps.den, cdxl->fps.num);
            else
                avpriv_set_pts_info(st, 64, 1, cdxl->sample_rate);
        }

        if (av_new_packet(pkt, video_size + CDXL_HEADER_SIZE) < 0)
            return AVERROR(ENOMEM);
        memcpy(pkt->data, cdxl->header, CDXL_HEADER_SIZE);
        ret = avio_read(pb, pkt->data + CDXL_HEADER_SIZE, video_size);
        if (ret < 0) {
            av_free_packet(pkt);
            return ret;
        }
        av_shrink_packet(pkt, CDXL_HEADER_SIZE + ret);
        pkt->stream_index  = cdxl->video_stream_index;
        pkt->flags        |= AV_PKT_FLAG_KEY;
        pkt->pos           = pos;
        pkt->duration      = cdxl->framerate ? 1 : audio_size ? audio_size : 220;
        cdxl->read_chunk   = audio_size;
    }

    if (!cdxl->read_chunk)
        avio_skip(pb, current_size - audio_size - video_size - CDXL_HEADER_SIZE);
    return ret;
}

#define OFFSET(x) offsetof(CDXLDemuxContext, x)
static const AVOption cdxl_options[] = {
    { "sample_rate", "", OFFSET(sample_rate), AV_OPT_TYPE_INT,    { 11025 }, 1, INT_MAX, AV_OPT_FLAG_DECODING_PARAM },
    { "framerate",   "", OFFSET(framerate),   AV_OPT_TYPE_STRING, { NULL },  0, 0,       AV_OPT_FLAG_DECODING_PARAM },
    { NULL },
};

static const AVClass cdxl_demuxer_class = {
    "CDXL demuxer",
    av_default_item_name,
    cdxl_options,
    LIBAVUTIL_VERSION_INT,
};

AVInputFormat ff_cdxl_demuxer = {
    "cdxl",
    NULL_IF_CONFIG_SMALL("Commodore CDXL video"),
    AVFMT_GENERIC_INDEX,
    "cdxl,xl",
    0, &cdxl_demuxer_class,
    0, 0, sizeof(CDXLDemuxContext),
    0, cdxl_read_header,
    cdxl_read_packet,
};