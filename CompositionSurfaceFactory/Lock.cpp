#include "pch.h"
#include "Lock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

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
	m_lock->LockInternal();
}

LockSession::~LockSession()
{
	m_lock->UnlockInternal();
}