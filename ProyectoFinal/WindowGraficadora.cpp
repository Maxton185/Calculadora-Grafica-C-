#include "WindowGraficadora.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLineSeries>
#include <QScatterSeries> 
#include <QPainter>
#include <QPen>
#include <QLegendMarker> 
#include <QMenu>       
#include <QAction>     
#include <QCoreApplication>
#include <cmath>
#include <vector>
#include <string>

// Tus librerías externas
#include "CambiarTema.h" 
#include "exprtk.hpp"

WindowGraficadora::WindowGraficadora(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Graficadora Pro - Análisis Numérico");
    resize(1250, 750);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *sidebar = new QVBoxLayout();
    sidebar->setSpacing(8);

    // ==========================================
    // CAMBIO DE TEMA
    // ==========================================
    QPushButton *btnTema = new QPushButton("Cambiar Tema Visual");
    QMenu *menuTemas = new QMenu(this);

    QAction *actClaro = new QAction("Modo Claro (Por defecto)", this);
    QAction *actOscuro = new QAction("Modo Oscuro", this);
    QAction *actGrises = new QAction("Escala de Grises", this);
    QAction *actSolarizado = new QAction("Modo Solarizado", this);

    menuTemas->addAction(actClaro);
    menuTemas->addAction(actOscuro);
    menuTemas->addAction(actGrises);
    menuTemas->addAction(actSolarizado);

    btnTema->setMenu(menuTemas);
    sidebar->addWidget(btnTema);
    
    connect(actClaro, &QAction::triggered, [this]() {
        ThemeManager::aplicarTema(ThemeManager::Claro);
        ThemeManager::aplicarTemaGrafico(ThemeManager::Claro, this->chart, this->axisX, this->axisY);
    });
    
    connect(actOscuro, &QAction::triggered, [this]() {
        ThemeManager::aplicarTema(ThemeManager::Oscuro);
        ThemeManager::aplicarTemaGrafico(ThemeManager::Oscuro, this->chart, this->axisX, this->axisY);
    });
    
    connect(actGrises, &QAction::triggered, [this]() {
        ThemeManager::aplicarTema(ThemeManager::EscalaGrises);
        ThemeManager::aplicarTemaGrafico(ThemeManager::EscalaGrises, this->chart, this->axisX, this->axisY);
    });
    
    connect(actSolarizado, &QAction::triggered, [this]() {
        ThemeManager::aplicarTema(ThemeManager::Solarizado);
        ThemeManager::aplicarTemaGrafico(ThemeManager::Solarizado, this->chart, this->axisX, this->axisY);
    });

    // ==========================================
    // CONTROLES DE INTERFAZ
    // ==========================================
    sidebar->addWidget(new QLabel("<b>Rango de Análisis (X):</b>"));
    
    QHBoxLayout *rangoLayout = new QHBoxLayout();
    spinMinX = new QDoubleSpinBox();
    spinMinX->setRange(-10000.0, 10000.0);
    spinMinX->setValue(-10.0);
    rangoLayout->addWidget(new QLabel("Min:"));
    rangoLayout->addWidget(spinMinX);

    spinMaxX = new QDoubleSpinBox();
    spinMaxX->setRange(-10000.0, 10000.0);
    spinMaxX->setValue(10.0);
    rangoLayout->addWidget(new QLabel("Máx:"));
    rangoLayout->addWidget(spinMaxX);
    sidebar->addLayout(rangoLayout);

    sidebar->addWidget(new QLabel("<br><b>Ingresar Expresión:</b>"));
    editFuncion = new QLineEdit("x^2 - 4"); 
    sidebar->addWidget(editFuncion);

    btnAgregar = new QPushButton("Añadir a la Lista");
    sidebar->addWidget(btnAgregar);

    sidebar->addWidget(new QLabel("<br><b>Funciones Cargadas:</b>"));
    listaFunciones = new QListWidget();
    sidebar->addWidget(listaFunciones);

    QHBoxLayout *limpiezaLayout = new QHBoxLayout();
    btnEliminarSeleccionada = new QPushButton("Eliminar Selección");
    btnLimpiar = new QPushButton("Limpiar Todo");
    limpiezaLayout->addWidget(btnEliminarSeleccionada);
    limpiezaLayout->addWidget(btnLimpiar);
    sidebar->addLayout(limpiezaLayout);

    btnGraficar = new QPushButton("GRAFICAR");
    btnGraficar->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; font-size: 13px; padding: 10px;");
    sidebar->addWidget(btnGraficar);

    btnRaices = new QPushButton("Encontrar Raíces Exactas");
    btnRaices->setStyleSheet("background-color: #c0392b; color: white; font-weight: bold; font-size: 12px; padding: 8px;");
    sidebar->addWidget(btnRaices);

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

    // ==========================================
    // CONFIGURACIÓN DEL GRÁFICO
    // ==========================================
    chart = new QChart();
    chart->setTitle("Lienzo Matemático Activo");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new CustomChartView(chart);
    chartView->setLabelCoordenadas(lblCoordenadas); 

    axisX = new QValueAxis();
    axisX->setTitleText("Eje X");
    axisX->setTickType(QValueAxis::TicksDynamic);
    axisX->setTickAnchor(0.0); 

    axisY = new QValueAxis();
    axisY->setTitleText("Eje Y");
    axisY->setTickType(QValueAxis::TicksDynamic);
    axisY->setTickAnchor(0.0); 

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    mainLayout->addLayout(sidebar, 1);
    mainLayout->addWidget(chartView, 4);

    // ==========================================
    // SEÑALES Y SLOTS (CONEXIONES)
    // ==========================================
    auto optimizarSubdivisiones = [](QValueAxis *axis) {
        double max_val = axis->max();
        double min_val = axis->min();
        if (!std::isfinite(max_val) || !std::isfinite(min_val)) return;
        
        double amplitud = max_val - min_val;
        if (amplitud <= 1e-12) return; 
        
        double intervaloCrudo = amplitud / 10.0;
        if (intervaloCrudo <= 0) return;
        
        double logBase10 = std::log10(intervaloCrudo);
        if (!std::isfinite(logBase10)) return;
        
        double potenciaDe10 = std::pow(10.0, std::floor(logBase10));
        double proporcion = intervaloCrudo / potenciaDe10;
        
        double pasoLimpio;
        if (proporcion < 1.5)       pasoLimpio = 1.0 * potenciaDe10;
        else if (proporcion < 3.5)  pasoLimpio = 2.0 * potenciaDe10;
        else if (proporcion < 7.5)  pasoLimpio = 5.0 * potenciaDe10;
        else                        pasoLimpio = 10.0 * potenciaDe10;
        
        if (pasoLimpio > 0 && std::isfinite(pasoLimpio)) {
            axis->setTickInterval(pasoLimpio);
        }
    };

    connect(axisX, &QValueAxis::rangeChanged, [this, optimizarSubdivisiones](qreal min, qreal max) {
        optimizarSubdivisiones(this->axisX);
    });
    connect(axisY, &QValueAxis::rangeChanged, [this, optimizarSubdivisiones](qreal min, qreal max) {
        optimizarSubdivisiones(this->axisY);
    });

    connect(btnAgregar, &QPushButton::clicked, [this]() {
        QString formula = editFuncion->text().trimmed();
        if(!formula.isEmpty()) {
            std::string ecuacion = formula.toStdString();
            double x_variable;
            exprtk::symbol_table<double> symbol_table;
            symbol_table.add_variable("x", x_variable);
            symbol_table.add_constants();

            exprtk::expression<double> expression;
            expression.register_symbol_table(symbol_table);
            exprtk::parser<double> parser;
            
            if (!parser.compile(ecuacion, expression)) {
                QMessageBox::warning(this, "Error de Sintaxis", 
                    "La función tiene un error de tipeo.\nRevisa la sintaxis matemática.");
                editFuncion->selectAll(); 
            } else {
                listaFunciones->addItem(formula);
                editFuncion->clear();
            }
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
        }
    });

    connect(btnLimpiar, &QPushButton::clicked, [this]() {
        btnLimpiar->setEnabled(false);
        listaFunciones->clear();
        chart->removeAllSeries();
        axisX->setRange(-10, 10);
        axisY->setRange(-10, 10);
        btnLimpiar->setEnabled(true); 
    });

    connect(btnCentrar, &QPushButton::clicked, [this]() {
        double anchoActual = axisX->max() - axisX->min();
        double altoActual = axisY->max() - axisY->min();
        axisX->setRange(-anchoActual / 2.0, anchoActual / 2.0);
        axisY->setRange(-altoActual / 2.0, altoActual / 2.0);
    });

    connect(btnGraficar, &QPushButton::clicked, [this]() {
        btnGraficar->setEnabled(false); 
        this->procesarYRenderizar();
        btnGraficar->setEnabled(true);
    });

    connect(btnRaices, &QPushButton::clicked, [this]() {
        btnRaices->setEnabled(false); 
        this->calcularYMarcarRaices();
        btnRaices->setEnabled(true);  
    });
    
    // Tema por defecto 
    ThemeManager::aplicarTemaGrafico(ThemeManager::Claro, this->chart, this->axisX, this->axisY);
}

