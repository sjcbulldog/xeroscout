#pragma once

#include "scoutviews_global.h"
#include <QObject>
#include <QString>
#include <QSharedMemory>
#include <QSystemSemaphore>

class SCOUTVIEWS_EXPORT SingleInstanceGuard
{

public:
    SingleInstanceGuard(const QString& key);
    ~SingleInstanceGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY(SingleInstanceGuard)
};

