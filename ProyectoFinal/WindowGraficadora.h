#ifndef WINDOWGRAFICADORA_H
#define WINDOWGRAFICADORA_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QChart>
#include <QValueAxis>
#include "CustomChartView.h"

class WindowGraficadora : public QWidget {
    Q_OBJECT
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
    QPushButton     *btnRaices; 
    
    QLabel          *lblCoordenadas;          

    QChart          *chart;
    CustomChartView *chartView;
    QValueAxis      *axisX;
    QValueAxis      *axisY;

    // Métodos privados para la lógica
    void procesarYRenderizar();
    void calcularYMarcarRaices();

public:
    explicit WindowGraficadora(QWidget *parent = nullptr);
};

#endif // WINDOWGRAFICADORA_H
