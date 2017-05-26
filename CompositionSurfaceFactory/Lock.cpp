#include "pch.h"
#include "Lock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

long LockSession::s_lockCount = 0;

Lock::Lock() { }

LockSession^ Lock::GetLockSession()
{
	auto lockSession = ref new LockSession(this);
	return lockSession;
}

void Lock::LockInternal()
{
	m_mutex.lock();
}

void Lock::UnlockInternal()
{
	m_mutex.unlock();
}

LockSession::LockSession(Lock^ lock)
{
	m_lock = lock;
	//m_lock->LockInternal();
	InterlockedIncrement(&s_lockCount);
}

LockSession::~LockSession()
{
	//m_lock->UnlockInternal();
	InterlockedDecrement(&s_lockCount);
}