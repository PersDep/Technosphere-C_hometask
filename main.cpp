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
class Queue
{
private:
    queue<T> q;

public:
    void Push(T &&t) { q.push(forward<T>(t)); }

    T Pop()
    {
        T t = move(q.front());
        q.pop();
        return t;
    }
};

class thread_pool
{
private:
    vector<thread> threads;
    Queue<function<void()>> tasks;
    mutex mtx;

    condition_variable cv;

public:
    thread_pool(int num_threads) : threads(vector<thread>(num_threads))
    {
        for (auto &i : threads)
        {
            i = thread([this](condition_variable &cv)
                    {
                        while (true)
                        {
                            function<void()> task;
                            {
                                unique_lock<mutex> lock(mtx);
                                cv.wait(lock);
                                task = tasks.Pop();
                            }
                            task();
                        }
                    }, ref(cv));
        }
    }
    ~thread_pool()
    {
        for (auto &i : threads)
            if (i.joinable())
                i.join();
    }
    thread_pool(const thread_pool &pool) = delete;
    thread_pool& operator=(const thread_pool &pool) = delete;

    template<typename type>
    void push(type handler)
    {
        unique_lock<mutex> lock(mtx);
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
            ofstream fout(to_string(filesizes[index]), ofstream::app);
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
