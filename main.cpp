#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;

template <typename type>
class Set
{
private:
    const int EXP = 10;
    int size, cur_pos;
    type *array, buf;

public:
    void swap(Set &set) throw()
    {
        std::swap(size, set.size);
        std::swap(cur_pos, set.cur_pos);
        std::swap(array, set.array);
    }

    Set(int size = 0) : size(size), cur_pos(0)
    {
        if (size > 0)
            array = new type[size];
        else if (!size)
            array = nullptr;
        else if (size < 0)
        {
            cout << "Error trying to create a Set with negative amount of elements!" << endl;
            array = nullptr;
        }
    }

    Set(const Set &set)
    {
        if (set.array)
        {
            size = set.size, cur_pos = set.cur_pos;
            array = new type[size];
            memcpy(array, set.array, size * sizeof(type));
        }
        else
        {
            size = 0, cur_pos = 0;
            array = nullptr;
        }
    }

    Set(Set &&set)
    {
        if (set.array)
        {
            size = set.size, cur_pos = set.cur_pos;
            array = set.array;
            set.size = 0, set.cur_pos = 0;
            set.array = nullptr;
        }
        else
        {
            size = 0, cur_pos = 0;
            array = nullptr;
        }
    }

    ~Set()
    {
        if (array)
        {
            delete[] array;
            array = nullptr;
            size = 0, cur_pos = 0;
        }
    }

    /*Set &operator=(const Set &set)
    {
        if (this != &set)
        {
            if (set.array)
            {
                size = set.size, cur_pos = set.cur_pos;
                type *temp = new type[size];
                memcpy(temp, set.array, size * sizeof(type));
                if (array)
                    delete[] array;
                array = temp;
            }
            else
            {
                size = 0, cur_pos = 0;
                array = nullptr;
            }
        }
        return *this;
    }*/

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
        if (cur_pos < size)
        {
            array[cur_pos++] = elem;
            return true;
        }
        else if (cur_pos == size)
        {
            type *temp = new type[size + EXP];
            if (array)
            {
                memcpy(temp, array, size * sizeof(type));
                delete[] array;
            }
            array = temp;
            array[cur_pos++] = elem;
            size += EXP;
            return true;
        }
        return false;
    }

    void resize(int new_size)
    {
        if (new_size == size)
            return;
        if (new_size > 0)
        {
            type *temp = new type[new_size];
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
        else if (!new_size)
        {
            if (array)
            {
                delete[] array;
                array = nullptr;
                size = 0, cur_pos = 0;
            }
        }
        else if (new_size < 0)
            cout << "Error trying to make a Set size negative value! Set will remain the same." << endl;
    }

    void print() const
    {
        for (int i = 0; i < cur_pos; i++)
            cout << array[i] << ' ';
        cout << endl;
    }
};

int main(int argc, char **argv)
{
    Set<double> mySet;
    cout << mySet.getSize() << endl;
    mySet.insert(10);
    mySet.insert(100500);
    mySet.print();
    cout << mySet.getSize() << endl;
    Set<double> newSet(mySet);
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
    Set<double> crossSet(newSet);
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
    Set<double> mulSet = crossSet * newSet;
    mulSet.print();

    Set<double> addSet(mulSet + crossSet);
    addSet.print();

    (addSet - crossSet).print();
    Set<double> subtracionSet = newSet - crossSet;
    subtracionSet.print();

    Set<double> checkSet;
    checkSet = newSet;
    checkSet = checkSet;
    checkSet.resize(200);
    cout << checkSet.getSize() << endl;
    checkSet.print();
    checkSet.resize(4);
    cout << checkSet.getSize() << endl;
    checkSet.print();

    Set<double> emptySet;
    cout << emptySet.getActualSize() << endl;
    cout << emptySet.getSize() << endl;
    emptySet.insert(10);
    emptySet.print();
    emptySet.resize(0);
    emptySet.~Set();

    return 0;
}
