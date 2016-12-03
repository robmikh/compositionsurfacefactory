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
        std::mutex m_mutex;
    };

	public ref class LockSession sealed
	{
	internal:
		LockSession(Lock^ lock);
	public:
		virtual ~LockSession();
	private:
		Lock^ m_lock;
	};
}
}