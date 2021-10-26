#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <stdexcept>
#include <string>

void notify(const std::string& message,
        const std::string header = "Gentoo Update Notifier",
        const std::string icon = "dialog-information");

#endif
