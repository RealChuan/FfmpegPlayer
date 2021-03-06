#include "subtitledecoder.h"
#include "avimage.h"
#include "decoderaudioframe.h"

namespace Ffmpeg {

class SubtitleDecoderPrivate{
public:
    SubtitleDecoderPrivate(QObject *parent)
        : owner(parent){

    }

    QObject *owner;
    bool pause = false;
    QMutex mutex;
    QWaitCondition waitCondition;
};

SubtitleDecoder::SubtitleDecoder(QObject *parent)
    : Decoder<Packet>(parent)
    , d_ptr(new SubtitleDecoderPrivate(this))
{
    qRegisterMetaType<QVector<Ffmpeg::SubtitleImage>>("QVector<Ffmpeg::SubtitleImage>");
}

SubtitleDecoder::~SubtitleDecoder()
{

}

void SubtitleDecoder::stopDecoder()
{
    pause(false);
    Decoder<Packet>::stopDecoder();
}

void SubtitleDecoder::pause(bool state)
{
    if(!isRunning())
        return;

    d_ptr->pause = state;
    if(state)
        return;
    d_ptr->waitCondition.wakeOne();
}

void SubtitleDecoder::runDecoder()
{
    Subtitle subtitle;

    while(m_runing){
        checkPause();
        checkSeek();

        if(m_queue.isEmpty()){
            msleep(1);
            continue;
        }

        Packet packet = m_queue.takeFirst();

        if(!m_contextInfo->decodeSubtitle2(&subtitle, &packet)){
            continue;
        }

        double duration = 0;
        double pts = 0;
        calculateTime(packet.avPacket(), duration, pts);
        subtitle.setdefault(pts, duration, (const char *)packet.avPacket()->data);
        QVector<SubtitleImage> subtitles = subtitle.subtitleImages();
        subtitle.clear();
        if(subtitles.isEmpty())
            continue;

        double audioPts = DecoderAudioFrame::audioClock() * 1000;
        double diff = subtitles.at(0).startDisplayTime - audioPts;
        if(diff > 0){
            msleep(diff);
        }else if(audioPts > subtitles.at(0).endDisplayTime){
            continue;
        }

        emit subtitleImages(subtitles);
    }
}

void SubtitleDecoder::checkPause()
{
    while(d_ptr->pause){
        QMutexLocker locker(&d_ptr->mutex);
        d_ptr->waitCondition.wait(&d_ptr->mutex);
    }
}

void SubtitleDecoder::checkSeek()
{
    if(!m_seek)
        return;

    seekCodec(m_seekTime);
    seekFinish();
}

}
