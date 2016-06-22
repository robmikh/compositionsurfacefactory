#pragma once

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
    public delegate void SharedLockWork();

    public ref class SharedLock sealed
    {
    public:
        SharedLock();

        void Lock(SharedLockWork^ work);
    private:
        std::mutex m_mutex;
    };
}
}