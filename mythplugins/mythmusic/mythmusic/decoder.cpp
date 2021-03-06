// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

// mythmusic
#include "decoder.h"
#include "constants.h"
#include "musicplayer.h"

// qt
#include <QDir>

// libmyth
#include <mythcontext.h>
#include <output.h>
#include <visual.h>

// libmythmetadata
#include "musicmetadata.h"
#include "metaio.h"

QEvent::Type DecoderEvent::Decoding =
    (QEvent::Type) QEvent::registerEventType();
QEvent::Type DecoderEvent::Stopped =
    (QEvent::Type) QEvent::registerEventType();
QEvent::Type DecoderEvent::Finished =
    (QEvent::Type) QEvent::registerEventType();
QEvent::Type DecoderEvent::Error =
    (QEvent::Type) QEvent::registerEventType();

Decoder::Decoder(DecoderFactory *d, AudioOutput *o) :
    MThread("MythMusicDecoder"), fctry(d), out(o)
{
}

Decoder::~Decoder()
{
    fctry = 0;
    out = 0;
}

QIODevice *Decoder::input(void)
{
    return gPlayer->getDecoderHandler()->getIOFactory()->getInput();
}

void Decoder::setOutput(AudioOutput *o)
{
    lock();
    out = o;
    unlock();
}

void Decoder::error(const QString &e)
{
    QString *str = new QString(e.toUtf8());
    DecoderEvent ev(str);
    dispatch(ev);
}

// static methods
static QList<DecoderFactory*> *factories = NULL;

static void checkFactories()
{
    if (!factories)
    {
        factories = new QList<DecoderFactory*>;

#ifdef HAVE_CDIO
        Decoder::registerFactory(new CdDecoderFactory);
#endif
        Decoder::registerFactory(new avfDecoderFactory);
    }
}

QStringList Decoder::all()
{
    checkFactories();

    QStringList l;

    QList<DecoderFactory*>::iterator it = factories->begin();
    for (; it != factories->end(); ++it)
        l += (*it)->description();

    return l;
}

bool Decoder::supports(const QString &source)
{
    checkFactories();

    QList<DecoderFactory*>::iterator it = factories->begin();
    for (; it != factories->end(); ++it)
    {
        if ((*it)->supports(source))
            return true;
    }

    return false;
}

void Decoder::registerFactory(DecoderFactory *fact)
{
    factories->push_back(fact);
}

Decoder *Decoder::create(const QString &source, AudioOutput *output, bool deletable)
{
    checkFactories();

    QList<DecoderFactory*>::iterator it = factories->begin();
    for (; it != factories->end(); ++it)
    {
        if ((*it)->supports(source))
            return (*it)->create(source, output, deletable);
    }

    return NULL;
}
