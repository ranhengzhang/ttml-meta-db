#ifndef WINTOAST_H
#define WINTOAST_H

#include "wintoastlib.h"

using namespace WinToastLib;

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const override {
        std::wcout << L"The user clicked in this toast" << std::endl;
    }

    void toastActivated(int actionIndex) const override {
        std::wcout << L"The user clicked on button #" << actionIndex << L" in this toast" << std::endl;
    }

    void toastActivated(const char* response) const override {
        std::wcout << L"The user clicked on action #" << response << std::endl;
    }

    void toastFailed() const override {
        std::wcout << L"Error showing current toast" << std::endl;
    }

    void toastDismissed(WinToastDismissalReason state) const override {
        switch (state) {
            case UserCanceled:
                std::wcout << L"The user dismissed this toast" << std::endl;
            break;
            case ApplicationHidden:
                std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
            break;
            case TimedOut:
                std::wcout << L"The toast has timed out" << std::endl;
            break;
            default:
                std::wcout << L"Toast not activated" << std::endl;
            break;
        }
    }
};

#endif // WINTOAST_H
