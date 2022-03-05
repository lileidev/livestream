#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>

const unsigned char MAX_DEV_NAME_LENGTH = 16;

struct buffer {
        void   *start;
        size_t  length;
};

class Camera {
public:
    Camera() {;}
    Camera(const char *name) {;}
    void process_image(const void *p, int size);
    int read_frame(void);
    void stop_capturing(void);
    void start_capturing(void);
    void uninit_device(void);
    void init_read(unsigned int buffer_size);
    void init_mmap(void);
    void init_device(void);
    void close_device(void);
    void open_device(void);

private:
    char dev_name[MAX_DEV_NAME_LENGTH];
    int fd{-1};
    buffer *buffers;
    unsigned int n_buffers;
    int out_buf;
    int force_format;
    int frame_count{70};
};

#endif