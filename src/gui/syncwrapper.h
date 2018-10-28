#ifndef SYNCWRAPPER_H
#define SYNCWRAPPER_H

#include <QObject>
#include <QMap>
#include "common/syncjournaldb.h"

namespace OCC {

struct SyncQueue
{
    bool _syncing; // false means syncing is done
    SyncQueue(bool syncing = false)
        : _syncing(syncing)
    {
    }
};

class SyncWrapper : public QObject
{
    Q_OBJECT
public:
    static SyncWrapper *instance();
    ~SyncWrapper() {}

    void sync(const QString path);
    bool syncing(QString path);
    QDateTime lastAccess(const QString path);
    int updateLastAccess(const QString path);
    int syncMode(const QString path);
    void initSyncMode(const QString path);
    int syncModeDownload(const QString path);
    bool shouldSync(const QString path);

private:
    QString fixPath(QString path);
    QMap<QString, SyncQueue *> _syncQueue;

public slots:
    void updateSyncQueue(const QString path, bool syncing);

signals:

private:
    SyncWrapper() {
        connect(SyncJournalDb::instance(), &SyncJournalDb::syncStatusChanged, this, &SyncWrapper::updateSyncQueue);
    }
};
}

#endif // SYNCWRAPPER_H
