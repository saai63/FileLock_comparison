#include <stdio.h>
#include <iostream>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <thread>

#define FILE_TO_BE_USED "test_locking.txt"

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
    flock(fd, LOCK_SH);
    char line[1000] = {0};
    char ch;
    int idx=0;
    do{
        if(1 == read(fd, &ch, 1))
            line[idx++] = ch;
        else
            break;
    }while(ch != '\n');
    flock(fd, LOCK_UN);
    if(idx)
        std::cout << "[Reader: " << ID << "]" << line;
    return idx;
}

void* reader(int reader_ID)
{
    int fd = open(FILE_TO_BE_USED, O_RDONLY);
    if( fd < 0 )
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        while(readLine(fd, reader_ID))
        {
            usleep(100*1000);
        }
        close(fd);
    }
}

void* writer(int writer_ID)
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
            flock(fd,LOCK_EX);
            write(fd, szString1.c_str(), szString1.size());
            if(idx == 105 || idx==110 || idx==115)
            {
                std::cout << " Write started" << std::endl;
                sleep(2);
                std::cout << " Write done" << std::endl;
            }
            flock(fd, LOCK_UN);
            usleep(100*1000);
        }
        sync();
        close(fd);
    }
}

int main()
{
    init();
    std::thread reader1([]{ reader(1); });
    std::thread reader2([]{ reader(2); });
    std::thread writer1([]{ writer(1); });
    std::thread reader3([]{ reader(3); });
    reader1.join();
    reader2.join();
    writer1.join();
    reader3.join();
    return 0;
}