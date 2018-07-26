#ifndef DIALOGWKE_H
#define DIALOGWKE_H

#include <QDialog>
#include "QMiniBlink.h"
namespace Ui {
class DialogWke;
}

class DialogWke : public QMiniBlink
{
    Q_OBJECT

public:
    explicit DialogWke(QWidget *parent = 0);
    ~DialogWke();

private:
    Ui::DialogWke *ui;
    // QWidget interface

};

#endif // DIALOGWKE_H
