#include "USBUnitTest.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <iostream>

int main(int argc, char *argv[])
{
    USBUnitTest test(nullptr);

    argc--;
    argv++;

    if (argc != 1) {
        std::cerr << "usbunittest: expected a single argument, --server or --client" << std::endl;
        return -1;
    }

    QString arg = argv[0];
    if (arg == "--server")
        test.setServer();
    else if (arg == "--client")
        test.setClient();
    else {
        std::cerr << "usbunittest: expected a single argument, --server or --client" << std::endl;
        return -1;
    }


    QCoreApplication a(argc, argv);
    QTimer::singleShot(10, &test, &USBUnitTest::runtest);

    return a.exec();
}
