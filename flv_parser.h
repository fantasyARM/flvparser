#pragma once
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#pragma pack(1)
#define FLV_HEAD "FLV"

typedef struct FLVHead_tag
{
    char signture[3];/*FLV*/

    uint8_t version;

    uint8_t hasaudio:1;
    uint8_t reserved2:1;
    uint8_t hasvideo:1;
    uint8_t reserved1:5;

    int dataoffset;
    void DUMP()
    {
        printf("signture    [%s]\n", signture);
        printf("version     [%d]\n", version);
        printf("hasaudio    [%d]\n", hasaudio);
        printf("reserved2   [%d]\n", reserved2);
        printf("hasvideo    [%d]\n", hasvideo);
        printf("reserved1   [%d]\n", reserved1);
        printf("dataoffset  [%d]\n", ntohl(dataoffset));
    }
} FLVHead;

typedef struct FlvTag_tag
{
    uint8_t tag_type;
    uint8_t data_size[3];
    uint8_t timestamp[3];
    uint8_t time_extended;
    uint8_t stream_id[3];

    void DUMP()
    {
        printf("tag_type	[%d]\n", tag_type);
        printf("data_size	[%d]\n", data_size[0]*256*256 + data_size[1]*256 + data_size[2]);
        printf("timestamp	[%d]\n", timestamp[0]*256*256 + timestamp[1]*256 + timestamp[2]);
        printf("time_extended	[%d]\n", time_extended);
        printf("stream_id	[%d]\n", stream_id[0]*256*256 + stream_id[1]*256 + stream_id[2]);
    }
    int GetTagSize() {
        return data_size[0] << 16 | data_size[1] << 8 | data_size[2];
    }

    int64_t GetTimestamp() {
        return (timestamp[0] << 16) | (timestamp[1] << 8) | timestamp[2];
    }
} FlvTag;

typedef struct VideoTag_tag
{
    uint8_t code_id:4;
    uint8_t frame_type:4;
    void DUMP()
    {
        printf("code_id		[%d]\n", code_id);
        printf("frame_type	[%d]\n", frame_type);
    }
} VideoTag;

typedef struct AVCTag_tag
{
    uint8_t video_type;
    uint8_t composition_time[3];
    void DUMP()
    {
        printf("video_type		[%d]\n", video_type);
        printf("composition_time	[%d]\n", composition_time[0]*256*256 + composition_time[1]*256 + composition_time[2]);
    }
    uint64_t GetCts() {
        return composition_time[0] << 16 | composition_time[1] << 8 | composition_time[2];
    }
} AVCTag;


typedef struct AVCDecoderHeader_tag
{
    uint8_t version;
    uint8_t profile;
    uint8_t compatility;
    uint8_t level_indication;
    uint8_t nalu_len:2;
    uint8_t reserved:6;

    uint8_t sps_count:5;
    uint8_t reserved1:3;

    void DUMP()
    {
        printf("version ## %d\n", version);
        printf("profile ## %d\n", profile);
        printf("compatility ## %d\n", compatility);
        printf("level_indication ## %d\n", level_indication);
        printf("nalu_len ## %d\n", nalu_len);
        printf("sps_count ## %d\n", sps_count);
    }
} AVCDecoderHeader;

typedef struct AudioHead_tag
{
    uint8_t sound_rate:2;
    uint8_t sound_size:1;
    uint8_t sound_type:1;
    uint8_t sound_format:4;

    void DUMP()
    {
        printf("	sound_rate	[%d]\n", sound_rate);
        printf("	sound_size	[%d]\n", sound_size);
        printf("	sound_type	[%d]\n", sound_type);
        printf("	sound_format	[%d]\n", sound_format);
    }
} AudioHead;

typedef struct AACHead_tag
{
    uint8_t aac_packet_type;/*0 sequence head 1 aac frame*/
    void DUMP()
    {
        printf("aac_packet_type ## %d\n", aac_packet_type);
    }
} AACHead;


typedef struct AudioSequenceHead_tag
{
    uint8_t sample_freq_index1:3;
    uint8_t audio_profile_type2:1;
    uint8_t audio_profile_type1:4;

    uint8_t extension_flag:1;
    uint8_t dependon_core_coder:1;
    uint8_t frame_length_flag:1;
    uint8_t channel_configuration:4;
    uint8_t sample_freq_index2:1;

    void DUMP()
    {
        printf("sample_freq_index1 ## %d\n", sample_freq_index1);
        printf("audio_profile_type2 ## %d\n", audio_profile_type2);
        printf("audio_profile_type1 ## %d\n", audio_profile_type1);
        printf("extension_flag ## %d\n", extension_flag);
        printf("dependon_core_coder ## %d\n", dependon_core_coder);
        printf("frame_length_flag ## %d\n", frame_length_flag);
        printf("channel_configuration ## %d\n", channel_configuration);
        printf("sample_freq_index2 ## %d\n", sample_freq_index2);
    }
} AudioSequenceHead;

#pragma pack()

int ParseFlvHead(int fd, FLVHead *head);

int ParseFlvPrevSize(int fd, int *size);

int ParseFlvTag(int fd, FlvTag *tag);

int ReadAndParseTag(int fd, const int size, const int tagtype);

int ParseVideoTag(std::vector<uint8_t> &buf);

int ParseAudioTag(std::vector<uint8_t> &buf);

int ParseVideoSequence(uint8_t *buf, const int len);
