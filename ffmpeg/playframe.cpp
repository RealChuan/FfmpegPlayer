#include "playframe.h"
#include "codeccontext.h"

#include <QImage>

extern "C"{
#include <libavformat/avformat.h>
}

namespace Ffmpeg {

PlayFrame::PlayFrame()
{
    m_frame = av_frame_alloc();
    Q_ASSERT(m_frame != nullptr);
}

PlayFrame::PlayFrame(const PlayFrame &other)
{
    m_frame = av_frame_clone(other.m_frame);
}

PlayFrame::~PlayFrame()
{
    Q_ASSERT(m_frame != nullptr);
    av_frame_free(&m_frame);
}

void PlayFrame::clear()
{
    av_frame_unref(m_frame);
}

AVFrame *PlayFrame::avFrame()
{
    Q_ASSERT(m_frame != nullptr);
    return m_frame;
}

QImage PlayFrame::toImage(CodecContext *codecContext)
{
    Q_ASSERT(m_frame != nullptr);
    return QImage((uchar*)m_frame->data[0], codecContext->width(), codecContext->height(), QImage::Format_RGB32);
}

}
