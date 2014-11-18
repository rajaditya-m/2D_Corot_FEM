#ifndef TUBICSPLINES_H
#define TUBICSPLINES_H

#include <QtWidgets/QMainWindow>
#include "ui_TubicSplines.h"
#include "ui_custom_mods.h"
#include "glwidget.h"

class Tubic_Splines : public QMainWindow
{
	Q_OBJECT

public:
	Tubic_Splines(QWidget *parent = 0);
	~Tubic_Splines();

private:
	Ui::TubicSplineClass ui;
	Ui::ui_custom_mods ui_custom;

};

#endif // TUBICSPLINES_H
