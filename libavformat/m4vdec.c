/*
 * RAW MPEG-4 video demuxer
 * Copyright (c) 2006  Thijs Vermeir <thijs.vermeir@barco.com>
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
#include "rawdec.h"

#define VISUAL_OBJECT_START_CODE       0x000001b5
#define VOP_START_CODE                 0x000001b6

static int mpeg4video_probe(AVProbeData *probe_packet)
{
    uint32_t temp_buffer= -1;
    int VO=0, VOL=0, VOP = 0, VISO = 0, res=0;
    int i;

    for(i=0; i<probe_packet->buf_size; i++){
        temp_buffer = (temp_buffer<<8) + probe_packet->buf[i];
        if ((temp_buffer & 0xffffff00) != 0x100)
            continue;

        if (temp_buffer == VOP_START_CODE)                         VOP++;
        else if (temp_buffer == VISUAL_OBJECT_START_CODE)          VISO++;
        else if (temp_buffer < 0x120)                              VO++;
        else if (temp_buffer < 0x130)                              VOL++;
        else if (   !(0x1AF < temp_buffer && temp_buffer < 0x1B7)
                 && !(0x1B9 < temp_buffer && temp_buffer < 0x1C4)) res++;
    }

    if (VOP >= VISO && VOP >= VOL && VO >= VOL && VOL > 0 && res==0)
        return AVPROBE_SCORE_MAX/2;
    return 0;
}

static const AVClass m4v_demuxer_class = { "m4v" " demuxer", av_default_item_name, ff_rawvideo_options, LIBAVUTIL_VERSION_INT,};AVInputFormat ff_m4v_demuxer = { "m4v", NULL_IF_CONFIG_SMALL("raw MPEG-4 video"), 0x0100, "m4v", 0, &m4v_demuxer_class, 0, AV_CODEC_ID_MPEG4, sizeof(FFRawVideoDemuxerContext), mpeg4video_probe, ff_raw_video_read_header, ff_raw_read_partial_packet,};

