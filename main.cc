#include "flv_parser.h"
#define ASSERT(ret) \
{ \
    if (ret < 0) { \
        printf("error ret[%d]\n", ret); \
        return ret; \
    } \
} \
void(0)
int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        return -1;
    }
    FLVHead head = {0};
    int ret = ParseFlvHead(fd, &head);    
    ASSERT(ret);

    head.DUMP();

    int size = 0;
    ret = ParseFlvPrevSize(fd, &size);
    ASSERT(ret);


    FlvTag tag = {0};
    ret = ParseFlvTag(fd, &tag);
    ASSERT(ret);

    tag.DUMP();
    ret = ReadAndParseTag(fd, tag.GetTagSize(), tag.tag_type);        
    ASSERT(ret);
    printf("\n---------------------------------split-line------------------------------------\n");
    ret = ParseFlvPrevSize(fd, &size);
    ASSERT(ret);
    while (true) {
        FlvTag tag = {0};
        ret = ParseFlvTag(fd, &tag);
        ASSERT(ret);

        tag.DUMP();
        ret = ReadAndParseTag(fd, tag.GetTagSize(), tag.tag_type);
        ASSERT(ret);
        ret = ParseFlvPrevSize(fd, &size);
        ASSERT(ret);
    }        
    return 0;
}
