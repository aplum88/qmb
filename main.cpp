#include <QApplication>
#include <wke.h>
#include "DialogLogin.h"
#include "DialogWke.h"
int main(int argc, char *argv[])
{
    //初始化wke
    //QString dll = "G:/qt_wke/lib/node.dll";


    //wkeSetWkeDllPath(reinterpret_cast<const wchar_t *>(dll.utf16()));
    wkeInitialize();
    QApplication a(argc, argv);
    DialogWke w;
    //DialogLogin w;
    w.show();
    w.LoadUrl("http://www.baidu.com");
    return a.exec();
}
