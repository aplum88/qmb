#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H

#include <QDialog>
#include <QMouseEvent>
#include "wke.h"
namespace Ui {
class DialogLogin;
}

class DialogLogin : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLogin(QWidget *parent = 0);
    ~DialogLogin();
    void onMouseMove(int x, int y);
    void onMouseDown(int x, int y);
    void onMouseUp(int x, int y);

    QPoint GetScreenPoint(int x, int y);
    QPoint GetMyCursorPoint();
private:
    bool		mMoveing;

    QPoint	    mMovePosition;

    wkeWebView webView;
    Ui::DialogLogin *ui;

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
};

#endif // DIALOGLOGIN_H
