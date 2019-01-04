#include <iostream>
#include <string>
#include <iterator>
#include <functional>
#include <numeric>
#include <vector>
#include <memory>
#include <map>

namespace extstd {

    template<typename Val, typename Key = int, typename Container = std::vector<Val>>
    class partition {
        using Condition = std::function<Key(const Val&)>;

        std::map<Key, std::unique_ptr<Container>> m_map;
        Condition m_condition;

        public:
        partition(const Condition& condition)
            : m_condition(condition) {}

        template<typename Input>
        partition(Input&& data, const Condition& condition)
            : m_condition(condition) {
            for(auto&& v : data) {
                insert(std::move(v));
            }
        }

        template<typename Input>
        partition(Input& data, const Condition& condition)
            : m_condition(condition) {
            for(const auto& v : data) {
                insert(v);
            }
        }

        template<typename Value>
        void insert(Value&& v) {
            auto key = m_condition(v);
            auto it = m_map.lower_bound(key);
            if(it != m_map.end() && it->first == key) {
                it->second->emplace(std::forward<Value>(v));
            } else {
                auto* c = new Container();
                c->emplace(std::forward<Value>(v));
                m_map.emplace_hint(it,
                                    std::make_pair(key, std::unique_ptr<Container>(c))
                                );
            }
        }

        void remove(const Val& v) {
            auto key = m_condition(v);
            auto it = m_map.find(key);
            if(it != m_map.end()) {
                auto& c = *(it->second);
                c.erase(std::remove(c.begin(), c.end(), v), c.end());
                if(c.empty()) {
                    m_map.erase(it);
                }
            }
        }

        std::size_t size() const {
            return std::accumulate(m_map.begin(), m_map.end(), std::size_t(0), [](const auto& sum, const auto& pair) {
                return sum + pair.second->size();
            });
        }

        void for_each(const std::function<void(const Key&, const Val&)>& fun) {
            for(const auto& pair : m_map) {
                for(const auto& v : *pair.second) {
                    fun(pair.first, v);
                }
            }
        }

        void for_each_nth_nonthrow(const std::size_t n,
                          const std::function<void(const Key&, const Val&)>& existFun,
                          const std::function<void(const Key&)>& emptyFun = [](const Key&){}) {
            for(const auto& pair : m_map) {
                auto size = pair.second->size();
                if(size > n) {
                    auto it = std::next(pair.second->begin(), n);
                    existFun(pair.first, *it);
                } else {
                    emptyFun(pair.first);
                }
            }
        }

        void for_each_nth(const std::size_t n,
                          const std::function<void(const Key&, const Val&)>& existFun) {
            for_each_nth_nonthrow(n, existFun, [&n](const Key&) {
                throw std::out_of_range(std::string("Could not find value for index n = ") + std::to_string(n));
            });
        }

        void print() const {
            std::cout << "-- Partitions --" << std::endl;
            for(const auto& m : m_map) {
                std::cout << m.first << ": ";
                std::copy(m.second->begin(), m.second->end(), std::ostream_iterator<Val>(std::cout, " "));
                std::cout << std::endl << std::endl;
            }
            std::cout << "Total size: " << size() << std::endl;
        }
    };

}
