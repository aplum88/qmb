#include "DialogLogin.h"
#include "ui_DialogLogin.h"
#include <QFile>
#include <QDebug>

#include <QSerialPortInfo>
DialogLogin* login = NULL;
//js调用c++接口
jsValue JS_CALL jsInvokeClose(jsExecState es)
{
    //QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),jsToString(es, jsArg(es, 0)));
    login->close();
    return jsUndefined();
}
jsValue JS_CALL JsonMouseMove(jsExecState es)
{
    //QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),jsToString(es, jsArg(es, 0)));
    int x = jsArg(es,0);
    int y = jsArg(es,1);
    login->onMouseMove(x,y);
    return jsUndefined();
}
jsValue JS_CALL JsonMouseDown(jsExecState es)
{
    //QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),jsToString(es, jsArg(es, 0)));

    int x = jsArg(es,0);
    int y = jsArg(es,1);
    login->onMouseDown(x,y);
    return jsUndefined();
}
jsValue JS_CALL JsonMouseUp(jsExecState es)
{
    //QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),jsToString(es, jsArg(es, 0)));
    int x = jsArg(es,0);
    int y = jsArg(es,1);
    login->onMouseUp(x,y);
    return jsUndefined();
}

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
jsValue JS_CALL JsCallListPorts(jsExecState es)
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    QStringList str;
    QJsonDocument doc;
    QJsonArray arr;

    for(int i = 0; i < ports.size(); i++)
    {
        QJsonObject o;
        o["value"] = ports[i].portName();
        o["lable"] = ports[i].portName();
        arr.push_back(o);
    }
    doc.setArray(arr);

    QString json = doc.toJson();
    qDebug() << "json = " << json;
    return jsString(es,json.toStdString().c_str());


}
wkeWebView onCreateView(wkeWebView webView, void* param, wkeNavigationType navType, const wkeString urlStr, const wkeWindowFeatures* features)
{
    const wchar_t* url = wkeGetStringW(urlStr);

    wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
    wkeShowWindow(newWindow, true);
    return newWindow;
}

DialogLogin::DialogLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLogin)
{
    ui->setupUi(this);

    mMoveing=false;
   // setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint |Qt::WindowStaysOnTopHint);
        this->setStyleSheet("QDialog{border:1px solid black;}");


    login = this;

    //关于wke函数详见 wkedefine.h
    //创建wkeWebView实例并将其绑定到qt界面中的widget控件
    //qDebug() << ui->widget->width() << " h" << ui->widget->height();
//    webView = wkeCreateWebView();
//    wkeSetTransparent(webView, false);
//    wkeSetHandle(webView, (HWND)this->winId());
//    wkeResize(webView, 800, 600);


    webView= wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)this->winId(), 0, 30, 800,600);
    //显示wkeWebView
//    wkeOnCreateView(webView, onCreateView, NULL);
//    wkeMoveToCenter(webView);
    wkeShowWindow(webView, TRUE);
    QFile file("debug/demo.html");
    file.open(QIODevice::ReadOnly);
    //wkeLoadHtmlWithBaseUrl(webView,file.read(file.bytesAvailable()),"res");
    wkeLoadHTML(webView,file.read(file.bytesAvailable()));
    //wkeLoadURL(webView,"http://www.baidu.com");
    file.close();
    jsBindFunction("invokeClose",jsInvokeClose,1);
    jsBindFunction("invokeMouseUp",  JsonMouseUp,1);
    jsBindFunction("invokeMouseDown",JsonMouseDown,1);
    jsBindFunction("invokeMouseMove",JsonMouseMove,1);
    jsBindFunction("invokeListUarts",JsCallListPorts,1);

}

DialogLogin::~DialogLogin()
{
    delete ui;
}


QPoint DialogLogin::GetMyCursorPoint()
{
     POINT pt;
     GetCursorPos(&pt);
     return QPoint(pt.x, pt.y);
}
QPoint DialogLogin::GetScreenPoint(int x, int y)
{
    POINT pt;
    pt.x = x;
    pt.y = y;

    ClientToScreen((HWND)this->winId(),&pt);

    return QPoint(pt.x, pt.y);
}
void DialogLogin::onMouseDown(int x, int y)
{
    QPoint cp = GetMyCursorPoint();
    qDebug() <<"mousePressEvent x" << cp.x() << "y=" << cp.y();
    mMoveing = true;
    //QPoint p = GetScreenPoint(x,y) ;
    mMovePosition = cp - pos();

}

void DialogLogin::onMouseUp(int x, int y)
{
     qDebug() <<"mouseReleaseEvent";
    mMoveing = false;
}


void DialogLogin::mousePressEvent(QMouseEvent *event)
{


    mMoveing = true;
    //记录下鼠标相对于窗口的位置
    //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
    //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
    mMovePosition = event->globalPos() - pos();
    return QDialog::mousePressEvent(event);
}

void DialogLogin::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() <<"mouseReleaseEvent";
    mMoveing = false;
}
void DialogLogin::onMouseMove(int x, int y)
{
    //qDebug() <<"mouseMoveEvent y=" << y;



    if (mMoveing&& (GetMyCursorPoint()-mMovePosition).manhattanLength() > QApplication::startDragDistance())
    {
        move(GetMyCursorPoint()-mMovePosition);
        mMovePosition = GetMyCursorPoint() - pos();
    }

}
void DialogLogin::mouseMoveEvent(QMouseEvent *event)
{

    //(event->buttons() && Qt::LeftButton)按下是左键
    //鼠标移动事件需要移动窗口，窗口移动到哪里呢？就是要获取鼠标移动中，窗口在整个屏幕的坐标，然后move到这个坐标，怎么获取坐标？
    //通过事件event->globalPos()知道鼠标坐标，鼠标坐标减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标

    if (mMoveing && (event->buttons() && Qt::LeftButton)
        && (event->globalPos()-mMovePosition).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    return QDialog::mouseMoveEvent(event);
}
