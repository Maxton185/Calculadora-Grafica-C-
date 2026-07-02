#ifndef CAMBIARTEMA_H
#define CAMBIARTEMA_H

#include <QApplication>
#include <QString>
#include <QChart>
#include <QValueAxis>
#include <QLegend>

using namespace QtCharts;

class ThemeManager {
public:
    enum Tema {
        Claro,
        Oscuro,
        EscalaGrises,
        Solarizado
    };

    // Aplica el estilo a la interfaz
    static void aplicarTema(Tema tema) {
        QString estilo;
        switch (tema) {
            case Claro:
                estilo = ""; 
                break;
            case Oscuro:
                estilo = R"(
                    QWidget { background-color: #2b2b2b; color: #d3d3d3; }
                    QLineEdit, QDoubleSpinBox, QListWidget { background-color: #3c3f41; color: #ffffff; border: 1px solid #555555; }
                    QPushButton { background-color: #4b4d4f; color: #ffffff; border: 1px solid #555555; padding: 5px; }
                    QPushButton:hover { background-color: #5c5f61; }
                )";
                break;
            case EscalaGrises:
                estilo = R"(
                    QWidget { background-color: #e0e0e0; color: #000000; }
                    QLineEdit, QDoubleSpinBox, QListWidget { background-color: #ffffff; color: #000000; border: 2px solid #7a7a7a; }
                    QPushButton { background-color: #c0c0c0; color: #000000; border: 2px solid #555555; }
                    QPushButton:hover { background-color: #a0a0a0; }
                )";
                break;
            case Solarizado:
                estilo = R"(
                    QWidget { background-color: #fdf6e3; color: #657b83; }
                    QLineEdit, QDoubleSpinBox, QListWidget { background-color: #eee8d5; color: #586e75; border: 1px solid #93a1a1; }
                    QPushButton { background-color: #cb4b16; color: #fdf6e3; border: none; padding: 5px; border-radius: 3px; }
                    QPushButton:hover { background-color: #dc322f; }
                )";
                break;
        }
        qApp->setStyleSheet(estilo);
    }

    // Aplica los colores
    static void aplicarTemaGrafico(Tema tema, QChart* chart, QValueAxis* axisX, QValueAxis* axisY) {
        if (!chart || !axisX || !axisY) return; 

        QColor bgColor, textColor, gridColor;

        // Paletas de colores 
        switch (tema) {
            case Claro:
                bgColor = QColor("#ffffff");
                textColor = QColor("#2c3e50"); 
                gridColor = QColor("#e0e0e0"); 
                break;
            case Oscuro:
                bgColor = QColor("#2b2b2b");
                textColor = QColor("#d3d3d3"); 
                gridColor = QColor("#555555"); 
                break;
            case EscalaGrises:
                bgColor = QColor("#ffffff");
                textColor = QColor("#000000");
                gridColor = QColor("#a0a0a0");
                break;
            case Solarizado:
                bgColor = QColor("#fdf6e3"); 
                textColor = QColor("#657b83");
                gridColor = QColor("#93a1a1"); 
                break;
        }

        chart->setBackgroundBrush(QBrush(bgColor));
        chart->setTitleBrush(QBrush(textColor));

        chart->legend()->setLabelColor(textColor);
        
        auto pintarEje = [&](QValueAxis* axis) {
            axis->setLabelsBrush(QBrush(textColor));      
            axis->setTitleBrush(QBrush(textColor));      
            axis->setGridLineColor(gridColor);            
            axis->setLinePenColor(textColor);            
        };

        pintarEje(axisX);
        pintarEje(axisY);
    }
};

#endif 
