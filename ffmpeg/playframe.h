#ifndef PLAYFRAME_H
#define PLAYFRAME_H

#include <QImage>

struct AVFrame;

namespace Ffmpeg {

class CodecContext;
class PlayFrame
{
public:
    explicit PlayFrame();
    PlayFrame(const PlayFrame& other);
    ~PlayFrame();

    void clear();

    AVFrame* avFrame();

    QImage toImage(CodecContext *codecContext);

private:
    AVFrame *m_frame;
};

}

#endif // PLAYFRAME_H
