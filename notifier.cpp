#include "notifier.h"

void
notify(const std::string& message, const std::string header, const std::string icon)
{
        notify_init(header.c_str());

        NotifyNotification* notification = notify_notification_new(header.c_str(), message.c_str(), icon.c_str());
        notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL); /* Quit it when user clicks it. */

        GError *error = nullptr;
        if (!notify_notification_show(notification, &error)) {
                std::string error_message = "An error occurred while showing notification.\n";
                error_message += std::string(error->message);
                throw std::runtime_error(error_message);
        }

        notify_uninit();
}