void WindowGraficadora::procesarYRenderizar() {
    chart->removeAllSeries();

    int totalFunciones = listaFunciones->count();
    if (totalFunciones == 0) return;

    double minX = spinMinX->value();
    double maxX = spinMaxX->value();

    if (minX >= maxX) {
        QMessageBox::critical(this, "Error", "El límite X mínimo no puede ser mayor al máximo.");
        return;
    }

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
    const double LIMITE_Y_SEGURO = 10000.0; 

    const QList<QColor> paletaColores = {
        QColor("#3498db"), QColor("#2ecc71"), QColor("#9b59b6"),
        QColor("#e67e22"), QColor("#1abc9c"), QColor("#f1c40f"), QColor("#e74c3c")
    };

    for (int k = 0; k < totalFunciones; ++k) {
        QString qstrFormula = listaFunciones->item(k)->text();
        std::string ecuacion = qstrFormula.toStdString();

        QColor colorActual = paletaColores[k % paletaColores.size()];
        listaFunciones->item(k)->setForeground(colorActual);

        double x_variable;
        exprtk::symbol_table<double> symbol_table;
        symbol_table.add_variable("x", x_variable);
        symbol_table.add_constants();

        exprtk::expression<double> expression;
        expression.register_symbol_table(symbol_table);
        exprtk::parser<double> parser;
        
        if(!parser.compile(ecuacion, expression)) continue;

        double delta = (maxX - minX) / 3000.0; 
        if (delta <= 0) delta = 0.01;

        QLineSeries *serieActual = nullptr;
        QList<QPointF> puntosSegmento; 
        bool enSegmentoValido = false;
        bool primerSegmentoDeLaFuncion = true; 
        int segmentosCreados = 0; 

        for (x_variable = minX; x_variable <= maxX + (delta / 2.0); x_variable += delta) {
            double y_calculado = expression.value();
            
            if (!std::isfinite(y_calculado) || std::abs(y_calculado) > LIMITE_Y_SEGURO) {
                if (enSegmentoValido && serieActual && !puntosSegmento.isEmpty()) {
                    serieActual->replace(puntosSegmento);
                    puntosSegmento.clear();
                } 
                enSegmentoValido = false;
                continue; 
            }

            if (!enSegmentoValido) {
                if (segmentosCreados > 200) continue; 

                serieActual = new QLineSeries();
                segmentosCreados++;
                
                if (primerSegmentoDeLaFuncion) {
                    serieActual->setName(qstrFormula);
                    primerSegmentoDeLaFuncion = false;
                }
                
                QPen penCurva = serieActual->pen();
                penCurva.setWidth(2);
                penCurva.setColor(colorActual);
                serieActual->setPen(penCurva);
                
                chart->addSeries(serieActual);
                serieActual->attachAxis(axisX);
                serieActual->attachAxis(axisY);
                
                if (serieActual->name().isEmpty() && !chart->legend()->markers(serieActual).isEmpty()) {
                    chart->legend()->markers(serieActual).first()->setVisible(false);
                }
                enSegmentoValido = true;
            }

            puntosSegmento.append(QPointF(x_variable, y_calculado));

            if (x_variable <= maxX && std::abs(y_calculado) < (LIMITE_Y_SEGURO * 0.5)) {
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
        if (enSegmentoValido && serieActual && !puntosSegmento.isEmpty()) {
            serieActual->replace(puntosSegmento);
        }
    }

    axisX->setRange(minX, maxX);
    double holguraY = (globalMaxY - globalMinY) * 0.1;
    if (holguraY == 0.0) holguraY = 5.0; 
    
    if (std::abs(globalMaxY - globalMinY) > 5000) {
        axisY->setRange(-50, 50); 
    } else {
        axisY->setRange(globalMinY - holguraY, globalMaxY + holguraY);
    }
}

void WindowGraficadora::calcularYMarcarRaices() {
    if (listaFunciones->count() == 0) return;

    this->procesarYRenderizar();

    QScatterSeries *serieRaices = new QScatterSeries();
    serieRaices->setName("Raíces Exactas");
    serieRaices->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    serieRaices->setMarkerSize(12.0); 
    serieRaices->setColor(QColor("#e74c3c"));
    serieRaices->setBorderColor(QColor("#c0392b"));

    double minX = spinMinX->value();
    double maxX = spinMaxX->value();
    double delta = (maxX - minX) / 3000.0;
    if (delta <= 0) delta = 0.01;

    std::vector<double> raicesUnicas; 
    const double TOLERANCIA_DUPLICADOS = (maxX - minX) / 1000.0; 

    for (int k = 0; k < listaFunciones->count(); ++k) {
        std::string ecuacion = listaFunciones->item(k)->text().toStdString();
        double x_variable;
        exprtk::symbol_table<double> symbol_table;
        symbol_table.add_variable("x", x_variable);
        symbol_table.add_constants();

        exprtk::expression<double> expression;
        expression.register_symbol_table(symbol_table);
        exprtk::parser<double> parser;
        
        if(!parser.compile(ecuacion, expression)) continue;

        double prev_y = NAN;
        double prev_x = minX;

        for (double x_val = minX; x_val <= maxX; x_val += delta) {

            QCoreApplication::processEvents(QEventLoop::AllEvents, 2);
            x_variable = x_val;
            double y = expression.value();

            if (!std::isfinite(y)) {
                prev_y = NAN;
                prev_x = x_val;
                continue;
            }

            if (!std::isnan(prev_y)) {
                if ((prev_y <= 0 && y >= 0) || (prev_y >= 0 && y <= 0)) {
                    
                    double a = prev_x;
                    double b = x_val;
                    double fa = prev_y;
                    double fb = y;
                    
                    for (int step = 0; step < 50; step++) {
                        double mid = (a + b) / 2.0;
                        x_variable = mid;
                        double fmid = expression.value();
                        
                        if (!std::isfinite(fmid)) break; 
                        
                        if ((fa > 0 && fmid < 0) || (fa < 0 && fmid > 0)) {
                            b = mid;
                            fb = fmid;
                        } else {
                            a = mid;
                            fa = fmid;
                        }
                    }
                    
                    double root_x = (a + b) / 2.0;
                    x_variable = root_x;
                    double comprobacion = expression.value();

                    if (std::isfinite(comprobacion) && std::abs(comprobacion) < 0.1) {
                        bool esDuplicada = false;
                        for (double raizGuardada : raicesUnicas) {
                            if (std::abs(raizGuardada - root_x) < TOLERANCIA_DUPLICADOS) {
                                esDuplicada = true;
                                break; 
                            }
                        }
                        
                        if (!esDuplicada) {
                            raicesUnicas.push_back(root_x);
                            serieRaices->append(root_x, 0.0);
                            
                            if (raicesUnicas.size() > 200) break;
                        }
                    }
                }
            }
            prev_y = y;
            prev_x = x_val;
        }
    }

    if (!raicesUnicas.empty()) {
        chart->addSeries(serieRaices);
        serieRaices->attachAxis(axisX);
        serieRaices->attachAxis(axisY);
        QMessageBox::information(this, "Análisis Numérico Completado", 
            QString("Se han detectado %1 raíces únicas (ceros reales) en el rango de visualización.").arg(raicesUnicas.size()));
    } else {
        delete serieRaices;
        QMessageBox::information(this, "Análisis Numérico Completado", 
            "No existen intersecciones reales con el eje X en el rango actual.");
    }
}
