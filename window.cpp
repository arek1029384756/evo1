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
        for(std::int32_t i = 0; i < FIELDS; ++i) {
            std::vector<bool> line;
            for(std::int32_t j = 0; j < FIELDS; ++j) {
                bool rnd = std::rand() % 2;
                line.emplace_back(rnd);
            }
            m_world.emplace_back(line);
        }
        m_world[FIELDS / 2][FIELDS / 2] = true;
    }

    void Window::populateWorld() {
        auto* const pP = newPartition();
        pP->insert(Creature(Creature::MAX_GENE / 2, std::make_pair(FIELDS / 2, FIELDS / 2)));
        m_fifoLand.emplace_back(pP);
    }

    Creature::Position Window::getNeighbouringField(const Creature& cr, bool land) const {
        std::vector<Creature::Position> fields;
        for(std::int32_t i = -1; i <= 1; ++i) {
            for(std::int32_t j = -1; j <= 1; ++j) {
                auto pos = cr.getPos();
                pos.first += i;
                pos.second += j;
                pos.first = std::max(0, std::min(FIELDS - 1, pos.first));
                pos.second = std::max(0, std::min(FIELDS - 1, pos.second));

                if(land == isFieldLand(pos)) {
                    fields.emplace_back(pos);
                }
            }
        }

        auto size = fields.size();
        if(size > 0) {
            std::size_t idx = std::rand() % size;
            return fields.at(idx);
        } else {
            return cr.getPos();
        }
    }

    auto Window::changeCreaturePos(Creature& cr) const {
        std::int32_t rnd = std::rand() % (Creature::MAX_GENE + 1);
        bool land = (rnd > cr.getGene()) ? false : true;
        auto pos = getNeighbouringField(cr, land);
        cr.setPos(pos);
        return pos;
    }

    Creature Window::offspring(const Creature& cr) const {
        auto gene = cr.getGene();
        gene += (std::rand() % 3 - 1);
        return Creature(gene, cr.getPos());
    }

    bool Window::isFieldLand(const Creature::Position& pos) const {
        return m_world[pos.second][pos.first];
    }

    bool Window::isCreatureOnLand(const Creature& cr) const {
        return isFieldLand(cr.getPos());
    }

    void Window::deleteDead() {
        m_fifoWater.clear();
        update();
        auto size = m_fifoLand.front()->size();
        if(size == 0) {
            std::exit(1);
        }
        QTimer::singleShot(100, this, SLOT(animate()));
    }

    void Window::animate() {
        auto* const pL = newPartition();
        auto* const pW = newPartition();

        const auto& part = m_fifoLand.front();
        part->for_each([&](const Creature::Position&, const Creature& crx) {
            Creature cr = crx;
            changeCreaturePos(cr);
            auto land = isCreatureOnLand(cr);
            if(land) {
                for(std::size_t i = 0; i < 3; ++i) {
                    pL->insert(offspring(cr));
                }
            } else {
                pW->insert(std::move(cr));
            }
        });

        m_fifoLand.emplace_back(pL);
        m_fifoWater.emplace_back(pW);
        m_fifoLand.pop_front();

        update();
        QTimer::singleShot(100, this, SLOT(deleteDead()));
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

    void Window::drawCreatures(QPainter& painter, const MapFifo& fifo) const {
        if(fifo.empty()) {
            return;
        }

        const auto& part = fifo.front();
        part->for_each_nth(0, [&](const Creature::Position& pos, const Creature& cr) {
            auto center = log2center(pos);
            auto boundingRect = log2bounding(pos);

            auto land = isCreatureOnLand(cr);
            auto colBody = (land) ? Color::gene2Color(cr.getGene()) : Color::deadBody;
            auto colText = (land) ? Color::liveText : Color::deadText;

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
        drawCreatures(painter, m_fifoLand);
        drawCreatures(painter, m_fifoWater);

        painter.setPen(Qt::black);
        painter.drawText(log2phys(std::make_pair(FIELDS + 1, 1)),
                         QString("Creatures: ") + QString::number(m_fifoLand.front()->size()));

    }

}
