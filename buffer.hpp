#ifndef __M_BUFFER_H__
#define __M_BUFFER_H__

#include <vector>
#include <cassert>

namespace ns_log
{
#define DEFAULT_BUFFER_SIZE (10 * 1024 * 1024)
#define THRESHOLD_BUFFER_SIZE (40 * 1024 * 1024)
#define INCREMENT_BUFFER_SIZE (10 * 1024 * 1024)
    class Buffer
    {
    public:
        Buffer() : _buffer(DEFAULT_BUFFER_SIZE), _reader_idx(0), _writer_idx(0) {}
        void push(const char *data, const size_t &len)
        {
            // if (len > writeAbleSize())
            //     return false;
            ensureEnoughSize(len);
            std::copy(data, data + len, &_buffer[_writer_idx]);
            moveWriter(len);
            return;
        }
        const char *readBegin()
        {
            return &_buffer[_reader_idx];
        }
        size_t writeAbleSize()
        {
            return _buffer.size() - _writer_idx;
        }
        size_t readAbleSize()
        {
            return _writer_idx - _reader_idx;
        }
        void moveReader(const size_t &len)
        {
            assert(_reader_idx + len <= _writer_idx);
            _reader_idx += len;
        }
        void swap(Buffer &buffer)
        {
            std::swap(_buffer, buffer._buffer);
            std::swap(_writer_idx, buffer._writer_idx);
            std::swap(_reader_idx, buffer._reader_idx);
        }
        void reset()
        {
            _writer_idx = 0;
            _reader_idx = 0;
        }
        bool empty()
        {
            return _writer_idx == _reader_idx;
        }

    private:
        void ensureEnoughSize(const size_t &len)
        {
            if (len <= writeAbleSize())
                return;
            size_t new_size = _buffer.size();
            while (_buffer.size() < THRESHOLD_BUFFER_SIZE && len > new_size - _writer_idx)
            {
                new_size *= 2;
            }
            while (_buffer.size() >= THRESHOLD_BUFFER_SIZE && len > new_size - _writer_idx)
            {
                new_size += INCREMENT_BUFFER_SIZE;
            }
            _buffer.resize(new_size);
            std::cout << new_size << ":" << len << ":" << _buffer.size() << "\n";
        }
        void moveWriter(const size_t &len)
        {
            if ((_writer_idx + len <= _buffer.size()) == false)
            {
                std::cout << _writer_idx << "+" << len << "-" << _buffer.size() << "\n";
            }
            assert(_writer_idx + len <= _buffer.size());
            _writer_idx += len;
        }

    private:
        std::vector<char> _buffer;
        size_t _reader_idx;
        size_t _writer_idx;
    };
}

#endif