#include "cexecute.h"
#include <QCoreApplication>
#include <QThread>
#include <QEvent>
#include <QMutex>
#include <QDebug>
namespace Async {

class CRemoteExecuteEvent: public QEvent
{
public:
    CRemoteExecuteEvent(const CExecute::Func& func, QMutex* mutex = nullptr)
        : QEvent(QEvent::Type(QEvent::MaxUser - 10))
        , func(func)
        , mutex(mutex)
    {}

    ~CRemoteExecuteEvent() {
        if (func)func();
        if (mutex)mutex->unlock();
    }

    CExecute::Func func;
    QMutex* mutex;
};


//void CExecute::run(const CExecute::Func& func)
//{
//    run(qApp, func);
//}

//void CExecute::run(QObject* target, const CExecute::Func& func)
//{
//    Q_ASSERT_X(target, __FUNCTION__, "target == nullptr");
//    if (!target)return;
//    qApp->postEvent(target, new CRemoteExecuteEvent(func));
//}

//void CExecute::blockRun(const CExecute::Func& func)
//{
//    blockRun(qApp, func);
//}

//void CExecute::blockRun(QObject* target, const CExecute::Func& func)
//{
//    QMutex mutex;
//    Q_ASSERT_X(target, __FUNCTION__, "target == nullptr");
//    if (!target)return;
//    mutex.lock();
//    qApp->postEvent(target, new CRemoteExecuteEvent(func, &mutex));
//    mutex.lock();
//    mutex.unlock();
//}

CExecute::CExecute(bool block)
    : CExecute(qApp, block)
{

}

CExecute::CExecute(QObject* target, bool block)
    : m_target(target)
    , m_block(block)
{}

void CExecute::run(const CExecute::Func& func) const
{
    Q_ASSERT_X(m_target, __FUNCTION__, "target == nullptr");
    if (!m_target)return;
    if (QThread::currentThread() == m_target->thread()) {
        if (func)func();
        return;
    }
    if (!m_block) {
        qApp->postEvent(m_target, new CRemoteExecuteEvent(func, nullptr));
    } else {
        QMutex mutex;
        mutex.lock();
        qApp->postEvent(m_target, new CRemoteExecuteEvent(func, &mutex));
        mutex.lock();
        mutex.unlock();
    }
}

}
