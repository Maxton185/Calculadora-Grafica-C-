#ifndef ZOOMYPANEO_H
#define ZOOMYPANEO_H

#include <QChartView>
#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>

using namespace QtCharts;

class ZoomYPaneo : public QChartView {
    Q_OBJECT 
private:
    bool m_isPanning = false;
    QPoint m_lastMousePos;
    QLabel* m_labelCoordenadas = nullptr; 

protected:
    void resizeEvent(QResizeEvent event) override;
    void wheelEvent(QWheelEventevent) override;
    void mousePressEvent(QMouseEvent event) override;
    void mouseMoveEvent(QMouseEventevent) override;
    void mouseReleaseEvent(QMouseEvent event) override;

public:
    explicit ZoomYPaneo(QChartchart, QWidget parent = nullptr);
    void setLabelCoordenadas(QLabel label);
};

#endif // ZOOMYPANEO_H
