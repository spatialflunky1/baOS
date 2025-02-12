#ifndef VIDEO_H
#define VIDEO_H

#include <datatypes.h>
#include <drivers/video/framebuffer.h>

// List of possible video driver mode types
typedef enum {
    FRAMEBUFFER_CON
} VIDEO_MODES;

//
// Functions
//
VIDEO_MODES get_video_mode(void);

#endif
