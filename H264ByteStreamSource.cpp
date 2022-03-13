#include "H264ByteStreamSource.h"


H264ByteStreamSource* H264ByteStreamSource::createNew(UsageEnvironment &env, Camera *cam){
    H264ByteStreamSource* newSource = new H264ByteStreamSource(env, cam);

    return newSource;
}

H264ByteStreamSource::H264ByteStreamSource(UsageEnvironment &env, Camera *cam)
    : FramedSource(env), cam(*cam)
{
    cam->initialize();
}

H264ByteStreamSource::~H264ByteStreamSource(){

}