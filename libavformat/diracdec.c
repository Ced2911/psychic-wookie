/*
 * RAW Dirac demuxer
 * Copyright (c) 2007 Marco Gerards <marco@gnu.org>
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

static int dirac_probe(AVProbeData *p)
{
    if (AV_RL32(p->buf) == MKTAG('B', 'B', 'C', 'D'))
        return AVPROBE_SCORE_MAX;
    else
        return 0;
}

static const AVClass dirac_demuxer_class = { "dirac" " demuxer", av_default_item_name, ff_rawvideo_options, LIBAVUTIL_VERSION_INT,};AVInputFormat ff_dirac_demuxer = { "dirac", NULL_IF_CONFIG_SMALL("raw Dirac"), 0x0100, ((void *)0), 0, &dirac_demuxer_class, 0, AV_CODEC_ID_DIRAC, sizeof(FFRawVideoDemuxerContext), dirac_probe, ff_raw_video_read_header, ff_raw_read_partial_packet,};
