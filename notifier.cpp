#include "notifier.h"

void
notify(const std::string& message, const std::string header)
{
        notify_init(header.c_str());
        NotifyNotification* n = notify_notification_new(header.c_str(), message.c_str(), NULL);
        notify_notification_set_urgency(n, NOTIFY_URGENCY_CRITICAL); /* Quit it when user clicks it. */

        if (!notify_notification_show(n, NULL))
                throw std::runtime_error("An error occurred while showing notification.");

        notify_uninit();
}
