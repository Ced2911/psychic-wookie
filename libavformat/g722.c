/*
 * g722 raw demuxer
 * Copyright (c) 2010 Martin Storsjo
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
#include "rawdec.h"

static int g722_read_header(AVFormatContext *s)
{
    AVStream *st;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id    = AV_CODEC_ID_ADPCM_G722;
    st->codec->sample_rate = 16000;
    st->codec->channels    = 1;

    st->codec->bits_per_coded_sample =
        av_get_bits_per_sample(st->codec->codec_id);

    assert(st->codec->bits_per_coded_sample > 0);

    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);
    return 0;
}

AVInputFormat ff_g722_demuxer = {
    "g722",
    NULL_IF_CONFIG_SMALL("raw G.722"),
    AVFMT_GENERIC_INDEX,
    "g722,722",
    0, 0, 0, AV_CODEC_ID_ADPCM_G722,
    0, 0, g722_read_header,
    ff_raw_read_partial_packet,
};
