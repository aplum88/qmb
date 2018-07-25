#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QMessageBox>
#include <wke.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *size);
    static bool isPlaning;

private slots:
    void onLoadurlClicked();
    void onOpenhtmlClicked();
    void onInvokejsClicked();

    void on_loadUrl_clicked();

    void on_invokeJs_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    wkeWebView webView;
};
#endif // MAINWINDOW_H
