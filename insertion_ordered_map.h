#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <unordered_map>
#include <list>
#include <limits>
#include <boost/exception/detail/shared_ptr.hpp>

const static size_t unshareable_v = std::numeric_limits<size_t>::max();

class lookup_error : std::exception {
    const char* what() const noexcept override {
        return "lookup_error";
    }
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
    using pair_t = std::pair<K, V>;
    using list_t = std::list<pair_t>;
    using map_t = std::unordered_map<const K, typename list_t::iterator, Hash>;

    class data_t {
    public: //TODO handle this
        std::shared_ptr<map_t> map;
        std::shared_ptr<list_t> list;
        size_t ref_count;

        data_t() : map(std::make_shared<map_t>()), list(std::make_shared<list_t>()), ref_count(1) {}; //TODO except/noexcept?
        data_t(data_t const &other) : ref_count(1) {
            map = std::make_shared<map_t>(*other.map); //TODO except
            list = std::make_shared<list_t>(*other.list); //TODO except
        }; //TODO except/noexcept?
        ~data_t() = default;
    };

    std::shared_ptr<data_t> data;

    void prepare_to_modify(bool mark_unshareable) {
        if (data->ref_count > 1 && data->ref_count != unshareable_v) {
            data->ref_count--;
            data = std::make_shared<data_t>(*data); //TODO except
        }
        data->ref_count = (mark_unshareable ? unshareable_v : 1);
    }

public:
    insertion_ordered_map() : data(std::make_shared<data_t>()) {}; //TODO noexcept?

    insertion_ordered_map(insertion_ordered_map const &other) {
        if (other.data->ref_count == unshareable_v) {
            data = std::make_shared<data_t>(*other.data); //TODO except
        } else {
            data = other.data;
            ++data->ref_count;
        }
    }; //TODO noexcept?

//    insertion_ordered_map(insertion_ordered_map &&other) {}; //TODO move-constructor, noexcept?

    insertion_ordered_map &operator=(insertion_ordered_map other) {}; //TODO noexcept?

    bool insert(K const &k, V const &v) {
        prepare_to_modify(false);
        pair_t pair;
        bool unique = true;
        typename map_t::iterator it = data->map->find(k);

        if (it != data->map->end()) {
            unique = false;
            typename list_t::iterator it_list = it->second;
            pair = *it_list;
            data->list->erase(it_list);
        } else {
            pair = std::make_pair(k, v);
        }

        try {
            data->list->push_back(pair);
            data->map->insert({k, --data->list->end()});

            return unique;
        } catch (std::bad_alloc &e) {
            if (!data->list->empty() && data->list->back() == pair)
                data->list->pop_back();
            throw;
        }
    }; //TODO copy on write

    void erase(K const &k) {
        typename map_t::iterator it = data->map->find(k);
        if (it == data->map->end())
            throw lookup_error();
        typename list_t::iterator list_it = it->second;
        data->map->erase(it);
        data->list->erase(list_it);
    }; //TODO handle exceptions thrown by erase, copy on write


    void merge(insertion_ordered_map const &other) {}; //TODO noexcept, copy on write?

    V &at(K const &k) {}; //TODO noexcept?

    V const &at(K const &k) const {}; //TODO noexcept?

    V &operator[](K const &k) {}; //TODO noexcept?

    size_t size() const noexcept {
        return data->list->size();
    };

    bool empty() const noexcept {
        return data->list->empty();
    };

    void clear() noexcept {
        prepare_to_modify(false); //TODO except
        data->list->clear();
        data->map->clear();
    }; //TODO copy on write

    bool contains(K const &k) noexcept {
        return (data->map->find(k) != data->map->end());
    };

};

//    iterator begin() const {
//        return iterator(list.begin());
//    }; //TODO noexcept?
//    iterator end() const {}; //TODO noexcept?

//    class iterator {
//    private:
//        typename list_t::iterator list_iterator;
//    public:
//        iterator() = default;
//        explicit iterator(typename list_t::iterator const &elem) {
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

#endif
