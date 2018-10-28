#include "syncwrapper.h"
#include "folderman.h"
#include "socketapi.h"
#include "vfs_mac.h"

namespace OCC {

Q_LOGGING_CATEGORY(lcSyncWrapper, "nextcloud.gui.wrapper", QtInfoMsg)

SyncWrapper *SyncWrapper::instance()
{
    static SyncWrapper instance;
    return &instance;
}

QString SyncWrapper::fixPath(QString path){
    if(path.startsWith('/'))
        path.remove(0, 1);

    return path;
}

void SyncWrapper::updateSyncQueue(const QString path, bool syncing) {
    _syncQueue.insert(fixPath(path), new SyncQueue(syncing));
}

void SyncWrapper::sync(const QString path){
    QString fixedPath = fixPath(path);
    if(_syncQueue.contains(fixedPath) && !_syncQueue.value(fixedPath)->_syncing){
        SyncJournalDb::instance()->setSyncMode(fixedPath, SyncJournalDb::SyncMode::SYNCMODE_ONLINE);
        SyncJournalDb::instance()->setSyncModeDownload(fixedPath, SyncJournalDb::SyncModeDownload::SYNCMODE_DOWNLOADED_NO);
        qCInfo(lcSyncWrapper) << "Path: " << fixedPath << "has SyncMode"<< SyncJournalDb::instance()->getSyncMode(fixedPath);
        qCInfo(lcSyncWrapper) << "Path: " << fixedPath << "has SyncModeDownload"<< SyncJournalDb::instance()->getSyncModeDownload(fixedPath);

        FolderMan *folderMan = OCC::FolderMan::instance();
        qCInfo(lcSyncWrapper) << "Current sync folder: " << folderMan->currentSyncFolder()->path();
        folderMan->currentSyncFolder()->startSync(QStringList(fixedPath));
        updateSyncQueue(path, true);
    }
}

bool SyncWrapper::syncing(QString path){
    QString fixedPath = fixPath(path);
    if(_syncQueue.contains(fixedPath)){
        return _syncQueue.value(fixedPath)->_syncing;
    }
}

QDateTime SyncWrapper::lastAccess(const QString path){
    return SyncJournalDb::instance()->getLastAccess(fixPath(path));
}

int SyncWrapper::updateLastAccess(const QString path){
    return SyncJournalDb::instance()->updateLastAccess(fixPath(path));
}

int SyncWrapper::syncMode(const QString path){
    return SyncJournalDb::instance()->getSyncMode(fixPath(path));
}

void SyncWrapper::initSyncMode(const QString path){
    QString fixedPath = fixPath(path);

    if(SyncJournalDb::instance()->getSyncMode(fixedPath) == SyncJournalDb::SyncMode::SYNCMODE_NONE)
        SyncJournalDb::instance()->setSyncMode(fixedPath, SyncJournalDb::SyncMode::SYNCMODE_OFFLINE);

    if(SyncJournalDb::instance()->getSyncModeDownload(fixedPath) == SyncJournalDb::SyncModeDownload::SYNCMODE_DOWNLOADED_NONE)
        SyncJournalDb::instance()->setSyncModeDownload(fixedPath, SyncJournalDb::SyncModeDownload::SYNCMODE_DOWNLOADED_NO);
}

int SyncWrapper::syncModeDownload(const QString path){
    return SyncJournalDb::instance()->getSyncModeDownload(fixPath(path));
}

bool SyncWrapper::shouldSync(const QString path){
    if(syncMode(fixPath(path)) == SyncJournalDb::SyncMode::SYNCMODE_NONE)
        return false;

    return true;
}

}
