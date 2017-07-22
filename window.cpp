#include <QtWidgets>
#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include "window.h"


Window::Window(QWidget *parent)
    : QWidget(parent), m_landCnt(0)
{
    setWindowTitle(tr("Evo1"));
    resize(500, 500);

    std::srand(std::chrono::system_clock::now().time_since_epoch().count());
    for(size_t i = 0; i < FIELDS; ++i) {
        std::vector<int> line;
        for(size_t j = 0; j < FIELDS; ++j) {
            auto rnd = std::rand() % 2;
            if(rnd) {
                ++m_landCnt;
            }
            line.emplace_back(rnd);
        }
        m_world.emplace_back(line);
    }

    for(size_t i = 0; i < 1000000; ++i) {
        auto pC = new Creature(i % 101, std::make_pair(FIELDS / 2, FIELDS / 2));
        m_creatures.emplace_back(std::unique_ptr<Creature>(pC));
    }

    QTimer::singleShot(100, this, SLOT(updateData()));
}

void Window::changePos(Creature* const cr)
{
    auto pos = cr->getPos();
    pos.first += std::rand() % 3 - 1;
    pos.second += std::rand() % 3 - 1;

    if(pos.first >= FIELDS) pos.first = FIELDS - 1;
    if(pos.second >= FIELDS) pos.second = FIELDS - 1;
 
    cr->setPos(pos);
}

void Window::updateData()
{
    for(auto& x : m_creatures) {
        changePos(x.get());
    }
    update();
    QTimer::singleShot(1000, this, SLOT(updateData()));
}

void Window::extractVisibleSubset(std::vector<Creature*>& subset)
{
    size_t visibleNum = 0;
    bool marked[FIELDS * FIELDS] = { false };
    for(const auto& cr : m_creatures) {
        auto pC = cr.get();
        auto idx = pC->getLinearPos(FIELDS);
        if(!marked[idx]) {
            marked[idx] = true;
            subset.emplace_back(pC);
            if(++visibleNum >= m_landCnt) {
                break;
            }
        }
    }
}

void Window::drawWorld(QPainter& painter)
{
    painter.setPen(Qt::black);
    std::pair<size_t, size_t> coord(0, 0);
    for(const auto& line : m_world) {
        coord.first = 0;
        for(const auto& field : line) {
            if(field) {
                painter.setBrush(Qt::black);
            } else {
                painter.setBrush(Qt::cyan);
            }
            auto boundingRect = log2bounding(coord);
            painter.drawRect(boundingRect);
            ++coord.first;
        }
        ++coord.second;
    }
}

void Window::drawCreatures(QPainter& painter, const std::vector<Creature*>& creaturesPainted)
{
    for(const auto cr : creaturesPainted) {
        auto pos = cr->getPos();
        auto center = log2center(pos);
        auto boundingRect = log2bounding(pos);

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        if(cr->getGene() == 13) {
            painter.setBrush(Qt::green);
        }
        painter.drawEllipse(center, 20, 20);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(boundingRect, Qt::AlignCenter, QString::number(cr->getGene()));
    }
}

void Window::initPainter(QPainter& painter)
{
    int side = qMin(width(), height());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / WSIZE, side / WSIZE);
}

void Window::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    initPainter(painter);

    drawWorld(painter);

    std::vector<Creature*> toPaint;
    extractVisibleSubset(toPaint);
    std::cout << "landCnt: " << m_landCnt << ",  size[1]: " << toPaint.size() << std::endl;
    drawCreatures(painter, toPaint);
}
