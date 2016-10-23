#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>

using namespace std;

template <typename type>
class Set
{
private:
    const int EXPANSION = 10;
    bool existance;
    int size, cur_pos;
    type *array, buf;

public:
    void swap(Set &set) throw()
    {
        std::swap(size, set.size);
        std::swap(cur_pos, set.cur_pos);
        std::swap(existance, set.existance);
        std::swap(array, set.array);
    }

    Set(int size = 0) : existance(true), size(size), cur_pos(0)
    {
        array = nullptr;
        if (!(array = new type[size]))
            throw std::bad_alloc();
    }

    Set(vector<type> values) : existance(true), size(values.size()), cur_pos(0)
    {
        array = nullptr;
        if (!(array = new type[size]))
            throw std::bad_alloc();

        for (auto &i : values)
            this->insert(i);
    }

    Set(const Set &set)
    {
        if (set.existance)
        {
            size = set.size, cur_pos = set.cur_pos, existance = true;
            array = nullptr;
            if (!(array = new type[size]))
                throw std::bad_alloc();
            memcpy(array, set.array, size * sizeof(type));
        }
        else
        {
            size = 0, cur_pos = 0, existance = false;
            array = nullptr;
        }
    }

    Set(Set &&set)
    {
        size = set.size, cur_pos = set.cur_pos, existance = set.existance;
        array = set.array;
        set.size = 0, set.cur_pos = 0, set.existance = false;
        set.array = nullptr;
    }

    ~Set()
    {
        if (array)
        {
            delete[] array;
            array = nullptr;
            size = 0, cur_pos = 0;
        }
        existance = false;
    }

    Set &operator=(Set set)
    {
        swap(set);
        return *this;
    }

    Set operator*(const Set &set)
    {
        Set<type> temp;
        vector<bool> flags(set.cur_pos, true);
        for (int i = 0; i < cur_pos; i++)
            for (int j = 0; j < set.cur_pos; j++)
                if (set[j] == (*this)[i] && flags[j])
                {
                    temp.insert(set[j]);
                    flags[j] = false;
                    break;
                }
        return temp;
    }

    Set operator+(const Set &set)
    {
        Set<type> temp(*this);
        for (int i = 0; i < set.cur_pos; i++)
            temp.insert(set[i]);
        return temp;
    }

    Set operator-(const Set &set)
    {
        Set<type> res;
        Set<type> temp = (*this) * set;
        vector<bool> flags(temp.cur_pos, true);
        for (int i = 0; i < cur_pos; i++)
        {
            bool flag = true;
            for (int j = 0; j < temp.cur_pos; j++)
                if (temp[j] == (*this)[i] && flags[j])
                {
                    flag = false, flags[j] = false;
                    break;
                }
            if (flag)
                res.insert((*this)[i]);
        }
        return res;
    }

    int getSize() const { return size; }
    int getActualSize() const { return cur_pos; }

    type &operator[](int n)
    {
        if (0 <= n && n < size)
        {
            if (n < cur_pos)
                return array[n];
            else if (n == cur_pos)
            {
                cur_pos++;
                return array[n];
            }
            else if (n > cur_pos)
            {
                cout << "Warning: this element is out of actual range and might be rewrited or ignored later." << endl;
                return array[n];
            }
        }
        cout << "Error when using [] (element index is not correct)!" << endl;
        buf = type();
        return buf;

    }

    type operator[](int n) const
    {
        if (0 <= n && n < size)
            return array[n];
        else
        {
            cout << "Error when using [] (element index is not correct)!" << endl;
            return type();
        }
    }

    bool insert(type elem)
    {
        if (existance)
        {
            if (cur_pos < size)
            {
                array[cur_pos++] = elem;
                return true;
            }
            else if (cur_pos == size)
            {
                type *temp = nullptr;
                if (!(temp = new type[size + EXPANSION]))
                    throw std::bad_alloc();
                if (array)
                {
                    memcpy(temp, array, size * sizeof(type));
                    delete[] array;
                }
                array = temp;
                array[cur_pos++] = elem;
                size += EXPANSION;
                return true;
            }
        }
        return false;
    }

