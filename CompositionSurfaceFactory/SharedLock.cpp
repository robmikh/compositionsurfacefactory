#include "pch.h"
#include "SharedLock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

SharedLock::SharedLock() { }

void SharedLock::Lock(SharedLockWork^ work)
{
    try
    {
        m_mutex.lock();
        work();
    }
    catch (Exception^ e)
    {
        m_mutex.unlock();
        throw e;
    }
    m_mutex.unlock();
}