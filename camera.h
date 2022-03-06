#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <cstdio>
#include <string>
#include <cstddef>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

using std::string;

#define CLEAR(x) memset(&(x), 0, sizeof(x))

int xioctl(int fh, int request, void *arg){
    int r; 

    do{
        r = ioctl(fh, request, arg);
    }while(r == -1 && errno == EINTR);

    return r;
}

struct buffer {
        void   *start;
        size_t  length;
};

class Camera {
public:
    Camera() : dev_name("/dev/video0") {};
    Camera(const string name) : dev_name(name) {};
    void process_image(const void *p, int size);
    int read_frame(void);
    void stop_capturing(void);
    void start_capturing(void);
    void uninit_device(void);
    void init_read(unsigned int buffer_size);
    void init_mmap(void);
    int init_device(void);
    void close_device(void);
    int open_device(void);

private:
    string dev_name;
    int fd{-1};
    buffer *buffers;
    unsigned int n_buffers;
    int out_buf;
    int force_format;
    int frame_count{70};
};

#endif