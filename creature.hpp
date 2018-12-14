#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <utility>

namespace gui {

    class Creature {

        public:
        using Position = std::pair<std::size_t, std::size_t>;

        Creature(std::int32_t gene, const Position& position)
            : m_gene(gene), m_pos(position) {}

        const auto& getPos() const {
            return m_pos;
        }

        void setPos(const Position& position) {
            m_pos = position;
        }

        auto getGene() const {
            return m_gene;
        }

        private:
        std::int32_t m_gene;
        Position m_pos;

    };

}

#endif
