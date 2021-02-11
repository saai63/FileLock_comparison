#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <thread>

#define FILE_TO_BE_USED "test_locking.txt"
std::shared_timed_mutex fileMutex;

bool init()
{
    FILE *fp = fopen(FILE_TO_BE_USED, "w");
    if(!fp)
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
            fprintf(fp, "%s", szString1.c_str());
        }
        fclose(fp);
        return true;
    }
}

int getLine(FILE *fp, int ID)
{
    char ch[1000] = {0};
    int idx = 0;
    {
        std::shared_lock<std::shared_timed_mutex> readerLock(fileMutex);
        while (1)
        {
            char ch1;
            fscanf(fp, "%c", &ch1);
            if (ch1 == '\n')
                break;
            else
                ch[idx++] = ch1;
        }
        std::cout << "[Reader : " << ID << "] " << ch << std::endl;
    }
    return idx;
}

void* reader(int reader_ID)
{
    FILE* fd = fopen(FILE_TO_BE_USED, "r");
    if( !fd )
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        char *line = NULL;
        size_t len = 0;
        while(!feof(fd))
        {
            getLine(fd, reader_ID);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        free(line);
        fclose(fd);
    }
}

void* writer(int writer_ID)
{
    FILE* fd = fopen(FILE_TO_BE_USED, "a+");
    if( !fd )
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        fseek(fd, 0, SEEK_END);
        std::string szString = "This is line number : ";
        for (auto idx=100; idx<120; idx++)
        {
            std::string szString1 = szString + std::to_string(idx) + "\n";
            std::lock_guard<std::shared_timed_mutex> writerLock(fileMutex);
            fprintf(fd,"%s", szString1.c_str());
            if(idx == 105 || idx==110 || idx==115)
            {
                std::cout << " Write started" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                std::cout << " Write done" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        fclose(fd);
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