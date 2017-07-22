#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <vector>
#include <memory>
#include <exception>
#include <stdexcept>    //covering g++ version differences

class Creature {
    int m_gene;
    std::pair<size_t, size_t> m_pos;

    public:
    Creature(int gene, std::pair<size_t, size_t> position)
        : m_gene(gene), m_pos(position) {

    }

    size_t getLinearPos(size_t dimensionSize) const {
        return m_pos.second * dimensionSize + m_pos.first;
    }

    const std::pair<size_t, size_t>& getPos() const {
        return m_pos;
    }

    void setPos(const std::pair<size_t, size_t>& position) {
        m_pos = position;
    }

    int getGene() const {
        return m_gene;
    }

    bool operator== (const Creature& cr) const {
        return m_pos == cr.getPos();
    }

    bool operator> (const Creature& cr) const {
        if(m_pos.second > cr.getPos().second) return true;
        if(m_pos.second < cr.getPos().second) return false;
        return m_pos.first > cr.getPos().first;
    }

    bool operator< (const Creature& cr) const {
        if(m_pos.second < cr.getPos().second) return true;
        if(m_pos.second > cr.getPos().second) return false;
        return m_pos.first < cr.getPos().first;
    }

};


class Window : public QWidget
{
    Q_OBJECT

    static constexpr size_t FIELDS = 16;
    static constexpr qreal WSIZE = 1024.0;
    static constexpr qreal FIELD_SIZE = WSIZE / FIELDS;

    static QPointF log2phys(const std::pair<size_t, size_t>& log) {
        return QPointF(log.first, log.second) * FIELD_SIZE - QPointF(WSIZE / 2, WSIZE / 2);
    }

    static QPointF log2center(const std::pair<size_t, size_t>& log) {
        return log2phys(log) + QPointF(FIELD_SIZE / 2, FIELD_SIZE / 2);
    }

    static QRectF log2bounding(const std::pair<size_t, size_t>& log) {
        return QRectF(log2phys(log), QSizeF(FIELD_SIZE, FIELD_SIZE));
    }

    std::vector<std::vector<int>> m_world;

    std::vector<std::unique_ptr<Creature>> m_creatures;

    size_t m_landCnt;

    void extractVisibleSubset(std::vector<Creature*>& subset);

    void drawWorld(QPainter& painter);

    void drawCreatures(QPainter& painter, const std::vector<Creature*>& creaturesPainted);

    void initPainter(QPainter& painter);

    void changePos(Creature* const cr);

    public slots:
        void updateData();

    public:
        Window(QWidget *parent = 0);

    protected:
        void paintEvent(QPaintEvent *event) override;
};

#endif
