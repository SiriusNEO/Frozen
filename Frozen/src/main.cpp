#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    srand(time(NULL));

    QApplication a(argc, argv);

    Widget w;
    w.setWindowTitle("Frozen!");
    w.show();

    return a.exec();
}
