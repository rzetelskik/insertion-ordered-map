#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <unordered_map>
#include <list>
#include <limits>
#include <memory>

const static size_t unshareable = std::numeric_limits<size_t>::max();

class lookup_error : std::exception {
    [[nodiscard]] const char *what() const noexcept override {
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
    private:
        void transform_list_to_map() {
            for (typename list_t::iterator it = list->begin(); it != list->end(); ++it) {
                map->insert({it->first, it});
            }
        }

    public: //TODO handle this
        std::unique_ptr<map_t> map;
        std::unique_ptr<list_t> list;
        size_t ref_count;

        data_t() : map(std::make_unique<map_t>()), list(std::make_unique<list_t>()), ref_count(1) {};

        data_t(data_t const &other) : ref_count(1) {
            list = std::make_unique<list_t>(*other.list);
            map = std::make_unique<map_t>();
            transform_list_to_map();
        };

        ~data_t() = default;
    };

    std::shared_ptr<data_t> data;

    void prepare_to_modify(bool mark_unshareable) {
        if (data->ref_count > 1 && data->ref_count != unshareable) {
            std::shared_ptr<data_t> prev_data = data;

            data = std::make_shared<data_t>(*data);
            prev_data->ref_count--;
        }

        data->ref_count = mark_unshareable ? unshareable : 1;
    }

public:
    using iterator = typename list_t::const_iterator;

    insertion_ordered_map() : data(std::make_shared<data_t>()) {};

    insertion_ordered_map(insertion_ordered_map const &other) {
        if (other.data->ref_count == unshareable) {
            data = std::make_shared<data_t>(*other.data);
        } else {
            data = other.data;
            ++data->ref_count;
        }
    };

    ~insertion_ordered_map() {
        --data->ref_count;
    };

    insertion_ordered_map(insertion_ordered_map &&other) noexcept : data(move(other.data)) {}

    insertion_ordered_map &operator=(insertion_ordered_map other) {
        if (this->data != other.data) { //TODO sprawdzic to
            data = std::make_shared<data_t>(*other.data);
        }
        return *this;
    };

    /**
     * prepare to modify moze wyjebac, ale to git, i tak ma silna gwarancje
     * map->find ma silna gwarancje ✔️
     * map->end ani list->erase nie throwuje
     *
     * jesli jebnie list->push_back to dowiemy sie o tym, bo lista bedzie pusta lub na jej koncu nie bedzie dodawanego elementu
     * (bo jeblo wiec sie nie dodal) i wtedy nic nie trzeba robic
     *
     * jesli jebnie map->insert, to tez sie o tym dowiemy, bo na koncu listy bedzie dodawany element (bo lista nie jebla skoro jebla mapa)
     * i wtedy usuniemy ten dodany element z konca lista
     */
    // FIXME WARNING !!!! WARNING !!!! Para jest usuwana z listy poza throwem. Jesli w try catchu wyjebie sie lista to ten element nie zostanie przywrocony
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
            pair = {k, v};
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
    };

    /**
     * prepare_to_modify moze jebnac i niech jebnie
     * map->find ma silna gwarancje (a silna + no_throw = silna)
     * list::erase nie throwuje (w najgorszym przypadku undefined behaviour ale na to wplywu nie mamy)
     * map::erase moze jebnac ale tylko przez comparision object kontenera (ale co znaczy hmmm), w pozostalych przypadkach no-throw
     * zostaje lookup_error ✔️
     */
    //TODO jak wyzej, kiedy moze jebnac comparision object kontenera w naszym przypadku
    void erase(K const &k) {
        prepare_to_modify(false);

        typename map_t::iterator it = data->map->find(k);
        if (it == data->map->end())
            throw lookup_error();

        typename list_t::iterator list_it = it->second;
        data->map->erase(it);
        data->list->erase(list_it);
    }; //TODO handle exceptions thrown by erase

//    void merge(insertion_ordered_map const &other)
//    {
//        if (this->data != other->data)
//        {
//            insertion_ordered_map temp(this);
//            //TODO to be continued
//
//        }
//    }; //TODO noexcept, copy on write?

    V const &at(K const &k) const {
        typename map_t::iterator it = data->map->find(k);

        if (it == data->map->end())
            throw lookup_error();

        return it->second->second;
    };

    V &at(K const &k) {
        return const_cast<V &>(const_cast<const insertion_ordered_map *>(this)->at(k));
    };

    /**
     * Jak jebnie prepare_to_modify to trudno, tak ma byc
     * map->find ma silna gwarancje
     * map->end nie throwuje
     * jak list->push_back jebnie, to nic sie nie zmodyfikuje
     *
     * jak map->insert jebnie, to znaczy, ze do listy cos wczesniej zostalo dodane
     * wtedy dowiemy sie o tej zmianie, bo dodany element bedzie na koncu listy i go usuniemy.
     * wiec fajno.
     */
    V &operator[](K const &k) {
        prepare_to_modify(true);

        pair_t pair;
        typename map_t::iterator it = data->map->find(k);

        if (it == data->map->end()) {
            try {
                pair = {k, V()}; //TODO except, ma dzialac tylko jesli konstruktor bezparametrowy

                data->list->push_back(pair);
                typename list_t::iterator it_list = --data->list->end();
                data->map->insert({k, it_list});

                return it_list->second;
            } catch (std::bad_alloc &e) {
                if (!data->list->empty() && data->list->back() == pair)
                    data->list->pop_back();
                throw;
            }
        }

        return it->second->second;
    };

    [[nodiscard]] size_t size() const noexcept {
        return data->list->size();
    };

    [[nodiscard]] bool empty() const noexcept {
        return data->list->empty();
    };

    void clear() {
        prepare_to_modify(false);
        data->list->clear();
        data->map->clear();
    };

    [[nodiscard]] bool contains(K const &k) noexcept {
        return (data->map->find(k) != data->map->end());
    };

    iterator begin() const noexcept {
        return iterator(data->list->begin());
    }

    iterator end() const noexcept {
        return iterator(data->list->end());
    }
};

#endif
