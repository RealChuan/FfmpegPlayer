#include "codeccontext.h"
#include "packet.h"
#include "playframe.h"

extern "C"{
#include <libswscale/swscale.h>
}

CodecContext::CodecContext(const AVCodec *codec)
{
    m_codecCtx = avcodec_alloc_context3(codec);
    m_ok = m_codecCtx == nullptr ? false : true;
}

CodecContext::~CodecContext()
{
    avcodec_free_context(&m_codecCtx);
}

AVCodecContext *CodecContext::avCodecCtx()
{
    Q_ASSERT(m_codecCtx != nullptr);
    return m_codecCtx;
}

bool CodecContext::isOk()
{
    return m_ok;
}

bool CodecContext::setParameters(const AVCodecParameters *par)
{
    Q_ASSERT(m_codecCtx != nullptr);
    if(avcodec_parameters_to_context(m_codecCtx, par) < 0){
        return false;
    }
    return true;
}

void CodecContext::setTimebase(const AVRational &timebase)
{
    Q_ASSERT(m_codecCtx != nullptr);
    m_codecCtx->pkt_timebase = timebase;
}

bool CodecContext::open(AVCodec *codec)
{
    if (avcodec_open2(m_codecCtx, codec, NULL) < 0){
        return false;
    }
    return true;
}

bool CodecContext::sendPacket(Packet *packet)
{
    if(avcodec_send_packet(m_codecCtx, packet->avPacket()) < 0){
        return false;
    }
    return true;
}

bool CodecContext::receiveFrame(PlayFrame *frame)
{
    if(avcodec_receive_frame(m_codecCtx, frame->avFrame()) < 0){
        return false;
    }
    return true;
}

unsigned char *CodecContext::imageBuffer(PlayFrame &frame)
{
    unsigned char *out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, width(), height(), 1));
    av_image_fill_arrays(frame.avFrame()->data, frame.avFrame()->linesize, out_buffer,
                         AV_PIX_FMT_RGB32, width(), height(), 1);

    return out_buffer;
}

int CodecContext::width()
{
    return m_codecCtx->width;
}

int CodecContext::height()
{
    return m_codecCtx->height;
}