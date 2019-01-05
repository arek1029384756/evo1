#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <vector>
#include <list>
#include <set>
#include <memory>
#include <cmath>
#include "creature.hpp"
#include "partitions.hpp"

namespace gui {

    class Window : public QWidget
    {
        Q_OBJECT

        struct Color {
            inline static const QColor land = Qt::black;
            inline static const QColor water = Qt::cyan;
            inline static const QColor liveBody = Qt::red;
            inline static const QColor deadBody = Color::water;
            inline static const QColor liveText = Qt::white;
            inline static const QColor deadText = Qt::black;
            inline static const QColor outline = Color::land;

            template<typename TGene>
            static QColor gene2Color(TGene gene) {
                static auto gauss = [](auto gene, auto mean) -> int {
                    constexpr qreal sigma2 = qreal(3) * Creature::MAX_GENE;
                    auto exponent = -std::pow((gene - mean), 2) / (sigma2 * 2);
                    auto g = qreal(255) * std::exp(exponent);
                    return std::round(g);
                };

                return QColor(gauss(gene, Creature::MAX_GENE),
                              gauss(gene, Creature::MAX_GENE / 2),
                              gauss(gene, 0));
            }
        };

        static constexpr std::int32_t FIELDS = 16;
        static constexpr qreal WSIZE = 1024.0;
        static constexpr qreal FIELD_SIZE = WSIZE / FIELDS;

        static QPointF log2phys(const Creature::Position& log) {
            return QPointF(log.first, log.second) * FIELD_SIZE - QPointF(WSIZE / 2, WSIZE / 2);
        }

        static QPointF log2center(const Creature::Position& log) {
            return log2phys(log) + QPointF(FIELD_SIZE / 2, FIELD_SIZE / 2);
        }

        static QRectF log2bounding(const Creature::Position& log) {
            return QRectF(log2phys(log), QSizeF(FIELD_SIZE, FIELD_SIZE));
        }

        using CreaturesPartition = extstd::partition<Creature, Creature::Position, std::set<Creature, std::greater<Creature>>>;
        //using CreaturesPartition = extstd::partition<Creature, Creature::Position, std::list<Creature>>;
        using MapFifo = std::list<std::unique_ptr<CreaturesPartition>>;

        MapFifo m_fifoLand;
        MapFifo m_fifoWater;

        std::vector<std::vector<bool>> m_world;

        auto* newPartition() const;
        void createWorld();
        void populateWorld();
        void drawWorld(QPainter& painter) const;
        void drawCreatures(QPainter& painter, const MapFifo& fifo) const;
        void initPainter(QPainter& painter) const;
        Creature::Position getNeighbouringField(const Creature& cr, bool land) const;
        Creature offspring(const Creature& cr) const;
        auto changeCreaturePos(Creature& cr) const;
        bool isFieldLand(const Creature::Position& pos) const;
        bool isCreatureOnLand(const Creature& cr) const;

        public slots:
            void animate();
            void deleteDead();

        public:
            Window(QWidget *parent = 0);

        protected:
            void paintEvent(QPaintEvent *event) override;
    };

}

#endif
