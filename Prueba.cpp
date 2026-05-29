#include <iostream>
#include <string>
#include <QApplication>
#include <QChart>	//Para crear el gráfico
#include <QChartView>	//Para ver el gráfico
#include <QMouseEvent>	//Para permitir moverse en el gráfico en el mouse
#include <QWheelEvent>	//Lo de arriba pero para la rueda
#include <QToolTip>	//Para permitir ver valores en el gráfico
#include <QPen>		//Para controlar color y grosor de lineas
#include <QLineSeries>
#include <QSplineSeries>
#include <QValueAxis>
#include <QString>
#include <QPainter>
#include <vector>
#include <cmath>
#include "exprtk.hpp"

using namespace QtCharts;

class Extremos {

	public:
		static double minY;
		static double maxY;
		static double minX;
		static double maxX;
		void print() const{
			std::cout << "WithStatic::(minX, maxX, minY, maxY) = "<< "(" << minX << ", " << maxX << ", " << minY << ", " << maxY << std::endl;
		}
};

double Extremos::minY = 0;
double Extremos::maxY = 0;
double Extremos::minX = 0;
double Extremos::maxX = 0;


QLineSeries* EjeX() {
    QLineSeries *ejeX = new QLineSeries();
    ejeX->setName("Eje X");

    ejeX->append(Extremos::minX, 0);
    ejeX->append(Extremos::maxX, 0);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    ejeX->setPen(pen);

    return ejeX;
}

QLineSeries* EjeY() {
    QLineSeries *ejeY = new QLineSeries();
    ejeY->setName("Eje Y");

    ejeY->append(0, Extremos::minY - 1);
    ejeY->append(0, Extremos::maxY + 1);

    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    ejeY->setPen(pen);

    return ejeY;
}

QLineSeries* FLineal(double m, double b){
	QLineSeries *lineal = new QLineSeries();
	lineal -> setName(QString::number(m) + "x + " + QString::number(b));
	

	for(int i = 0; i <= 1; i++) {
		double x = Extremos::minX + i*(Extremos::maxX - Extremos::minX);
		double y = m*x + b;
		lineal -> append(x,y);
		if (y < Extremos::minY) Extremos::minY = y;
		if (y > Extremos::maxY) Extremos::maxY = y;
	}	

	return lineal;

}

QLineSeries* FCualquiera(std::string entrada){
	QLineSeries *funcion = new QLineSeries();
	funcion -> setName(QString::fromStdString(entrada));
	double i;
	exprtk::symbol_table<double> symbol_table;
    	symbol_table.add_variable("x", i);		//"Conecta" el string "x" con la variable i
    	symbol_table.add_constants();
    	
    	exprtk::expression<double> expression;
    	expression.register_symbol_table(symbol_table);
    	
    	exprtk::parser<double> parser;	//Compila el string
    	
    	if (!parser.compile(entrada, expression)) {
        	std::cerr << "Error al compilar la funcion: " << parser.error() << std::endl;
        	funcion->setName("Funcion invalida");
        	return funcion;
    	}
    	
	for(i = Extremos::minX; i <= Extremos::maxX + 1; i += 0.01){
		double y = expression.value();
		if (!std::isfinite(y)) {
            		continue;
        	}
		funcion->append(i, y);
		if (y < Extremos::minY && i <= Extremos::maxX) Extremos::minY = y;
        	if (y > Extremos::maxY && i <= Extremos::maxX) Extremos::maxY = y;
	}
	return funcion;
}
//std::getline(std::cin >> std::ws, n);
int main(int argc, char *argv[]) {

	std::cout << "Ingrese su punto mínimo X para graficar: ";
	std::cin >> Extremos::minX;
	std::cout << "Ingrese su punto máximo X para graficar: ";
	std::cin >> Extremos::maxX;
	int numero;
	std::cout << "Número de funciones a ingresar: ";
	std::cin >> numero;
	std::vector<QLineSeries*> L = {};
	double pendiente;
	double suma;
	for(int i = 0; i < numero; i++){
		std::cout << "Ingresa tu función número " << i+1 << ":";
		
		std::string entry = "";
		std::getline(std::cin >> std::ws, entry);
		
		QLineSeries *serie = FCualquiera(entry);
	
		L.push_back(serie);
	}
	QApplication app(argc, argv);
	//Crear Gráfico
	QChart *chart = new QChart();
	QLineSeries *ejeX = EjeX();
	QLineSeries *ejeY = EjeY();


	for(int i = 0; i < L.size(); i++) chart -> addSeries(L[i]);
	chart -> addSeries(ejeX);
	chart -> addSeries(ejeY);
	chart -> setTitle("Funciónes Lineales");

	//Crear Ejes
	QValueAxis *axisX = new QValueAxis();
	axisX -> setTitleText("X");
	axisX -> setRange((Extremos::minX), (Extremos::maxX));

	QValueAxis *axisY = new QValueAxis();
	axisY -> setTitleText("Y");
	axisY -> setRange((Extremos::minY - 1), (Extremos::maxY + 1));

	chart -> addAxis(axisX, Qt::AlignBottom);
	chart -> addAxis(axisY, Qt::AlignLeft);

	for (int i = 0; i < L.size(); i++) {
		L[i] -> attachAxis(axisX);
		L[i] -> attachAxis(axisY);
	}

	ejeX->attachAxis(axisX);
	ejeX->attachAxis(axisY);

	ejeY->attachAxis(axisX);
	ejeY->attachAxis(axisY);

	//Mostrar Gráfico en una Ventana
	QChartView *chartView = new QChartView(chart);
	chartView -> setRenderHint(QPainter::Antialiasing);
	chartView->setRubberBand(QChartView::RectangleRubberBand);

	chartView -> resize(1000, 750);
	chartView -> show();

	return app.exec();

}





















