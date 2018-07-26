#include "DialogWke.h"
#include "ui_DialogWke.h"
#include <QDebug>


DialogWke::DialogWke(QWidget *parent) :
    QMiniBlink(parent),
    ui(new Ui::DialogWke)
{
    ui->setupUi(this);


}

DialogWke::~DialogWke()
{
    delete ui;
}

