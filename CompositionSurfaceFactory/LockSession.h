#pragma once

#include "LockSession.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct Lock;

    struct LockSession : LockSessionT<LockSession>
    {
        LockSession(winrt::com_ptr<Robmikh::CompositionSurfaceFactory::implementation::Lock> lock);
        ~LockSession() { Close(); }

        void Close();

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        winrt::com_ptr<Robmikh::CompositionSurfaceFactory::implementation::Lock> m_lock;
        std::atomic<bool> m_closed = false;
    };
}
