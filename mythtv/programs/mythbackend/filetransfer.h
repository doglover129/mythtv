#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_

// POSIX headers
#include <pthread.h>

// ANSI C headers
#include <stdint.h>

// C++ headers
#include <vector>
using namespace std;

// Qt headers
#include <QMutex>
#include <QWaitCondition>

class ProgramInfo;
class RingBuffer;
class MythSocket;
class QString;

class FileTransfer
{
    friend class QObject; // quiet OSX gcc warning

  public:
    FileTransfer(QString &filename, MythSocket *remote,
                 bool usereadahead, int timeout_ms);
    FileTransfer(QString &filename, MythSocket *remote, bool write);

    MythSocket *getSocket() { return sock; }

    bool isOpen(void);

    void Stop(void);

    void UpRef(void);
    bool DownRef(void);

    void Pause(void);
    void Unpause(void);
    int RequestBlock(int size);
    int WriteBlock(int size);

    long long Seek(long long curpos, long long pos, int whence);

    uint64_t GetFileSize(void);

    void SetTimeout(bool fast);

  private:
   ~FileTransfer();

    volatile bool  readthreadlive;
    bool           readsLocked;
    QWaitCondition readsUnlockedCond;

    ProgramInfo *pginfo;
    RingBuffer *rbuffer;
    MythSocket *sock;
    bool ateof;

    vector<char> requestBuffer;

    QMutex lock;
    QMutex refLock;
    int refCount;

    bool writemode;
};

#endif
