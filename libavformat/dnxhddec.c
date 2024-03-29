/*
 * RAW DNxHD (SMPTE VC-3) demuxer
 * Copyright (c) 2008 Baptiste Coudurier <baptiste.coudurier@gmail.com>
 * Copyright (c) 2009 Reimar Döffinger <Reimar.Doeffinger@gmx.de>
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

#include "libavutil/intreadwrite.h"
#include "avformat.h"
#include "rawdec.h"

static int dnxhd_probe(AVProbeData *p)
{
    static const uint8_t header[] = {0x00,0x00,0x02,0x80,0x01};
    int w, h, compression_id;
    if (p->buf_size < 0x2c)
        return 0;
    if (memcmp(p->buf, header, 5))
        return 0;
    h = AV_RB16(p->buf + 0x18);
    w = AV_RB16(p->buf + 0x1a);
    if (!w || !h)
        return 0;
    compression_id = AV_RB32(p->buf + 0x28);
    if (compression_id < 1237 || compression_id > 1253)
        return 0;
    return AVPROBE_SCORE_MAX;
}

static const AVClass dnxhd_demuxer_class = { "dnxhd" " demuxer", av_default_item_name, ff_rawvideo_options, LIBAVUTIL_VERSION_INT,};AVInputFormat ff_dnxhd_demuxer = { "dnxhd", NULL_IF_CONFIG_SMALL("raw DNxHD (SMPTE VC-3)"), 0x0100, ((void *)0), 0, &dnxhd_demuxer_class, 0, AV_CODEC_ID_DNXHD, sizeof(FFRawVideoDemuxerContext), dnxhd_probe, ff_raw_video_read_header, ff_raw_read_partial_packet,};
