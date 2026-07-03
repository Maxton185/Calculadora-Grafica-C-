# Calculadora-Grafica-C++

Este proyecto es una calculadora gráfica desarrollada en C++ utilizando el framework Qt5 (Widgets y Charts) para la interfaz de usuario y la renderización de los gráficos. Utiliza la librería matemática `exprtk` para el análisis y evaluación de las expresiones matemáticas.

## Requisitos Previos

Para poder compilar y ejecutar este proyecto, necesitarás un entorno basado en Linux (como Ubuntu o WSL) con los siguientes compiladores y librerías instalados:

* **Compilador C++**: Se utiliza `clang++`
  
* **Herramientas de construcción:** `make` y `qmake`.
* **Librerías de Qt5:** Módulos Core, Gui, Widgets y Charts.

Puedes instalar todas las dependencias necesarias en Ubuntu/Debian ejecutando el siguiente comando en tu terminal:

```bash
sudo apt update
sudo apt install clang make qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools libqt5charts5-dev
```

## Instalación y Compilación

Sigue estos pasos para clonar el repositorio y compilar el código fuente en tu máquina local.

**1. Clonar el repositorio**
Descarga los archivos del proyecto a tu computadora:
```bash
git clone [https://github.com/Maxton185/Calculadora-Grafica-C-.git](https://github.com/Maxton185/Calculadora-Grafica-C-.git)
```

**2. Entrar a la carpeta del proyecto**
Navega hacia el directorio donde se encuentra el archivo `.pro`:
```bash
cd Calculadora-Grafica-C-/ProyectoFinal
```

**3. Generar el archivo Makefile**
Utiliza `qmake` para leer el archivo de configuración del proyecto y preparar las instrucciones de compilación:
```bash
qmake Prueba.pro
```

**4. Compilar el proyecto**
Ejecuta `make` para construir el ejecutable. Este proceso enlazará tu código con las librerías de Qt:
```bash
make
```

**5. Ejecutar la aplicación**
Una vez finalizada la compilación, inicia el programa con el siguiente comando:
```bash
./Prueba
```

## Limpieza del Proyecto

Si en algún momento necesitas recompilar el código desde cero o limpiar los archivos temporales generados (como los `.o`), ejecuta:
```bash
make clean
```
