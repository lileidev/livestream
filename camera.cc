#include "camera.h"

Camera::Camera() : dev_name("/dev/video0")
{
    if(open_device() == -1) exit(-1);
    if(init_device() == -1) exit(-1);
}

Camera::~Camera() {
    uninit_device();
    close_device();
    free(buffers);  // release allocated resources
}

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

int Camera::close_device(void){
    if(close(fd) == -1)
    {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
        return -1;
    }

    fd = -1;

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

    min = fmt.fmt.pix.width * 2;
    if(fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if(fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    init_mmap();
}

int Camera::uninit_device(void){
    unsigned int i;

    for(i = 0; i < n_buffers; i++){
        if(munmap(buffers[i].start, buffers[i].length)){
            fprintf(stderr, "%s error %d, %s\n", "munmap", errno, strerror(errno));
            return -1;
        }
    }

    return 0;
}

int Camera::init_mmap(){
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(xioctl(fd, VIDIOC_REQBUFS, &req)){
        if(errno == EINVAL) {
            fprintf(stderr, "%s does not support memory mapping\n", dev_name);
            return -1;
        } else {
            fprintf(stderr, "%s error %d, %s\n", "VIDIOC_REQBUFS", errno, strerror(errno));
            return -1;
        }
    }

    if(req.count < 2){
        fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
        return -1;
    }

    buffers = (struct buffer*)calloc(req.count, sizeof(*buffers));
    if(!buffers){
        fprintf(stderr, "Out of memory\n");
        return -1;
    }

    for(n_buffers = 0; n_buffers < req.count; ++n_buffers){
        struct v4l2_buffer buf;
        CLEAR(buf);
        
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if(xioctl(fd, VIDIOC_QUERYBUF, &buf)){
            fprintf(stderr, "%s error %d, %s\n", "VIDIOC_QUERYBUF", errno, strerror(errno));
            return -1;
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if(MAP_FAILED == buffers[n_buffers].start){
            fprintf(stderr, "%s error %d, %s\n", "mmap", errno, strerror(errno));
            return -1;
        }
    }

    return 0;
}

int Camera::start_capturing(void){
    unsigned int i;
    enum v4l2_buf_type type;

    for(i = 0; i < n_buffers; ++i){
        struct v4l2_buffer buf;
        
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if(xioctl(fd, VIDIOC_QBUF, &buf) == -1){
            fprintf(stderr, "%s error %d, %s\n", "VIDIOC_QBUF", errno, strerror(errno));
            return -1;
        } 
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(xioctl(fd, VIDIOC_STREAMON, &type) == -1){
        fprintf(stderr, "%s error %d, %s\n", "VIDIOC_STREAMON", errno, strerror(errno));
        return -1;
    }

    return 0;
}

int Camera::stop_capturing(void){
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(xioctl(fd, VIDIOC_STREAMOFF, &type) == -1){
        fprintf(stderr, "%s error %d, %s\n", "VIDIOC_STREAMOFF", errno, strerror(errno));
        return -1;
    }

    return 0;
}
