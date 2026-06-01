#ifndef SHARED_H_
#define SHARED_H_

#ifdef USE_LIBNOTIFY
#    ifndef __MOD_NOTIFY_MOD_NAME
#        error __MOD_NOTIFY_MOD_NAME not defined
#    endif

#    define MOD_PRINT_AND_NOTIFY(print, fmt, ...)                              \
        ({                                                                     \
            char * body = svconcat(fmt __VA_OPT__(, ) __VA_ARGS__);            \
            assert(body);                                                      \
            print("%s\n", body);                                               \
            chlsdl_notify_notification_show_new(                               \
                "chlsdl-" __MOD_NOTIFY_MOD_NAME, body,                         \
                .timeout = default_notification_timeout);                      \
            free(body);                                                        \
        })

#    define MOD_ERROR_AND_NOTIFY(print, fmt, ...)                              \
        ({                                                                     \
            char * body = svconcat(fmt __VA_OPT__(, ) __VA_ARGS__);            \
            assert(body);                                                      \
            print("%s\n", body);                                               \
            chlsdl_notify_notification_show_new(                               \
                "chlsdl-" __MOD_NOTIFY_MOD_NAME, body,                         \
                .timeout = default_notification_timeout,                       \
                .urgency = chlsdl_notify_urgency_critical);                    \
            free(body);                                                        \
        })
#else
#    define MOD_PRINT_AND_NOTIFY(print, fmt, ...)                              \
        print(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)

#    define MOD_ERROR_AND_NOTIFY(print, fmt, ...)                              \
        print(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#endif

#endif // SHARED_H_
