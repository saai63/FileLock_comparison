#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <mutex>
#include <shared_mutex>

#define FILE_TO_BE_USED "test_locking.txt"
std::shared_timed_mutex fileMutex;

bool init()
{
    int fd = open(FILE_TO_BE_USED, O_CREAT | O_TRUNC | O_RDWR, 0777 );
    if(fd < 0)
    {
        std::cout << "Failed to create the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
        return false;
    }
    else
    {
        std::string szString = "This is line number : ";
        for (auto idx=0; idx<100; idx++)
        {
            std::string szString1 = szString + std::to_string(idx) + "\n";
            write(fd, szString1.c_str(), szString1.size());
        }
        close(fd);
        return true;
    }
}

int readLine(int fd, int ID)
{
    std::shared_lock<std::shared_timed_mutex> readerLock(fileMutex);
    char line[1000] = {0};
    char ch;
    int idx=0;
    do{
        if(1 == read(fd, &ch, 1))
            line[idx++] = ch;
        else
            break;
    }while(ch != '\n');
    if(idx)
        std::cout << "[Reader: " << ID << "]" << line;
    return idx;
}

void* reader(void* reader_ID)
{
    int fd = open(FILE_TO_BE_USED, O_RDONLY);
    if( fd < 0 )
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        while(readLine(fd, *(int*)(reader_ID)))
        {
            usleep(100*1000);
        }
        close(fd);
    }
}

void* writer(void* writer_ID)
{
    int fd = open(FILE_TO_BE_USED, O_RDWR);
    if( fd < 0 )
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        lseek(fd, 0, SEEK_END);
        std::string szString = "This is line number : ";
        for (auto idx=100; idx<120; idx++)
        {
            std::string szString1 = szString + std::to_string(idx) + "\n";
            std::lock_guard<std::shared_timed_mutex> writerLock(fileMutex);
            write(fd, szString1.c_str(), szString1.size());
            if(idx == 105 || idx==110 || idx==115)
            {
                std::cout << " Write started" << std::endl;
                sleep(2);
                std::cout << " Write done" << std::endl;
            }
            usleep(100*1000);
        }
        sync();
        close(fd);
    }
}

int main()
{
    init();
    pthread_t reader1, reader2, reader3, writer1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int reader_id[3] = {1,2,3};
    if (pthread_create(&reader1, &attr, reader, &reader_id[0]))
    {
        std::cout << "Failed to create Reader-1" << std::endl;       
    }
    if (pthread_create(&reader2, &attr, reader, &reader_id[1]))
    {
        std::cout << "Failed to create Reader-2" << std::endl;       
    }
    if (pthread_create(&writer1, &attr, writer, &reader_id[1]))
    {
        std::cout << "Failed to create Writer-1" << std::endl;       
    }
    if (pthread_create(&reader3, &attr, reader, &reader_id[2]))
    {
        std::cout << "Failed to create Reader-3" << std::endl;       
    }
    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);
    pthread_join(writer1, NULL);
    pthread_join(reader3, NULL);
    return 0;
}