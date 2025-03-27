#include <vector>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "flv_parser.h"
#define FLV_TAGHEAD_SIZE (11)

static int64_t last_video_dts = 0;
static int64_t last_audio_dts = 0;
static int64_t last_tag_size = 0;
int ParseFlvHead(int fd, FLVHead *head)
{
    uint8_t buf[9] = {0};
    int ret = read(fd, buf, 9);
    if (ret != 9) {
        printf("read to end\n");
        return -1;
    }
    *head = *(FLVHead*)buf;
    return 0;
}

int ParseFlvPrevSize(int fd, int *size)
{
    uint8_t data[4] = {0};
    int ret = read(fd, data, 4);
    if (ret != 4) {
        printf("read to end\n");
        return -1;
    }
    *size = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    if (last_tag_size != 0 && last_tag_size != *size) {
        printf("[error] tagsize not equal last_tag_size[%d] prevtagsize[%d]\n", last_tag_size, *size);
        exit(1);
    }
    return 0;
}

int ParseFlvTag(int fd, FlvTag *tag)
{
    uint8_t buf[11] = {0};
    int ret = read(fd, buf, 11);
    if (ret != 11) {
        printf("read to end\n");
        return -1;
    }
    *tag = *(FlvTag*)buf;
    last_tag_size = tag->GetTagSize() + 11;
    if (tag->tag_type == 8) {
        if (last_audio_dts > 0) {
            printf("duration	[%lld]\n", tag->GetTimestamp() - last_audio_dts);
            if (last_audio_dts >= tag->GetTimestamp() || tag->GetTimestamp() - last_audio_dts >= 1000)
                printf("[error] timestamp exception last_audio_dts[%lld] GetTimestamp[%lld]\n", last_audio_dts, tag->GetTimestamp());
        }
        last_audio_dts = tag->GetTimestamp();
    } else if (tag->tag_type == 9) {
        if (last_video_dts > 0) {
            printf("duration	[%lld]\n", tag->GetTimestamp() - last_video_dts);
            if (last_video_dts >= tag->GetTimestamp() || tag->GetTimestamp() - last_video_dts >= 1000)
                printf("[error] timestamp exception last_video_dts[%lld] GetTimestamp[%lld]\n", last_video_dts, tag->GetTimestamp());
        }
        last_video_dts = tag->GetTimestamp();
    }
    return 0;
}

int ReadAndParseTag(int fd, const int size, const int tag_type)
{
    std::vector<uint8_t> buf(size);
    int ret = read(fd, &buf[0], size);
    if (ret != size) {
        printf("read to end\n");
        return -1;
    }
    if (tag_type == 12) {
        printf("			Its Metadata here                 \n");        
    } else if (tag_type == 9) {
        ParseVideoTag(buf);
    } else if (tag_type == 8) {
        ParseAudioTag(buf);
    }
    return 0;
}
static inline char * GetCodecTYPE(const int id)
{
    if (id == 7) {
        return "H264";
    } else if (id == 10) {
        return "AAC";
    } else if (id == 12) {
        return "H265";
    } else {
        return "Unknow";
    }
}

static inline char *GetFrameType(const int frameid)
{
    if (frameid == 1) {
        return "KeyFrame";
    } else if (frameid == 2) {
        return "InterFrame";
    } else if (frameid == 3) {
        return "DisposableInterFrame";
    } else {
        return "Unknow";
    }
}
static inline uint8_t *ReadSize(uint8_t *buf, int &size)
{
    size = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]);
    return buf + 4;
}

int ParseVideoTag(std::vector<uint8_t> &buf)
{
    uint8_t *tmp = &buf[0];
    VideoTag *tag = (VideoTag*)tmp;
    printf("	video tag code_id	[%s]\n", GetCodecTYPE(tag->code_id));
    printf("	video tag frame_type    [%s]\n", GetFrameType(tag->frame_type));
    tmp += 1;
    AVCTag *avgtag = (AVCTag*)tmp;
    printf("	video tag video_type	[%d]\n", avgtag->video_type);
    printf("	video tag cts		[%lld]\n", avgtag->GetCts());
    tmp += 4;
    int nalusize = 0;
    int leftlen = buf.size() - 5;
    if (avgtag->video_type == 0) {
        ParseVideoSequence(tmp, leftlen);
        return 0;
    }
    int datalen = leftlen;
    int totalnalulen = 0;
    while (leftlen >= 4) {
        tmp = ReadSize(tmp, nalusize);
        if (tag->code_id == 7) {
	    uint8_t nalutype = tmp[0]&0x1f;
            printf("		nalutype[%d]	nalusize[%d]\n", nalutype, nalusize);
            if (nalutype == 12) {
                printf("			[%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X]\n",
				tmp[0]&0xff, tmp[1]&0xff, tmp[2]&0xff, tmp[3]&0xff, tmp[4]&0xff,
				tmp[5]&0xff, tmp[6]&0xff, tmp[7]&0xff, tmp[8]&0xff, tmp[9]&0xff);
            }
        } else if (tag->code_id == 12) {
            uint8_t nalutype = (tmp[0]&0x3f) >> 1;
            printf("            nalutype[%d]    nalusize[%d]\n", nalutype, nalusize);
        }
        tmp += nalusize;
        leftlen -= (4 + nalusize);
        totalnalulen += (4 + nalusize);
    }
    printf("\n---------------------------------video-split-line------------------------------------\n");
    if (datalen != totalnalulen) {
        printf("datalen error datalen[%d] totalnalulen[%d]\n", datalen, totalnalulen);
        exit(1);
    }
    return 0;
}

int ParseAudioTag(std::vector<uint8_t> &buf)
{
    uint8_t *tmp = &buf[0];
    AudioHead *ahead = (AudioHead*)tmp;
    ahead->DUMP();
    printf("\n---------------------------------audio-split-line------------------------------------\n");
    return 0;
}

int ParseVideoSequence(uint8_t *buf, const int len)
{
    printf("		VideoSequence[");
    for (int i = 0; i < len; i++)
        printf("%02X ", buf[i]&0xff);
    printf("]\n");
}
