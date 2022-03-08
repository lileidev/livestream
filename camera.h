#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <cstdio>
#include <string>
#include <cstddef>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

using std::string;

#define CLEAR(x) memset(&(x), 0, sizeof(x))

int xioctl(int fh, int request, void *arg);

struct buffer {
        void   *start;
        size_t  length;
};

class Camera {
public:
    Camera();
    Camera(const string name);
    ~Camera();
    // void process_image(const void *p, int size);
    int read_frame(void);
    int stop_capturing(void);
    int start_capturing(void);
    int uninit_device(void);
    int init_mmap(void);
    int init_device(void);
    int close_device(void);
    int open_device(void);
    int mainloop(void);

private:
    string dev_name;
    int fd{-1};
    struct buffer *buffers;
    unsigned int n_buffers;
    int out_buf;
    int force_format;
    int frame_count{70};
};

#endif