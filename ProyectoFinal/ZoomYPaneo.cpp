#include "CustomChartView.h"

CustomChartView::CustomChartView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    setRubberBand(QChartView::NoRubberBand); 
    setRenderHint(QPainter::Antialiasing); 
    setMouseTracking(true); 
}

void CustomChartView::setLabelCoordenadas(QLabel* label) {
    m_labelCoordenadas = label;
}

void CustomChartView::resizeEvent(QResizeEvent *event) {
    QChartView::resizeEvent(event);
    this->viewport()->update();
}

void CustomChartView::wheelEvent(QWheelEvent *event) {
    qreal factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    chart()->zoom(factor);
    event->accept(); 
}

void CustomChartView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor); 
        event->accept();
    } else {
        QChartView::mousePressEvent(event);
    }
}

void CustomChartView::mouseMoveEvent(QMouseEvent *event) {
    if (m_labelCoordenadas) {
        QPointF mathPoint = chart()->mapToValue(event->pos());
        m_labelCoordenadas->setText(QString("X: %1\nY: %2")
                                    .arg(mathPoint.x(), 0, 'f', 3)
                                    .arg(mathPoint.y(), 0, 'f', 3));
    }

    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        chart()->scroll(-delta.x(), delta.y());
        event->accept();
    } else {
        QChartView::mouseMoveEvent(event);
    }
}

void CustomChartView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor); 
        event->accept();
    } else {
        QChartView::mouseReleaseEvent(event);
    }
}
