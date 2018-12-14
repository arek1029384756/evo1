#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <vector>
#include <list>
#include <memory>
#include "creature.hpp"

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
        };

        static constexpr std::size_t FIELDS = 16;
        static constexpr qreal WSIZE = 1024.0;
        static constexpr qreal FIELD_SIZE = WSIZE / FIELDS;

        static QPointF log2phys(const std::pair<std::size_t, std::size_t>& log) {
            return QPointF(log.first, log.second) * FIELD_SIZE - QPointF(WSIZE / 2, WSIZE / 2);
        }

        static QPointF log2center(const std::pair<std::size_t, std::size_t>& log) {
            return log2phys(log) + QPointF(FIELD_SIZE / 2, FIELD_SIZE / 2);
        }

        static QRectF log2bounding(const std::pair<std::size_t, std::size_t>& log) {
            return QRectF(log2phys(log), QSizeF(FIELD_SIZE, FIELD_SIZE));
        }

        using CreaturesContainer = std::list<Creature>;
        using CreaturesMap = std::map<std::pair<std::size_t, std::size_t>, std::unique_ptr<CreaturesContainer>>;
        using MapFifo = std::list<std::unique_ptr<CreaturesMap>>;

        MapFifo m_fifoLand;
        MapFifo m_fifoWater;

        std::vector<std::vector<bool>> m_world;

        void createWorld();
        void populateWorld();
        void drawWorld(QPainter& painter) const;
        void drawCreatures(QPainter& painter, const MapFifo& fifo, QColor colBody, QColor colText) const;
        void initPainter(QPainter& painter) const;
        auto changeCreaturePos(Creature& cr) const;
        bool isCreatureOnLand(const Creature& cr) const;

        public slots:
            void animate();

        public:
            Window(QWidget *parent = 0);

        protected:
            void paintEvent(QPaintEvent *event) override;
    };

}

#endif
