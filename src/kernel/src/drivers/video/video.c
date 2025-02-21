#include <drivers/video/video.h>

VIDEO_MODES video_mode = FRAMEBUFFER_CON;

VIDEO_MODES get_video_mode(void) { 
    return video_mode; 
}
