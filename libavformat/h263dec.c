/*
 * RAW H.263 video demuxer
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

#include "avformat.h"
#include "rawdec.h"

static int h263_probe(AVProbeData *p)
{
    uint64_t code= -1;
    int i;
    int valid_psc=0;
    int invalid_psc=0;
    int res_change=0;
    int src_fmt, last_src_fmt=-1;
    int last_gn=0;

    for(i=0; i<p->buf_size; i++){
        code = (code<<8) + p->buf[i];
        if ((code & 0xfffffc0000) == 0x800000) {
            src_fmt= (code>>2)&3;
            if(   src_fmt != last_src_fmt
               && last_src_fmt>0 && last_src_fmt<6
               && src_fmt<6)
                res_change++;

            if((code&0x300)==0x200 && src_fmt){
                valid_psc++;
                last_gn=0;
            }else
                invalid_psc++;
            last_src_fmt= src_fmt;
        } else if((code & 0xffff800000) == 0x800000) {
            int gn= (code>>(23-5)) & 0x1F;
            if(gn<last_gn){
                invalid_psc++;
            }else
                last_gn= gn;
        }
    }
    if(valid_psc > 2*invalid_psc + 2*res_change + 3){
        return 50;
    }else if(valid_psc > 2*invalid_psc)
        return 25;
    return 0;
}

static const AVClass h263_demuxer_class = { "h263" " demuxer", av_default_item_name, ff_rawvideo_options, LIBAVUTIL_VERSION_INT,};AVInputFormat ff_h263_demuxer = { "h263", NULL_IF_CONFIG_SMALL("raw H.263"), 0x0100, ((void *)0), 0, &h263_demuxer_class, 0, AV_CODEC_ID_H263, sizeof(FFRawVideoDemuxerContext), h263_probe, ff_raw_video_read_header, ff_raw_read_partial_packet,};
