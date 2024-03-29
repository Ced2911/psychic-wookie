/*
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

#ifndef AVCODEC_VERSION_H
#define AVCODEC_VERSION_H

/**
 * @file
 * @ingroup libavc
 * Libavcodec version macros.
 */

#define LIBAVCODEC_VERSION_MAJOR 55
#define LIBAVCODEC_VERSION_MINOR  0
#define LIBAVCODEC_VERSION_MICRO  0

#define LIBAVCODEC_VERSION_INT  AV_VERSION_INT(LIBAVCODEC_VERSION_MAJOR, \
                                               LIBAVCODEC_VERSION_MINOR, \
                                               LIBAVCODEC_VERSION_MICRO)
#define LIBAVCODEC_VERSION      AV_VERSION(LIBAVCODEC_VERSION_MAJOR,    \
                                           LIBAVCODEC_VERSION_MINOR,    \
                                           LIBAVCODEC_VERSION_MICRO)
#define LIBAVCODEC_BUILD        LIBAVCODEC_VERSION_INT

#define LIBAVCODEC_IDENT        "Lavc" AV_STRINGIFY(LIBAVCODEC_VERSION)

/**
 * FF_API_* defines may be placed below to indicate public API that will be
 * dropped at a future version bump. The defines themselves are not part of
 * the public API and may change, break or disappear at any time.
 */

#ifndef FF_API_REQUEST_CHANNELS
#define FF_API_REQUEST_CHANNELS (LIBAVCODEC_VERSION_MAJOR < 56)
#endif
#ifndef FF_API_AVCODEC_RESAMPLE
#define FF_API_AVCODEC_RESAMPLE  (LIBAVCODEC_VERSION_MAJOR < 55)
#endif
#ifndef FF_API_LIBMPEG2
#define FF_API_LIBMPEG2          (LIBAVCODEC_VERSION_MAJOR < 55)
#endif
#ifndef FF_API_MMI
#define FF_API_MMI               (LIBAVCODEC_VERSION_MAJOR < 55)
#endif
#ifndef FF_API_SNOW
#define FF_API_SNOW              (LIBAVCODEC_VERSION_MAJOR < 55)
#endif
#ifndef FF_API_IDCT
#define FF_API_IDCT              (LIBAVCODEC_VERSION_MAJOR < 55)
#endif
#ifndef FF_API_DEINTERLACE
#define FF_API_DEINTERLACE       (LIBAVCODEC_VERSION_MAJOR < 56)
#endif
#ifndef FF_API_DESTRUCT_PACKET
#define FF_API_DESTRUCT_PACKET   (LIBAVCODEC_VERSION_MAJOR < 56)
#endif
#ifndef FF_API_GET_BUFFER
#define FF_API_GET_BUFFER        (LIBAVCODEC_VERSION_MAJOR < 56)
#endif

#endif /* AVCODEC_VERSION_H */
