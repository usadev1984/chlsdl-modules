#ifndef NOTIFY_H_
#define NOTIFY_H_

#ifdef USE_LIBNOTIFY /* ... */

typedef void * chlsdl_notification;

/* https://specifications.freedesktop.org/notification/latest/categories.html */
enum chlsdl_notify_category {
    chlsdl_notify_category_none,
    chlsdl_notify_category_im,
};

enum chlsdl_notify_urgency {
    chlsdl_notify_urgency_normal,
    chlsdl_notify_urgency_low,
    chlsdl_notify_urgency_critical,
};

struct __chlsdl_notify_notification_create_args {
    const char *                summary;
    const char *                body;
    const char *                icon;
    enum chlsdl_notify_category category;
    enum chlsdl_notify_urgency  urgency;
    int                         timeout;
};

struct __chlsdl_notify_notification_set_args {
    chlsdl_notification noti;
    const char *        summary;
    const char *        body;
    const char *        icon;
};

/* there should be no problem for the args of both functions to be the same for
 * now */
typedef struct __chlsdl_notify_notification_create_args
    __chlsdl_notify_notification_show_new_args;

#    define chlsdl_notify_notification_create(...)                             \
        __chlsdl_notify_notification_create(                                   \
            &(const struct __chlsdl_notify_notification_create_args) {         \
                __VA_ARGS__ })

#    define chlsdl_notify_notification_set(...)                                \
        __chlsdl_notify_notification_set(                                      \
            &(const struct __chlsdl_notify_notification_set_args) {            \
                __VA_ARGS__ })

#    define chlsdl_notify_notification_show_new(...)                           \
        __chlsdl_notify_notification_show_new(&(                               \
            const __chlsdl_notify_notification_show_new_args) { __VA_ARGS__ })

extern bool
chlsdl_notify_init(const char * app_name);

extern void
chlsdl_notify_uninit();

extern void
chlsdl_notify_notification_destroy(chlsdl_notification noti);

extern bool
chlsdl_notify_notification_show(const chlsdl_notification noti);

extern chlsdl_notification
__chlsdl_notify_notification_create(
    const struct __chlsdl_notify_notification_create_args * args);

extern bool
__chlsdl_notify_notification_set(
    const struct __chlsdl_notify_notification_set_args * args);

extern bool
__chlsdl_notify_notification_show_new(
    const __chlsdl_notify_notification_show_new_args * args);

#endif

#endif // NOTIFY_H_
