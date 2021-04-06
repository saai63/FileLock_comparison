#include <atomic>
#include <thread>

using namespace std;

class rw_lock
{
public:
    rw_lock()
    {
        m_reader_count = 0;
        m_bWriterWaiting = false;
    }

    void acquire_reader()
    {
        int retry = 0;
        while (true)
        {
            uint32_t prev_readers = m_reader_count;
            if ((prev_readers != m_locked_by_writer) && (m_bWriterWaiting == false))
            {
                uint32_t new_readers = prev_readers + 1;
                if (m_reader_count.compare_exchange_weak(prev_readers, new_readers))
                {
                    // Lock acquired
                    return;
                }
            }
            retry++;
            if (retry > m_retry_count)
            {
                retry = 0;
                this_thread::yield();
            }
        }
    }

    void release_reader()
    {
        int retry = 0;
        while (true)
        {
            uint32_t prev_readers = m_reader_count;
            if (prev_readers != m_locked_by_writer && prev_readers > 0)
            {
                uint32_t new_readers = prev_readers - 1;
                if (m_reader_count.compare_exchange_weak(prev_readers, new_readers))
                {
                    // release the lock
                    return;
                }
            }

            retry++;
            if (retry > m_retry_count)
            {
                retry = 0;
                this_thread::yield();
            }
        }
    }

    void acquire_writer()
    {
        m_bWriterWaiting = true;
        int retry = 0;
        while (true)
        {
            uint32_t prev_readers = m_reader_count;
            if (prev_readers == 0)
            {
                if (m_reader_count.compare_exchange_weak(prev_readers, m_locked_by_writer))
                {
                    // lock acquired
                    return;
                }
            }

            retry++;
            if (retry > m_retry_count)
            {
                retry = 0;
                this_thread::yield();
            }
        }
    }

    void release_writer()
    {
        int retry = 0;
        while (true)
        {
            uint32_t prev_readers = m_reader_count;
            if (prev_readers == m_locked_by_writer)
            {
                if (m_reader_count.compare_exchange_weak(prev_readers, 0))
                {
                    m_bWriterWaiting = false;
                    return;
                }
            }

            retry++;
            if (retry > m_retry_count)
            {
                retry = 0;
                this_thread::yield();
            }
        }
    }

private:
    const uint32_t m_locked_by_writer = 0xffffffff;
    const int m_retry_count = 100;
    std::atomic<uint32_t> m_reader_count;
    std::atomic<bool> m_bWriterWaiting;
};

class reader_lock
{
public:
    reader_lock(rw_lock &lock) : m_lock(lock)
    {
        m_lock.acquire_reader();
    }

    ~reader_lock()
    {
        m_lock.release_reader();
    }

private:
    rw_lock &m_lock;
};

class writer_lock
{
public:
    writer_lock(rw_lock &lock) : m_lock(lock)
    {
        m_lock.acquire_writer();
    }

    ~writer_lock()
    {
        m_lock.release_writer();
    }

private:
    rw_lock &m_lock;
};