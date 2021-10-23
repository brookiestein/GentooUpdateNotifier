#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <cstddef>
#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <stdexcept>
#include <string>

void notify(const std::string& message, const std::string header = "Gentoo Update Notifier");

#endif
