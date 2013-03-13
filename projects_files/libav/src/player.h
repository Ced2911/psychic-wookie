/*
 * Copyright (c) 2013 Ced2911
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

extern "C" {
#include "config.h"
#include <stdint.h>
#include <inttypes.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>	
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

typedef struct {
	char * filename;
	AVFormatContext * fmt_ctx;

	int vid;
	int aid;

	AVStream * video_stream;
	AVStream * audio_stream;

	SwsContext * sws_context;

} player_context_t;

extern player_context_t player_context;

// Ao Vo Input stuff
void ao_init();
void ao_update();

void vo_init(int, int);
void vo_update(AVFrame *);

void input_init();
void input_update();

// Player func...
void player_init();
void player_dump_info();
int player_run(char * filename);
void player_close();
