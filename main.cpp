#include <condition_variable>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>

using namespace std;

string get_random_string(int size)
{
    string str(size, 0);

    srand(clock());
    for (int i = 0; i < size; i++)
        str[i] = 32 + static_cast<int>((rand() / (RAND_MAX + 1.0)) * 127);

    return str;
}

template<class T>
class ThreadSafeQueue
{
private:
    queue<T> q;
    mutex mtx;

public:
    void Push(T t) {
        mtx.lock();
        q.push(t);
        mtx.unlock();
    }

    T Pop() {
        mtx.lock();
        T t = q.front();
        q.pop();
        mtx.unlock();
        return t;
    }

    bool Empty() {
        mtx.lock();
        bool empty = q.empty();
        mtx.unlock();
        return empty;
    }
};

class thread_pool
{
private:
    struct Thread
    {
        thread Thread;
        unique_lock<mutex> Lock;
        mutex Mutex;
    };
    vector<Thread> threads;
    ThreadSafeQueue<function<void()>> tasks;

    condition_variable cv;

public:
    thread_pool(int num_threads) : threads(vector<Thread>(num_threads))
    {
        for (auto &i : threads)
        {
            i.Lock = unique_lock<mutex>(i.Mutex);
            i.Thread = thread([this](condition_variable &cv, Thread &i)
                    {
                        while (true)
                        {
                            cv.wait(i.Lock);
                            this->tasks.Pop()();
                        }
                    }, ref(cv), ref(i));
        }
    }
    ~thread_pool()
    {
        for (auto &i : threads)
            if (i.Thread.joinable())
                i.Thread.join();
    }
    thread_pool(const thread_pool &pool) = delete;
    thread_pool& operator=(const thread_pool &pool) = delete;

    template<typename type>
    void push(type handler)
    {
        tasks.Push(handler);
        cv.notify_one();
    }
};

int main()
{
    thread_pool pool(5);
    vector<int> filesizes(0);
    vector<mutex *> mtxs(0);

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
            ofstream fout(to_string(filesizes[index]) + ".txt", ofstream::app);
            fout << get_random_string(filesizes[index]);
            fout.close();
            mtxs[index]->unlock();
        });
    }

    for (auto &i : mtxs)
        delete i;

    exit(0);
    return 0;
}
