#include "pch.h"
#include "Lock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

#if DEBUG
long LockSession::s_lockCount = 0;
#endif

Lock::Lock() { InitializeCriticalSection(&m_criticalSection); }

Lock::~Lock() { DeleteCriticalSection(&m_criticalSection); }

LockSession^ Lock::GetLockSession()
{
	auto lockSession = ref new LockSession(this);
	return lockSession;
}

void Lock::LockInternal()
{
	EnterCriticalSection(&m_criticalSection);
}

void Lock::UnlockInternal()
{
	LeaveCriticalSection(&m_criticalSection);
}

LockSession::LockSession(Lock^ lock)
{
	m_lock = lock;
	m_lock->LockInternal();
#if DEBUG
	InterlockedIncrement(&s_lockCount);
#endif
}

LockSession::~LockSession()
{
	m_lock->UnlockInternal();
#if DEBUG
	InterlockedDecrement(&s_lockCount);
#endif
}