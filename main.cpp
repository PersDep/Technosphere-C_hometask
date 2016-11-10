#include <condition_variable>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std;

string get_random_string(int size)
{
    string str(size, 0);

    srand(clock());
    for (int i = 0; i < size; i++)
        str[i] = 32 + static_cast<int>((rand() / (RAND_MAX + 1.0)) * 127);

    return str;
}

class thread_pool
{
private:
    struct bool_thread
    {
        thread Thread;
        bool in_use;
    };

    vector<bool_thread> threads;
    thread check;

    unsigned started;
    vector<bool> ready;

    condition_variable cv;
    unique_lock<mutex> lock;
    mutex mtx;

public:
    thread_pool(int num_threads) : threads(vector<bool_thread>(num_threads)), started(0), ready(vector<bool>(num_threads, false))
    {
        for (auto &i : threads)
            i.in_use = false;
        lock = unique_lock<mutex>(mtx);
        check = thread([](condition_variable &cv, unique_lock<mutex> &lock, vector<bool> &ready)
                    {
                        while (true)
                        {
                            cv.wait(lock);
                            for (unsigned i = 0; i < ready.size(); i++)
                            {
                                if (!ready[i])
                                {
                                    ready[i] = true;
                                    break;
                                }
                            }
                        }
                    }, ref(cv), ref(lock), ref(ready));
    }
    ~thread_pool()
    {
        for (auto &i : threads)
            if (i.Thread.joinable())
                i.Thread.join();
        check.detach();
    }
    thread_pool(const thread_pool &pool) = delete;
    thread_pool& operator=(const thread_pool &pool) = delete;

    template<typename type>
    void push(type handler)
    {
        if (started < threads.size())
        {
            for (auto &i : threads)
                if (!(i.Thread.joinable()))
                {
                    i.in_use = true;
                    i.Thread = thread([handler = move(handler)](bool_thread &i, condition_variable &cv)
                    {
                        handler();
                        i.in_use = false;
                        cv.notify_one();
                    }, ref(i), ref(cv));
                    started++;
                    break;
                }
        }
        else
        {
            bool some_ready = false;
            while (!some_ready)
                for (unsigned i = 0; i < ready.size(); i++)
                    if (ready[i])
                    {
                        ready[i] = false;
                        some_ready = true;
                        break;
                    }
            for (auto &i : threads)
                if (!i.in_use)
                {
                    i.Thread.join();
                    i.in_use = true;
                    i.Thread = thread([handler = move(handler)](bool_thread &i, condition_variable &cv)
                    {
                        handler();
                        i.in_use = false;
                        cv.notify_one();
                    }, ref(i), ref(cv));
                    break;
                }
        }
    }
};

int main()
{
    thread_pool pool(3);

    vector<int> filesizes(0);
    vector<mutex *> mtxs;

    while (true)
    {
        bool exist = false;
        int filesize, index = filesizes.size();
        cin >> filesize;

        if (filesize < 0)
            break;

        for (unsigned i = 0; i < filesizes.size(); i++)
            if (filesizes[i] == filesize)
            {
                index = i;
                exist = true;
                break;
            }
        if (!exist)
        {
            filesizes.push_back(filesize);
            mtxs.push_back(new mutex);
        }

        pool.push([index, filesizes, &mtxs]()
        {
            mtxs[index]->lock();
            ofstream fout(to_string(filesizes[index]), ofstream::app);
            fout << get_random_string(filesizes[index]);
            fout.close();
            mtxs[index]->unlock();
        });
    }

    for (auto &i : mtxs)
        delete i;

    return 0;
}
