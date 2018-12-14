#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <utility>

namespace gui {

    class Creature {
        std::int32_t m_gene;
        std::pair<std::size_t, std::size_t> m_pos;

        public:
        Creature(std::int32_t gene, const std::pair<std::size_t, std::size_t>& position)
            : m_gene(gene), m_pos(position) {}

        const auto& getPos() const {
            return m_pos;
        }

        void setPos(const std::pair<std::size_t, std::size_t>& position) {
            m_pos = position;
        }

        auto getGene() const {
            return m_gene;
        }
    };

}

#endif
