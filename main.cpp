#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>

using namespace std;

template<typename left, typename right>
class Iterator: public iterator<input_iterator_tag, typename map<left, right>::value_type>
{
    template <typename Left, typename Right>
    friend class Bimap;

    typedef typename map<left, right>::iterator iterator;
private:
    Iterator(iterator p) : p(p) {}
    iterator p;

    iterator trans() { return p; }

public:
    Iterator(const Iterator &it) { p = it.p; }

    bool operator!=(Iterator const& it) const { return p != it.p; }
    bool operator==(Iterator const& it) const { return p == it.p; }
    typename Iterator::reference operator*() const { return *p; }
    iterator operator->() const { return p; }
    Iterator& operator++() { ++p; return *this; }
    Iterator& operator--() { --p; return *this; }
    Iterator& operator++(int) { ++p; return *this; }
    Iterator& operator--(int) { --p; return *this; }
};

template <typename left, typename right>
class Map
{
    template <typename Left, typename Right>
    friend class Bimap;

private:
    map<left, right> my_map;

public:
    Map () {}

    ~Map() {}

    typedef typename map<left, right>::iterator type;
    typedef typename map<left, right>::const_iterator const_type;

    type begin() {return my_map.begin(); }
    type end() {return my_map.end(); }

    const_type begin() const { return my_map.begin(); }
    const_type end() const { return my_map.end(); }

    const right &operator[](const left &elem)
    {
        left val = elem;
        if (!my_map.count(val))
            throw invalid_argument("This elem doesn't exist");
        return my_map[val];
    }
};

template <typename Left, typename Right>
class Bimap
{
public:
    Map<Left, Right> left;
    Map<Right, Left> right;

    Bimap() {}

    Bimap(const Bimap &bimap) { left = bimap.left, right = bimap.right; }

    Bimap(Bimap &&bimap) { left = bimap.left, right = bimap.right, bimap.left.~Map(), bimap.right.~Map(); }

    ~Bimap() {}

    Bimap &operator=(Bimap bimap)
    {
        swap(*this, bimap);
        return *this;
    }

    typedef Iterator<Left, Right> iterator;
    typedef Iterator<const Left, const Right> const_iterator;

    iterator begin() { return iterator(left.my_map.begin()); }
    iterator end() { return iterator(left.my_map.end()); }

    const_iterator begin() const { return const_iterator(left.my_map.begin()); }
    const_iterator end() const { return const_iterator(left.my_map.end()); }

    unsigned getSize() const { return left.my_map.size(); }
    unsigned getMaxSize() const { return left.my_map.max_size(); }

    iterator left_find(const Left &elem)
    {
        auto it = left.my_map.find(elem);
        if (it != left.my_map.end())
            return iterator(it);
        else
            return end();
    }
    iterator right_find(const Right &elem)
    {
        auto it = right.my_map.find(elem);
        if (it != right.my_map.end())
            return left_find(it->second);
        else
            return end();
    }

    int left_count(const Left &elem) { return left.my_map.count(elem); }
    int right_count(const Right &elem) { return right.my_map.count(elem); }

    void erase(iterator first, iterator last)
    {
        for (auto i = first.trans(); i != last.trans(); i++)
            right.my_map.erase(i->second);
        left.my_map.erase(first.trans(), last.trans());
    }

    void erase(iterator it)
    {
        right.my_map.erase(it.trans()->second);
        left.my_map.erase(it.trans());
    }

    iterator insert(const pair<Left, Right> &pair)
    {
        if (left.my_map.count(pair.first))
            return end();
        if (!(right.my_map.insert(make_pair(pair.second, pair.first)).second))
            return end();
        return iterator(left.my_map.insert(pair).first);
    }
};

template <typename left, typename right>
void swap(Bimap<left, right> &bimap1, Bimap<left, right> &bimap2) throw()
{
    std::swap(bimap1.left, bimap2.left);
    std::swap(bimap1.right, bimap2.right);
}

bool check(pair<const string, int> &pair) { return pair.second < 14; }

int main(int argc, const char **argv)
{
    Bimap<const string, int> bimap;
    cout << bimap.getSize() << endl;

    cout << endl;

    bimap.insert(make_pair("axxxxxx", 11));
    bimap.insert(make_pair("abc", 10));
    bimap.insert(make_pair("aacc", 15));
    auto it = bimap.insert(make_pair("abb", 14));
    cout << it->first << ' ' << it->second << endl;
    it--;
    cout << it->first << ' ' << it->second << endl;
    bimap.insert(make_pair("aacc", 30));//won't be inserted
    bimap.insert(make_pair("xxx", 10));//won't be inserted

    cout << endl;

    cout << bimap.left["aacc"] << endl;
    cout << bimap.right[14] << endl;

    cout << endl;

    for (auto i = bimap.begin(); i != bimap.end(); ++i)
        cout << i->first << ' ' << i->second << endl;
    for (auto i = bimap.begin(); i != bimap.end(); i++)
        cout << (*i).first << ' ' << (*i).second << endl;
    for (auto &i : bimap)
        cout << i.first << ' ' << i.second << endl;

    cout << endl;

    for (auto i = bimap.left.begin(); i != bimap.left.end(); i++)
        cout << (*i).first << ' ' << (*i).second << endl;
    for (auto&i : bimap.left)
        cout << i.first << ' ' << i.second << endl;

    cout << endl;

    for (auto i = bimap.right.begin(); i != bimap.right.end(); i++)
        cout << (*i).first << ' ' << (*i).second << endl;
    for (auto&i : bimap.right)
        cout << i.first << ' ' << i.second << endl;

    cout << endl;

    cout << bimap.left_count("aacc") << endl;
    cout << bimap.left_count("xxx") << endl;
    cout << bimap.right_count(14) << endl;
    cout << bimap.right_count(30) << endl;

    cout << endl;

    if ((it = bimap.left_find("aacc")) != bimap.end())
        cout << it->second << endl;
    if ((it = bimap.right_find(11)) != bimap.end())
        cout << it->first << endl;
    if ((it = bimap.left_find("xxx")) != bimap.end())
        cout << it->second << endl;//won't be found
    if ((it = bimap.right_find(30)) != bimap.end())
        cout << it->first << endl;//won't be found

    cout << endl;

    cout << bimap.getSize() << endl;

    cout << endl;

    Bimap<const string, int> my_map;
    my_map = bimap;

    for (auto&i : my_map.right)
        cout << i.first << ' ' << i.second << endl;

    cout << endl;

    my_map.erase(++my_map.begin(), --my_map.end());

    for (auto&i : my_map.left)
        cout << i.first << ' ' << i.second << endl;
    cout << endl;
    for (auto&i : my_map.right)
        cout << i.first << ' ' << i.second << endl;

    cout << endl;

    my_map.erase(++my_map.begin());

    for (auto&i : my_map.left)
        cout << i.first << ' ' << i.second << endl;
    cout << endl;
    for (auto&i : my_map.right)
        cout << i.first << ' ' << i.second << endl;

    cout << endl;

    int count = count_if (bimap.begin(), bimap.end(), check);
    cout << count << endl;

    return 0;
}
