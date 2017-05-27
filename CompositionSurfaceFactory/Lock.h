#pragma once

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
	ref class LockSession;

    public ref class Lock sealed
    {
    public:
        Lock();

		LockSession^ GetLockSession();
	internal:
		void LockInternal();
		void UnlockInternal();
    private:
		~Lock();
	private:
		CRITICAL_SECTION m_criticalSection;
    };

	public ref class LockSession sealed
	{
	internal:
		LockSession(Lock^ lock);
	public:
		virtual ~LockSession();
	private:
		Lock^ m_lock;
#if DEBUG
		static long s_lockCount;
#endif
	};
}
}