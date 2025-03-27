# flvparser

./build.sh

./main xx.flv

signture    [FLV^A^E]
version     [1]
hasaudio    [1]
reserved2   [0]
hasvideo    [1]
reserved1   [0]
dataoffset  [9]
tag_type        [18]
data_size       [293]
timestamp       [0]
time_extended   [0]
stream_id       [0]

---------------------------------split-line------------------------------------
tag_type        [9]
data_size       [60]
timestamp       [0]
time_extended   [0]
stream_id       [0]
        video tag code_id       [H264]
        video tag frame_type    [KeyFrame]
        video tag video_type    [0]
        video tag cts           [0]
                VideoSequence[01 42 C0 28 FF E1 00 28 67 42 C0 28 DB 01 E0 08 9F 97 01 6A 02 02 02 80 00 00 03 00 80 00 00 1E 71 20 00 16 E3 60 00 16 20 1C 94 50 0F 18 32 E0 01 00 04 68 CA 8C B2 ]
tag_type        [8]
data_size       [4]
timestamp       [0]
time_extended   [0]
stream_id       [0]
        sound_rate      [3]
        sound_size      [1]
        sound_type      [1]
        sound_format    [10]

---------------------------------audio-split-line------------------------------------
tag_type        [9]
data_size       [68101]
timestamp       [4073033]
time_extended   [0]
stream_id       [0]
        video tag code_id       [H264]
        video tag frame_type    [KeyFrame]
        video tag video_type    [1]
        video tag cts           [0]
                nalutype[7]     nalusize[40]
                nalutype[8]     nalusize[4]
                nalutype[6]     nalusize[9]
                nalutype[6]     nalusize[7]
                nalutype[5]     nalusize[68016]

---------------------------------video-split-line------------------------------------
tag_type        [8]
data_size       [234]
timestamp       [4073035]
time_extended   [0]
stream_id       [0]
        sound_rate      [3]
        sound_size      [1]
        sound_type      [1]
        sound_format    [10]

