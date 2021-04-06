#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include "rw_lock.h"

#define FILE_TO_BE_USED "test_locking.txt"
//std::shared_timed_mutex fileMutex;
rw_lock my_lock;

bool init()
{
    FILE *fp = fopen(FILE_TO_BE_USED, "w");
    if (!fp)
    {
        std::cout << "Failed to create the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
        return false;
    }
    else
    {
        std::string szString = "This is line number : ";
        for (auto idx = 0; idx < 100; idx++)
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
        reader_lock r_lock(my_lock);
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

void reader(int reader_ID)
{
    FILE *fd = fopen(FILE_TO_BE_USED, "r");
    if (!fd)
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        char *line = NULL;
        size_t len = 0;
        while (!feof(fd))
        {
            getLine(fd, reader_ID);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        free(line);
        fclose(fd);
    }
}

void writer(int writer_ID)
{
    FILE *fd = fopen(FILE_TO_BE_USED, "a+");
    if (!fd)
    {
        std::cout << "Failed to open the file " << FILE_TO_BE_USED << " with error " << strerror(errno);
    }
    else
    {
        fseek(fd, 0, SEEK_END);
        std::string szString = "This is line number : ";
        for (auto idx = 100; idx < 120; idx++)
        {
            std::string szString1 = szString + std::to_string(idx) + "\n";
            auto start = std::chrono::high_resolution_clock::now();
            writer_lock r_lock(my_lock);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            //std::cout << "Writer " << writer_ID << " waited for " << duration_in_milliseconds << " ms" << std::endl;
            fprintf(fd, "%s", szString1.c_str());
            
            if (idx == 105 || idx == 110 || idx == 115)
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

int main(int argc, char* argv[])
{
    int writer_count;
    if(argc > 1)
    {
        writer_count = atoi(argv[1]);
    }
    else
    {
        writer_count = 1;
    }
    init();
    std::vector<std::thread> readers;
    for (int n = 0; n < 50; n++)
    {
        readers.emplace_back(reader, n);
    }
    std::vector<std::thread> writers;
    for (int n = 0; n < writer_count; n++)
    {
        writers.emplace_back(writer, n);
    }

    for (auto &idx : readers)
    {
        idx.join();
    }
    for (auto &idx : writers)
    {
        idx.join();
    }
    return 0;
}