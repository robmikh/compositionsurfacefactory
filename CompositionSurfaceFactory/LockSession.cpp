#include "pch.h"
#include "LockSession.h"
#include "Lock.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    LockSession::LockSession(
        com_ptr<Robmikh::CompositionSurfaceFactory::implementation::Lock> lock)
    {
        m_lock = lock;
        m_lock->LockInternal();
    }

    void LockSession::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_lock->UnlockInternal();
            m_lock = nullptr;
        }
    }
}
