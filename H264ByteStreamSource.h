#ifndef H264BYTESTREAMSOURCE_H
#define H264BYTESTREAMSOURCE_H

#include "liveMedia.hh"
#include "camera.h"

class H264ByteStreamSource : public FramedSource {
public:
    static H264ByteStreamSource* createNew(UsageEnvironment &env, Camera *cam);

protected:
    H264ByteStreamSource(UsageEnvironment &env, Camera *cam);
    virtual ~H264ByteStreamSource();
    void doReadFromCam();

private:
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();

private:
    Camera cam;
};


#endif
