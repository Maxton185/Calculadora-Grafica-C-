#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// Componentes de la Interfaz Gráfica (Qt Base)
#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>

// Componentes del Gráfico (Qt Charts)
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QPainter>
#include <QPen>
#include <QLegendMarker> 

// Componentes de Eventos de Dispositivos de Entrada
#include <QWheelEvent>
#include <QMouseEvent>

// Librería del Motor Matemático Parsers
#include "exprtk.hpp"

using namespace QtCharts;

// ============================================================================
// SUBCLASE PERSONALIZADA DE QCHARTVIEW (CONTROL INTERACTIVO)
// ============================================================================
class CustomChartView : public QChartView {
private:
    bool m_isPanning = false;   
    QPoint m_lastMousePos;      
    QLabel* m_labelCoordenadas = nullptr; 

protected:
    void wheelEvent(QWheelEvent *event) override {
        qreal factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
        chart()->zoom(factor);
        event->accept(); 
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::RightButton) {
            m_isPanning = true;
            m_lastMousePos = event->pos();
            setCursor(Qt::ClosedHandCursor); 
            event->accept();
        } else {
            QChartView::mousePressEvent(event);
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
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

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::RightButton) {
            m_isPanning = false;
            setCursor(Qt::ArrowCursor); 
            event->accept();
        } else {
            QChartView::mouseReleaseEvent(event);
        }
    }

public:
    CustomChartView(QChart *chart, QWidget *parent = nullptr) : QChartView(chart, parent) {
        setRubberBand(QChartView::NoRubberBand); 
        setRenderHint(QPainter::Antialiasing); 
        setMouseTracking(true); 
    }

    void setLabelCoordenadas(QLabel* label) {
        m_labelCoordenadas = label;
    }
};

// ============================================================================
// CLASE PRINCIPAL DE LA APLICACIÓN
// ============================================================================
class WindowGraficadora : public QWidget {
private:
    QDoubleSpinBox  *spinMinX;
    QDoubleSpinBox  *spinMaxX;
    QLineEdit       *editFuncion;
    QListWidget     *listaFunciones;
    
    QPushButton     *btnAgregar;
    QPushButton     *btnEliminarSeleccionada; 
    QPushButton     *btnLimpiar;
    QPushButton     *btnCentrar;              
    QPushButton     *btnGraficar;
    
    QLabel          *lblCoordenadas;          

    QChart          *chart;
    CustomChartView *chartView;
    QValueAxis      *axisX;
    QValueAxis      *axisY;

public:
    WindowGraficadora(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Graficadora Científica Pro - Cuadrícula Estilo GeoGebra");
        resize(1250, 750);

        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        QVBoxLayout *sidebar = new QVBoxLayout();
        sidebar->setSpacing(8);

        sidebar->addWidget(new QLabel("<b>Rango Inicial (X):</b>"));
        
        QHBoxLayout *rangoLayout = new QHBoxLayout();
        spinMinX = new QDoubleSpinBox();
        spinMinX->setRange(-10000.0, 10000.0);
        spinMinX->setValue(-10.0);
        rangoLayout->addWidget(new QLabel("Min:"));
        rangoLayout->addWidget(spinMinX);

        spinMaxX = new QDoubleSpinBox();
        spinMaxX->setRange(-10000.0, 10000.0);
        spinMaxX->setValue(10.0);
        rangoLayout->addWidget(new QLabel("Max:"));
        rangoLayout->addWidget(spinMaxX);
        sidebar->addLayout(rangoLayout);

        sidebar->addWidget(new QLabel("<br><b>Ingresar Expresión:</b>"));
        editFuncion = new QLineEdit("x^2");
        sidebar->addWidget(editFuncion);

        btnAgregar = new QPushButton("Añadir a la Lista");
        sidebar->addWidget(btnAgregar);

        sidebar->addWidget(new QLabel("<br><b>Funciones Cargadas:</b>"));
        listaFunciones = new QListWidget();
        sidebar->addWidget(listaFunciones);

        QHBoxLayout *limpiezaLayout = new QHBoxLayout();
        btnEliminarSeleccionada = new QPushButton("Eliminar 1");
        btnLimpiar = new QPushButton("Limpiar Todo");
        limpiezaLayout->addWidget(btnEliminarSeleccionada);
        limpiezaLayout->addWidget(btnLimpiar);
        sidebar->addLayout(limpiezaLayout);

        btnGraficar = new QPushButton("¡GRAFICAR!");
        btnGraficar->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; font-size: 13px; padding: 10px;");
        sidebar->addWidget(btnGraficar);

        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        sidebar->addWidget(line);

        btnCentrar = new QPushButton("Centrar en Origen (0,0)");
        sidebar->addWidget(btnCentrar);

        sidebar->addWidget(new QLabel("<b>Tracking en Tiempo Real:</b>"));
        lblCoordenadas = new QLabel("X: 0.000\nY: 0.000");
        lblCoordenadas->setStyleSheet("background-color: #1e1e1e; color: #00ff00; font-family: monospace; font-size: 14px; padding: 10px; border-radius: 5px;");
        sidebar->addWidget(lblCoordenadas);

        sidebar->addStretch();

        chart = new QChart();
        chart->setTitle("Lienzo Matemático Activo");
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        chartView = new CustomChartView(chart);
        chartView->setLabelCoordenadas(lblCoordenadas); 

        // ====================================================================
        // CONFIGURACIÓN DE EJES DINÁMICOS (ESTILO GEOGEBRA)
        // ====================================================================
        axisX = new QValueAxis();
        axisX->setTitleText("Eje X");
        axisX->setTickType(QValueAxis::TicksDynamic);
        axisX->setTickAnchor(0.0); // El origen siempre será una línea de rejilla

        axisY = new QValueAxis();
        axisY->setTitleText("Eje Y");
        axisY->setTickType(QValueAxis::TicksDynamic);
        axisY->setTickAnchor(0.0); // El origen siempre será una línea de rejilla

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);

