#include "camera.h"

int Camera::open_device(void){
    struct stat st;
    if(stat(dev_name.c_str(), &st) == -1){
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
        return -1;
    }

    if(!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", dev_name);
        return -1;
    }
    
    fd = open(dev_name.c_str(), O_RDWR | O_NONBLOCK, 0);
    if(fd == -1) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
        return -1;
    }

    return 0;
}

int Camera::init_device(void){
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if(xioctl(fd, VIDIOC_QUERYCAP, &cap)){
        if(errno == EINVAL){
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            return -1;
        }else{
            fprintf(stderr, "%s error %d, %s\n", "VIDIOC_QUERYCAP", errno, strerror(errno));
            return -1;
        }
    }

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n", dev_name);
        return -1;
    }

    if(!(cap.capabilities & V4L2_CAP_STREAMING)){
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
        return -1;
    }

    CLEAR(fmt);
    
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    
    if(xioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        fprintf(stderr, "%s error %d, %s\n", "VIDIOC_S_FMT", errno, strerror(errno));
        return -1;
    }

    
}
