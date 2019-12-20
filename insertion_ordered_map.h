#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <unordered_map>
#include <list>

class lookup_error : std::exception {
    const char* what() const noexcept override {
        return "lookup_error";
    }
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
    using _pair = std::pair<K, V>;
    using _list = std::list<_pair>; // kolejnosc wstawiania
    using _map = std::unordered_map<const K, typename _list::iterator, Hash>;

    _list list;
    _map map;

//    class iterator {
//    private:
//        typename _list::iterator list_iterator;
//    public:
//        iterator() = default;
//        explicit iterator(typename _list::iterator const &elem) {
//            list_iterator = elem;
//        }
//        iterator(iterator const &other) = default;
//        iterator const &operator++() {
//
//        };
//        bool operator==(iterator const &other) const {};
//        bool operator!=(iterator const &other) const {};
//        //TODO dereference
//    };



public:
    insertion_ordered_map() = default; //TODO implement, noexcept?
    insertion_ordered_map(insertion_ordered_map const &other) {}; //TODO copy-constructor, copy on write, noexcept?
    insertion_ordered_map(insertion_ordered_map &&other) {}; //TODO move-constructor, noexcept?

    insertion_ordered_map &operator=(insertion_ordered_map other) {}; //TODO noexcept?

    bool insert(K const &k, V const &v) {
        _pair pair;
        bool unique = true;
        typename _map::iterator it = map.find(k);

        if (it != map.end()) {
            unique = false;
            typename _list::iterator it_list = it->second;
            list.erase(it_list);
            pair = *it_list;
        } else {
            pair = std::make_pair(k, v);
        }

        try {
            list.push_back(pair);
            map.insert({k, --list.end()});

            return unique;
        } catch (std::bad_alloc &e) {
            if (!list.empty() && list.back() == pair)
                list.pop_back();
            throw;
        }
    }; //TODO copy on write

    void erase(K const &k) {
        typename _map::iterator it = map.find(k);

        if (it == map.end()) throw lookup_error();

        map.erase(it);
        typename _list::iterator it_list = it->second;
        list.erase(it_list);
    }; //TODO handle exceptions thrown by erase, copy on write


    void merge(insertion_ordered_map const &other) {}; //TODO noexcept, copy on write?

    V &at(K const &k) {}; //TODO noexcept?

    V const &at(K const &k) const {}; //TODO noexcept?

    V &operator[](K const &k) {}; //TODO noexcept?

    size_t size() const noexcept {
        return list.size();
    };

    bool empty() const noexcept {
        return list.empty();
    };

    void clear() noexcept {
        list.clear();
        map.clear();
    }; //TODO copy on write

    bool contains(K const &k) noexcept {
        return (map.find(k) != map.end());
    };

//    iterator begin() const {
//        return iterator(list.begin());
//    }; //TODO noexcept?
//    iterator end() const {}; //TODO noexcept?
};

#endif