        mainLayout->addLayout(sidebar, 1);
        mainLayout->addWidget(chartView, 4);

        // Algoritmo matemático para calcular intervalos perfectos (1, 2, 5, 10, etc.) al cambiar el zoom o paneo
        auto optimizarSubdivisiones = [](QValueAxis *axis) {
            double amplitud = axis->max() - axis->min();
            if (amplitud <= 0) return;
            
            // Buscamos tener aproximadamente entre 10 y 12 subdivisiones visibles en pantalla
            double intervaloCrudo = amplitud / 10.0;
            double logBase10 = std::log10(intervaloCrudo);
            double potenciaDe10 = std::pow(10.0, std::floor(logBase10));
            double proporcion = intervaloCrudo / potenciaDe10;
            
            double pasoLimpio;
            if (proporcion < 1.5)       pasoLimpio = 1.0 * potenciaDe10;
            else if (proporcion < 3.5)  pasoLimpio = 2.0 * potenciaDe10;
            else if (proporcion < 7.5)  pasoLimpio = 5.0 * potenciaDe10;
            else                        pasoLimpio = 10.0 * potenciaDe10;
            
            axis->setTickInterval(pasoLimpio);
        };

        // Conectar las señales de cambio de rango para recalcular la rejilla en tiempo real
        connect(axisX, &QValueAxis::rangeChanged, [this, optimizarSubdivisiones](qreal min, qreal max) {
            optimizarSubdivisiones(this->axisX);
        });
        
        connect(axisY, &QValueAxis::rangeChanged, [this, optimizarSubdivisiones](qreal min, qreal max) {
            optimizarSubdivisiones(this->axisY);
        });

        // Conexión de señales de botones
        connect(btnAgregar, &QPushButton::clicked, [this]() {
            QString formula = editFuncion->text().trimmed();
            if(!formula.isEmpty()) {
                listaFunciones->addItem(formula);
                editFuncion->clear();
                editFuncion->setFocus();
            }
        });

        connect(btnEliminarSeleccionada, &QPushButton::clicked, [this]() {
            int row = listaFunciones->currentRow();
            if (row >= 0) {
                delete listaFunciones->takeItem(row);
                if (listaFunciones->count() > 0) {
                    this->procesarYRenderizar();
                } else {
                    chart->removeAllSeries();
                }
            } else {
                QMessageBox::information(this, "Selección", "Haz clic en una función de la lista para eliminarla.");
            }
        });

        connect(btnLimpiar, &QPushButton::clicked, [this]() {
            listaFunciones->clear();
            chart->removeAllSeries();
            axisX->setRange(-10, 10);
            axisY->setRange(-10, 10);
        });

        connect(btnCentrar, &QPushButton::clicked, [this]() {
            double anchoActual = axisX->max() - axisX->min();
            double altoActual = axisY->max() - axisY->min();
            axisX->setRange(-anchoActual / 2.0, anchoActual / 2.0);
            axisY->setRange(-altoActual / 2.0, altoActual / 2.0);
        });

