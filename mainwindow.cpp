#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
bool MainWindow::isPlaning = true;

//js调用c++接口
jsValue JS_CALL jsInvokeC(jsExecState es)
{
    QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),jsToString(es, jsArg(es, 0)));
    return jsUndefined();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    //关于wke函数详见 wkedefine.h
    //创建wkeWebView实例并将其绑定到qt界面中的widget控件
    qDebug() << ui->widget->width() << " h" << ui->widget->height();
    webView= wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)ui->widget->winId(), 0, 0, 800,600);
    //显示wkeWebView
    wkeShowWindow(webView, true);

    wkeSetDebugConfig(webView,"showDevTools","G:/qt_wke/front_end/inspector.html");
    connect(ui->loadUrl,SIGNAL(clicked()),this,SLOT(onLoadurlClicked()));
    connect(ui->openHtml,SIGNAL(clicked()),this,SLOT(onOpenhtmlClicked()));
    //connect(ui->invokeJs,SIGNAL(clicked()),this,SLOT(onInvokejsClicked()));
    //绑定js要调用的c++方法，第一个参数是js中执行的方法，第二个参数是c++中定义的全局方法，第三个参数是方法传递的参数个数
    jsBindFunction("invokeC",jsInvokeC,1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* size){
    Q_UNUSED(size)
    //改变wkeWebView显示大小
   // wkeResize(webView, ui->widget->width(),ui->widget->height());
}

//加载网址
void MainWindow::onLoadurlClicked()
{
    wkeLoadURL(webView,ui->url->text().toLatin1().data());
}

//加载本地html文件，这里取出了html中的文本内容并加载
void MainWindow::onOpenhtmlClicked()
{
    QFile file("debug/login.html");
    file.open(QIODevice::ReadOnly);
    wkeLoadHTML(webView,file.read(file.bytesAvailable()));
    file.close();
}

//c++调用js方法
void MainWindow::onInvokejsClicked()
{
    static int count = 0;
    wkeRunJS(webView,QString("cInvokeJs(%1)").arg(++count).toLatin1().data());
}

void MainWindow::on_loadUrl_clicked()
{
    wkeLoadURL(webView,ui->url->text().toLatin1().data());
}

void MainWindow::on_invokeJs_clicked()
{


}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "hello";
    wkeLoadFile(webView,"fs.js");
//    jsExecState  s = wkeGlobalExec(webView);
//    jsValue func = jsString(s,"download");

//    jsValue args[1] = {jsString(s,"hello")};
//    jsCall(s,func,jsUndefined(),args,1);
}