    void resize(int new_size)
    {
        if (existance)
        {
            if (new_size == size)
                return;
            if (new_size)
            {
                type *temp = nullptr;
                if (!(temp = new type[new_size]))
                    throw std::bad_alloc();
                if (new_size > size)
                    memcpy(temp, array, size * sizeof(type));
                else
                {
                    memcpy(temp, array, new_size * sizeof(type));
                    if (cur_pos > new_size)
                        cur_pos = new_size;
                }
                if (array)
                    delete[] array;
                array = temp;
                size = new_size;
            }
            else
            {
                if (array)
                {
                    delete[] array;
                    array = nullptr;
                    size = 0, cur_pos = 0;
                }
            }
        }
    }

    void print() const
    {
        if (existance)
        {
            for (int i = 0; i < cur_pos; i++)
                cout << array[i] << ' ';
            cout << endl;
        }
    }
};

int main(int argc, const char **argv)
{
    if (argc == 2 && string(argv[1]) == "test")
    {
        Set<int> mySet;
        cout << mySet.getSize() << endl;
        mySet.insert(10);
        mySet.insert(100500);
        mySet.print();
        cout << mySet.getSize() << endl;
        Set<int> newSet(mySet);
        mySet.~Set();
        cout << mySet.getSize() << endl;
        cout << newSet.getSize() << endl;
        newSet.insert(30);
        newSet.insert(100501);
        newSet.insert(30);
        newSet.insert(100501);
        newSet.insert(30);
        newSet.insert(100501);
        newSet.insert(30);
        newSet.insert(100501);
        newSet.insert(30);
        newSet.insert(100501);
        newSet.print();
        cout << newSet.getSize() << endl;
        newSet.resize(100);
        cout << newSet.getSize() << endl;
        newSet.insert(10);
        newSet.insert(100501);
        newSet.insert(40);
        newSet.insert(20501);
        newSet.insert(40);
        newSet.insert(200501);
        newSet.insert(40);
        newSet.insert(200501);
        newSet.insert(30);
        newSet.insert(200501);
        newSet.print();
        cout << newSet.getSize() << endl;
        Set<int> crossSet(newSet);
        crossSet.print();
        newSet[10] = -1000;
        newSet.print();
        cout << newSet.getActualSize() << endl;
        newSet[22] = 7;
        cout << '!' << newSet[22] << endl;
        newSet.insert(18);
        newSet.insert(100500);
        cout << newSet.getActualSize() << endl;
        newSet.print();

        crossSet[3] = 111;
        crossSet.insert(44);
        crossSet.print();
        (crossSet * newSet).print();
        Set<int> mulSet = crossSet * newSet;
        mulSet.print();

        Set<int> addSet(mulSet + crossSet);
        addSet.print();

        (addSet - crossSet).print();
        Set<int> subtracionSet = newSet - crossSet;
        subtracionSet.print();

        Set<int> checkSet;
        checkSet = newSet;
        checkSet = checkSet;
        checkSet.resize(200);
        cout << checkSet.getSize() << endl;
        checkSet.print();
        checkSet.resize(4);
        cout << checkSet.getSize() << endl;
        checkSet.print();

        Set<int> emptySet;
        cout << emptySet.getActualSize() << endl;
        cout << emptySet.getSize() << endl;
        emptySet.insert(10);
        emptySet.print();
        emptySet.resize(0);
        emptySet.~Set();

        Set<int> nullSet(0);
        nullSet.insert(11);
        nullSet.print();
        nullSet = emptySet;
        nullSet.print();
    }

    if (argc > 2 && string(argv[1]) == "test")
    {
        vector<int> values1, values2;
        int size, value;

        sscanf(argv[2], "%d", &size);
        for (int i = 0; i < size; i++)
            sscanf(argv[i + 3], "%d", &value), values1.push_back(value);
        Set<int> work_set1(values1);
        work_set1.print();

        sscanf(argv[values1.size() + 4], "%d", &size);
        for (int i = 0; i < size; i++)
            sscanf(argv[i + values1.size() + 5], "%d", &value), values2.push_back(value);
        Set<int> work_set2(values2);
        work_set2.print();

        Set<int> resSet;
        if (argv[values1.size() + 3][0] == '*')
            resSet = work_set1 * work_set2;
        if (argv[values1.size() + 3][0] == '+')
            resSet = work_set1 + work_set2;
        if (argv[values1.size() + 3][0] == '-')
            resSet = work_set1 - work_set2;
        resSet.print();
    }

    getchar();
    return 0;
}
