/*
 * RAW null muxer
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

static int null_write_packet(struct AVFormatContext *s, AVPacket *pkt)
{
    return 0;
}

AVOutputFormat ff_null_muxer = {
    "null",
    NULL_IF_CONFIG_SMALL("raw null video"),
    0, 0, (AV_CODEC_ID_PCM_S16BE),
    AV_CODEC_ID_RAWVIDEO,
    0, AVFMT_NOFILE | AVFMT_NOTIMESTAMPS | AVFMT_RAWPICTURE,
    0, 0, 0, 0, 0, null_write_packet,
};
