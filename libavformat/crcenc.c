/*
 * CRC encoder (for codec/format testing)
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

#include "libavutil/adler32.h"
#include "avformat.h"

typedef struct CRCState {
    uint32_t crcval;
} CRCState;

static int crc_write_header(struct AVFormatContext *s)
{
    CRCState *crc = s->priv_data;

    /* init CRC */
    crc->crcval = 1;

    return 0;
}

static int crc_write_packet(struct AVFormatContext *s, AVPacket *pkt)
{
    CRCState *crc = s->priv_data;
    crc->crcval = av_adler32_update(crc->crcval, pkt->data, pkt->size);
    return 0;
}

static int crc_write_trailer(struct AVFormatContext *s)
{
    CRCState *crc = s->priv_data;
    char buf[64];

    snprintf(buf, sizeof(buf), "CRC=0x%08x\n", crc->crcval);
    avio_write(s->pb, buf, strlen(buf));

    return 0;
}

AVOutputFormat ff_crc_muxer = {
    "crc",
    NULL_IF_CONFIG_SMALL("CRC testing"),
    0, "",
    AV_CODEC_ID_PCM_S16LE,
    AV_CODEC_ID_RAWVIDEO,
    0, 0x0080,
    0, 0, 0, sizeof(CRCState),
    crc_write_header,
    crc_write_packet,
    crc_write_trailer,
};
