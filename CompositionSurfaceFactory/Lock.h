#pragma once

#include "Lock.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct Lock : LockT<Lock>
    {
        Lock() { InitializeCriticalSection(&m_criticalSection); }
        ~Lock() { DeleteCriticalSection(&m_criticalSection); }

        Robmikh::CompositionSurfaceFactory::LockSession GetLockSession();

        void LockInternal() { EnterCriticalSection(&m_criticalSection); }
        void UnlockInternal() { LeaveCriticalSection(&m_criticalSection); }

    private:
        CRITICAL_SECTION m_criticalSection;
    };
}

namespace winrt::Robmikh::CompositionSurfaceFactory::factory_implementation
{
    struct Lock : LockT<Lock, implementation::Lock>
    {
    };
}
