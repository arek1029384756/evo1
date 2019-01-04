#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <utility>

namespace gui {

    class Creature {

        public:
        using Position = std::pair<std::int32_t, std::int32_t>;

        static constexpr std::int32_t MAX_GENE = 100;

        Creature(std::int32_t gene, const Position& position)
            : m_gene(std::max(0, std::min(MAX_GENE, gene))),
              m_pos(position) {}

        const auto& getPos() const {
            return m_pos;
        }

        void setPos(const Position& position) {
            m_pos = position;
        }

        auto getGene() const {
            return m_gene;
        }

        bool operator>(const Creature& other) const {
            return (m_gene > other.getGene());
        }

        bool operator<(const Creature& other) const {
            return (m_gene < other.getGene());
        }

        private:
        std::int32_t m_gene;
        Position m_pos;

    };

}

#endif
