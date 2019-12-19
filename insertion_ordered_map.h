#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <unordered_map>
#include <list>

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
    class iterator {
    public:
        iterator() {};
        iterator(iterator const &other) {};
        iterator const &operator++() {};
        bool operator==(iterator const &other) const {};
        bool operator!=(iterator const &other) const {};
        //TODO dereference
    };

public:
    insertion_ordered_map() {}; //TODO implement, noexcept?
    insertion_ordered_map(insertion_ordered_map const &other) {}; //TODO copy-constructor, copy on write, noexcept?
    insertion_ordered_map(insertion_ordered_map &&other) {}; //TODO move-constructor, noexcept?

    insertion_ordered_map &operator=(insertion_ordered_map other) {}; //TODO noexcept?

    bool insert(K const &k, V const &v) {}; //TODO noexcept?

    void erase(K const &k) {

    }; //TODO noexcept?

    void merge(insertion_ordered_map const &other) {}; //TODO noexcept?
    V &at(K const &k) {}; //TODO noexcept?
    V const &at(K const &k) const {}; //TODO noexcept?

    V &operator[](K const &k) {}; //TODO noexcept?
    size_t size() const {}; //TODO noexcept?
    bool empty() const {}; //TODO noexcept?
    void clear() {}; //TODO noexcept?
    bool contains(K const &k) {};//TODO noexcept?

    iterator begin() const {}; //TODO noexcept?
    iterator end() const {}; //TODO noexcept?
};

#endif
