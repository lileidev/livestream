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
