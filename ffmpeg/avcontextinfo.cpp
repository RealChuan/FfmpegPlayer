#include "avcontextinfo.h"
#include "codeccontext.h"

#include <QDebug>
#include <QTime>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#define Error_Index -1

namespace Ffmpeg {

struct AVContextInfoPrivate{
    QScopedPointer<CodecContext> codecCtx; //解码器上下文
    AVStream *stream;   //流
    int streamIndex = Error_Index; // 索引
};

AVContextInfo::AVContextInfo(QObject *parent)
    : QObject(parent)
    , d_ptr(new AVContextInfoPrivate)
{

}

AVContextInfo::~AVContextInfo()
{

}

CodecContext *AVContextInfo::codecCtx()
{
    return d_ptr->codecCtx.data();
}

void AVContextInfo::resetIndex()
{
    d_ptr->streamIndex = Error_Index;
}

void AVContextInfo::setIndex(int index)
{
    d_ptr->streamIndex = index;
}

int AVContextInfo::index()
{
    return d_ptr->streamIndex;
}

bool AVContextInfo::isIndexVaild()
{
    if(d_ptr->streamIndex == Error_Index)
        return false;
    return true;
}

void AVContextInfo::setStream(AVStream *stream)
{
    d_ptr->stream = stream;
    qDebug() << av_q2d(stream->avg_frame_rate);
}

AVStream *AVContextInfo::stream()
{
    return d_ptr->stream;
}

bool AVContextInfo::findDecoder()
{
    Q_ASSERT(d_ptr->stream != nullptr);
    const char *typeStr = av_get_media_type_string(d_ptr->stream->codecpar->codec_type);
    AVCodec *codec = avcodec_find_decoder(d_ptr->stream->codecpar->codec_id);
    if (!codec){
        qWarning() <<  tr("Audio or Video Codec not found: ") << typeStr;
        return false;
    }

    d_ptr->codecCtx.reset(new CodecContext(codec));
    connect(d_ptr->codecCtx.data(), &CodecContext::error, this, &AVContextInfo::error, Qt::UniqueConnection);

    if(!d_ptr->codecCtx->setParameters(d_ptr->stream->codecpar))
        return false;
    d_ptr->codecCtx->setTimebase(d_ptr->stream->time_base);

    //用于初始化pCodecCtx结构
    if(!d_ptr->codecCtx->open(codec)){
        return false;
    }

    qInfo() << tr("Decoder name: ") << codec->name;

    return true;
}

bool AVContextInfo::sendPacket(Packet *packet)
{
    return d_ptr->codecCtx->sendPacket(packet);
}

bool AVContextInfo::receiveFrame(PlayFrame *frame)
{
    return d_ptr->codecCtx->receiveFrame(frame);
}

bool AVContextInfo::decodeSubtitle2(Subtitle *subtitle, Packet *packet)
{
    return d_ptr->codecCtx->decodeSubtitle2(subtitle, packet);
}

unsigned char *AVContextInfo::imageBuffer(PlayFrame &frame)
{
    return d_ptr->codecCtx->imageBuffer(frame);
}

void AVContextInfo::clearImageBuffer()
{
    d_ptr->codecCtx->clearImageBuffer();
}

void AVContextInfo::flush()
{
    d_ptr->codecCtx->flush();
}

double AVContextInfo::timebase()
{
    return av_q2d(d_ptr->stream->time_base);
}

}
