#include "pch.h"
#include "Lock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

long LockSession::s_lockCount = 0;

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
	InterlockedIncrement(&s_lockCount);
}

LockSession::~LockSession()
{
	m_lock->UnlockInternal();
	InterlockedDecrement(&s_lockCount);
}