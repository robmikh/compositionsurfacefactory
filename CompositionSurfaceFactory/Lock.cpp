#include "pch.h"
#include "Lock.h"
#include "LockSession.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    Robmikh::CompositionSurfaceFactory::LockSession Lock::GetLockSession()
    {
        com_ptr<implementation::Lock> lock;
        lock.copy_from(this);
        auto session = make<LockSession>(lock);
        return session;
    }
}
