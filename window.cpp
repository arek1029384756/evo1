#include <QtWidgets>
#include <iostream>
#include <algorithm>
#include <chrono>
#include "window.h"

namespace gui {

    Window::Window(QWidget *parent)
        : QWidget(parent) {
        setWindowTitle(tr("Evo1"));
        resize(500, 500);

        createWorld();
        populateWorld();

        QTimer::singleShot(100, this, SLOT(animate()));
    }

    auto* Window::newPartition() const {
        return new CreaturesPartition([](const Creature& x) {
            return x.getPos();
        });
    }

    void Window::createWorld() {
        std::srand(std::chrono::system_clock::now().time_since_epoch().count());
        for(std::size_t i = 0; i < FIELDS; ++i) {
            std::vector<bool> line;
            for(std::size_t j = 0; j < FIELDS; ++j) {
                bool rnd = std::rand() % 2;
                line.emplace_back(rnd);
            }
            m_world.emplace_back(line);
        }
        m_world[FIELDS / 2][FIELDS / 2] = true;
    }

    void Window::populateWorld() {
        auto* const pP = newPartition();
        for(std::size_t i = 0; i < 10000000; ++i) {
            auto c = Creature(i % 101, std::make_pair(FIELDS / 2, FIELDS / 2));
            pP->insert(c);
        }

        m_fifoLand.emplace_back(pP);
    }

    auto Window::changeCreaturePos(Creature& cr) const {
        auto pos = cr.getPos();
        pos.first += std::rand() % 3 - 1;
        pos.second += std::rand() % 3 - 1;

        if(pos.first >= FIELDS) pos.first = FIELDS - 1;
        if(pos.second >= FIELDS) pos.second = FIELDS - 1;

        cr.setPos(pos);
        return pos;
    }

    bool Window::isCreatureOnLand(const Creature& cr) const {
        auto pos = cr.getPos();
        return m_world[pos.second][pos.first];
    }

    void Window::deleteDead() {
        m_fifoWater.clear();
        update();
        QTimer::singleShot(1000, this, SLOT(animate()));
    }

    void Window::animate() {
        auto* const pL = newPartition();
        auto* const pW = newPartition();

        const auto& part = m_fifoLand.front();
        part->for_each([&](const Creature::Position&, Creature& cr) {
            changeCreaturePos(cr);
            auto land = isCreatureOnLand(cr);
            auto* const pP = (land) ? pL : pW;
            pP->insert(cr);
        });

        m_fifoLand.emplace_back(pL);
        m_fifoWater.emplace_back(pW);
        m_fifoLand.pop_front();

        update();
        QTimer::singleShot(1000, this, SLOT(deleteDead()));
    }

    void Window::drawWorld(QPainter& painter) const {
        painter.setPen(Color::land);
        Creature::Position coord(0, 0);
        for(const auto& line : m_world) {
            coord.first = 0;
            for(const auto& field : line) {
                if(field) {
                    painter.setBrush(Color::land);
                } else {
                    painter.setBrush(Color::water);
                }
                auto boundingRect = log2bounding(coord);
                painter.drawRect(boundingRect);
                ++coord.first;
            }
            ++coord.second;
        }
    }

    void Window::drawCreatures(QPainter& painter, const MapFifo& fifo, QColor colBody, QColor colText) const {
        if(fifo.empty()) {
            return;
        }

        const auto& part = fifo.front();
        part->for_each_nth(0, [&](const Creature::Position& pos, Creature& cr) {
            auto center = log2center(pos);
            auto boundingRect = log2bounding(pos);

            painter.setPen(Color::outline);
            painter.setBrush(colBody);
            painter.drawEllipse(center, FIELD_SIZE / 3, FIELD_SIZE / 3);

            painter.setPen(colText);
            painter.setFont(QFont("Arial", 14, QFont::Bold));
            painter.drawText(boundingRect,
                             Qt::AlignCenter,
                             QString::number(cr.getGene()));

        });
    }

    void Window::initPainter(QPainter& painter) const {
        auto side = qMin(width(), height());
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(width() / 2, height() / 2);
        painter.scale(side / WSIZE, side / WSIZE);
    }

    void Window::paintEvent(QPaintEvent *) {
        QPainter painter(this);
        initPainter(painter);

        drawWorld(painter);
        drawCreatures(painter, m_fifoLand, Color::liveBody, Color::liveText);
        drawCreatures(painter, m_fifoWater, Color::deadBody, Color::deadText);
    }

}
