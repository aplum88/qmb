#ifndef QMINIBLINK_H
#define QMINIBLINK_H

#include <QDialog>
#include <QAbstractNativeEventFilter>
#include <QKeyEvent>
#include "wke.h"
namespace Ui {
class QMiniBlink;
}

class QMiniBlink : public QDialog, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit QMiniBlink(QWidget *parent = 0);
    ~QMiniBlink();
    void LoadUrl(QString url);
    void LoadFile(QString file);

    // QAbstractNativeEventFilter interface
public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
protected:
    virtual void keyPressEvent(QKeyEvent *);
private:
    wkeWebView webView;

};

#endif // QMINIBLINK_H
