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

#include <xtl.h>
#include <stdio.h>
#include "player.h"

player_context_t player_context;

void player_init() {
	// register all codec and format
	av_register_all();

	av_log_set_level(AV_LOG_DEBUG);

	memset(&player_context, 0, sizeof(player_context_t));
	player_context.vid = -1;
	player_context.aid = -1;
}

void player_dump_info() {
	AVDictionaryEntry *tag = NULL;

	av_dump_format(player_context.fmt_ctx, 0, player_context.filename, 0);

    while ((tag = av_dict_get(player_context.fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) 
       printf("%s=%s\n", tag->key, tag->value);
}

void player_close() {
	if (player_context.fmt_ctx) {
		avformat_free_context(player_context.fmt_ctx);
	}

	free(player_context.filename);
}

static int open_stream(int stream_index) {
	AVCodecContext *codecCtx;
	AVCodec *codec;

	codecCtx = player_context.fmt_ctx->streams[stream_index]->codec;

	codec = avcodec_find_decoder(codecCtx->codec_id);

	if (!codec || (avcodec_open2(codecCtx, codec, NULL) < 0)) {
		printf("Unsupported codec!\n");
		return -1;
	}

	switch (codecCtx->codec_type) {
	case AVMEDIA_TYPE_VIDEO:
		player_context.video_stream = player_context.fmt_ctx->streams[stream_index];
		break;
	case AVMEDIA_TYPE_AUDIO:
		player_context.audio_stream = player_context.fmt_ctx->streams[stream_index];
		break;
	default:
		printf("Unsupported stream!\n");
		return -1;
	}

	return 0;
}

void add_picture_to_queue(AVFrame *pFrame) {
	
}

void refresh_display(AVFrame * pFrame) {
	vo_update(pFrame);
}

void video_decode_frame(AVPacket * pkt) {
	AVFrame *pFrame;
	int frameFinished;
	pFrame = avcodec_alloc_frame();

	avcodec_decode_video2(player_context.video_stream->codec, pFrame, &frameFinished, pkt);
	
	if (frameFinished) {
		add_picture_to_queue(pFrame);

		// Todo move ...
		refresh_display(pFrame);
	}

	av_free_packet(pkt);
	av_free(pFrame);
}

void add_packet_in_queue(void * queue, AVPacket * pkt) {
	/** Video **/
	av_dup_packet(pkt);
	video_decode_frame(pkt);
}

int player_run(char * filename) {
	int ret;
	int i;
	AVPacket pkt1;
	AVPacket *packet = &pkt1;
	player_context.filename = strdup(filename);
	
	if ((ret = avformat_open_input(&player_context.fmt_ctx, player_context.filename, NULL, NULL))) {
        return ret;
	}
	
	// Dump information
	player_dump_info();

	// Retrieve stream information
	if ((ret = avformat_find_stream_info(player_context.fmt_ctx, NULL))< 0)
		return ret;

	// Open streams
	player_context.vid = av_find_best_stream(player_context.fmt_ctx, AVMEDIA_TYPE_VIDEO, player_context.vid, -1, NULL, 0);
	player_context.aid = av_find_best_stream(player_context.fmt_ctx, AVMEDIA_TYPE_AUDIO, player_context.aid, -1, NULL, 0);


	if ((ret = open_stream(player_context.vid)) < 0) {
		return ret;
	}

	if (player_context.aid)
		open_stream(player_context.aid);

	// Init libswcale
	player_context.sws_context = sws_getContext(
		player_context.video_stream->codec->width, 
		player_context.video_stream->codec->height,
		player_context.video_stream->codec->pix_fmt, 
		player_context.video_stream->codec->width, 
		player_context.video_stream->codec->height,
		PIX_FMT_YUV420P, 
		SWS_FAST_BILINEAR, 
		NULL, NULL, NULL);

	
	vo_init(player_context.video_stream->codec->width, player_context.video_stream->codec->height);

	while(1) {
		if (av_read_frame(player_context.fmt_ctx, packet) < 0) {
			if (player_context.fmt_ctx->pb->error == 0) {
				Sleep(100);
				continue;
			} else {
				break;
			}
		}

		// Is this a packet from the video stream?
		if (packet->stream_index == player_context.vid) {
			add_packet_in_queue(/** audio_queue **/NULL, packet);
		} 
#if 0
		else if (packet->stream_index == player_context.aid) {
			add_packet_in_queue(/** video_queue **/NULL, packet);
		}
#endif
		else {
			av_free_packet(packet);
		}
	}

	return 0;
}

int main (int argc, char **argv)
{
	char error_str[512];
	int status;

	player_init();

	// status = player_run("game:\\test.avi");
	// status = player_run("game:\\video.wmv");
	status = player_run("game:\\movie.avi");
	
	printf("Player status : %08x\n", status);
	av_strerror(status, error_str, 512);
	printf("%s\n", error_str);
	
	player_close();

    return 0;
}