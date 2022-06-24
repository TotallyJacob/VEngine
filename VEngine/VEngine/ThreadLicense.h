#pragma once


#include "../Logger.hpp"
#include <mutex>

template <bool active>
class ThreadLicense
{
    public:

        ThreadLicense() = default;

        void release() noexcept;
        auto try_aquire() noexcept -> bool;
        auto any_thread_has_license() noexcept -> bool;
        auto thread_has_license() noexcept -> bool;
};

template <>
class ThreadLicense<true>
{
    public:

        void release() noexcept
        {
            if (any_thread_has_license())
            {
                m_mutex_license.unlock();
                m_license_given = false;
            }
            else
            {
                VE_LOG_ERROR("Cannot release license when no thread has aquired it.");
            }
        }

        auto try_aquire() noexcept -> bool
        {
            if (m_license_given)
                return false;

            auto mutex_lock = m_mutex_license.try_lock();
            m_license_given = mutex_lock;

            return mutex_lock;
        }

        auto any_thread_has_license() noexcept -> bool
        {
            return m_license_given;
        }

        auto thread_has_license() noexcept -> bool
        {
            if (!m_license_given)
            {
                return false;
            }

            std::unique_lock<std::recursive_mutex> ul(m_mutex_license, std::try_to_lock);
            return ul.owns_lock();
        }

    private:

        std::atomic<bool>    m_license_given = false;
        std::recursive_mutex m_mutex_license;
};

template <>
class ThreadLicense<false>
{
    public:

        void release()
        {
        }

        auto try_aquire() noexcept -> bool
        {
            return true;
        }

        auto any_thread_has_license() noexcept -> bool
        {
            return true;
        }

        auto thread_has_license() noexcept -> bool
        {
            return true;
        }
};