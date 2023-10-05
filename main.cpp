#include "datmessage.h"
#include "supmessage.h"
#include "mainwindow.h"

#include <QApplication>
#include <iostream>

bool isNonUsbFileFromArgv(const int argc, char **argv) {
    return 2 <= argc && strcmp("-usb", argv[1]) != 0 && strcmp("-usb_dump", argv[1]) != 0;
}

char getFormatFromArgv(const int argc, char **argv) {
    char format = ' ';
    if (3 == argc && strcmp("default", argv[2]) == 0) {
        format = 'd';
    } else if (4 == argc && strcmp("csv", argv[2]) == 0 && 1 == strlen(argv[3])) {
        const char slot = argv[3][0];
        switch (slot) {
            case '1' ... '8':
            case 'A' ... 'B': { format = slot; }
        }
    }
    return format;
}

bool validInputFromArgv(const int argc, char** argv, const bool is_non_usb_file, const char format) {
    if ((3 <= argc || is_non_usb_file) && format == ' ') {
        const char empty_format_literal[1] = "";
        const char* format_arg = empty_format_literal;
        if (3 <= argc) {
            format_arg = argv[2];
        }
        std::cerr << "Unknown format '" << format_arg << "'. Should be either 'default' or 'csv' <slot> (slot: 1-8, A or B)'." << std::endl;
        return false;
    }
    return true;
}

bool isDumpOutput(const int argc, char **argv) {
    return 2 <= argc && strcmp("-usb_dump", argv[1]) == 0;
}

int main(int argc, char **argv) {
    const bool is_non_usb_file = isNonUsbFileFromArgv(argc, argv);
    const char format = getFormatFromArgv(argc, argv);
    if (validInputFromArgv(argc, argv, is_non_usb_file, format)) {
        if (is_non_usb_file) {
            UsbWorker usb_worker(argv[1], false, format);
            usb_worker.readFile();
        } else {
            const bool dump_output = isDumpOutput(argc, argv);
            QApplication app(argc, argv);
            qRegisterMetaType<DatMessage>();
            qRegisterMetaType<SupMessage>();
            MainWindow window = MainWindow(dump_output, format);
            window.show();
            return app.exec();
        }
    }
    return 0;
}