        connect(btnGraficar, &QPushButton::clicked, [this]() {
            this->procesarYRenderizar();
        });
    }

private:
    void procesarYRenderizar() {
        chart->removeAllSeries();

        int totalFunciones = listaFunciones->count();
        if (totalFunciones == 0) return;

        double minX = spinMinX->value();
        double maxX = spinMaxX->value();

        if (minX >= maxX) {
            QMessageBox::critical(this, "Error", "El límite X mínimo no puede ser mayor o igual al máximo.");
            return;
        }

        // Trazado de Ejes Estilo GeoGebra
        QPen penEjes(QColor("#2c3e50"));
        penEjes.setWidth(3);

        QLineSeries *ejeXReal = new QLineSeries();
        ejeXReal->append(-1000000.0, 0.0);
        ejeXReal->append(1000000.0, 0.0);
        ejeXReal->setPen(penEjes);
        chart->addSeries(ejeXReal);
        ejeXReal->attachAxis(axisX);
        ejeXReal->attachAxis(axisY);
        if (!chart->legend()->markers(ejeXReal).isEmpty()) {
            chart->legend()->markers(ejeXReal).first()->setVisible(false);
        }

        QLineSeries *ejeYReal = new QLineSeries();
        ejeYReal->append(0.0, -1000000.0);
        ejeYReal->append(0.0, 1000000.0);
        ejeYReal->setPen(penEjes);
        chart->addSeries(ejeYReal);
        ejeYReal->attachAxis(axisX);
        ejeYReal->attachAxis(axisY);
        if (!chart->legend()->markers(ejeYReal).isEmpty()) {
            chart->legend()->markers(ejeYReal).first()->setVisible(false);
        }

        double globalMinY = 0.0;
        double globalMaxY = 0.0;
        bool esPrimerPuntoGlobal = true;

        const QList<QColor> paletaColores = {
            QColor("#e74c3c"), // Rojo
            QColor("#3498db"), // Azul
            QColor("#2ecc71"), // Verde
            QColor("#9b59b6"), // Morado
            QColor("#e67e22"), // Naranja
            QColor("#1abc9c"), // Turquesa
            QColor("#f1c40f")  // Amarillo
        };

        for (int k = 0; k < totalFunciones; ++k) {
            QString qstrFormula = listaFunciones->item(k)->text();
            std::string ecuacion = qstrFormula.toStdString();

            QLineSeries *serieCurva = new QLineSeries();
            serieCurva->setName(qstrFormula);

            QColor colorActual = paletaColores[k % paletaColores.size()];
            QPen penCurva = serieCurva->pen();
            penCurva.setWidth(2);
            penCurva.setColor(colorActual);
            serieCurva->setPen(penCurva);

            listaFunciones->item(k)->setForeground(colorActual);

            double x_variable;
            exprtk::symbol_table<double> symbol_table;
            symbol_table.add_variable("x", x_variable);
            symbol_table.add_constants();

            exprtk::expression<double> expression;
            expression.register_symbol_table(symbol_table);

            exprtk::parser<double> parser;
            if (!parser.compile(ecuacion, expression)) {
                QMessageBox::critical(this, "Error Sintáctico", "Error en la expresión: " + qstrFormula);
                delete serieCurva;
                listaFunciones->item(k)->setForeground(QColor("#7f8c8d"));
                continue;
            }

            double delta = (maxX - minX) / 1200.0;
            if (delta <= 0) delta = 0.01;

            for (x_variable = minX; x_variable <= maxX + (delta / 2.0); x_variable += delta) {
                double y_calculado = expression.value();
                if (!std::isfinite(y_calculado)) continue; 

                serieCurva->append(x_variable, y_calculado);

                if (x_variable <= maxX) {
                    if (esPrimerPuntoGlobal) {
                        globalMinY = y_calculado;
                        globalMaxY = y_calculado;
                        esPrimerPuntoGlobal = false;
                    } else {
                        if (y_calculado < globalMinY) globalMinY = y_calculado;
                        if (y_calculado > globalMaxY) globalMaxY = y_calculado;
                    }
                }
            }
            chart->addSeries(serieCurva);
            serieCurva->attachAxis(axisX);
            serieCurva->attachAxis(axisY);
        }

        axisX->setRange(minX, maxX);
        double holguraY = (globalMaxY - globalMinY) * 0.1;
        if (holguraY == 0.0) holguraY = 1.0;
        axisY->setRange(globalMinY - holguraY, globalMaxY + holguraY);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WindowGraficadora ventanaPrincipal;
    ventanaPrincipal.show();
    return app.exec();
}
