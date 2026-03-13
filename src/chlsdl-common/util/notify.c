#ifdef USE_LIBNOTIFY /* ... */

#    include <assert.h>
#    include <chlsdl-modules/chlsdl-common/print.h>
#    include <chlsdl-modules/chlsdl-common/util/notify.h>
#    include <libnotify/notify.h>

bool
chlsdl_notify_init(const char * app_name)
{
#    ifdef DEBUG
    char * server_name;
    char * server_vendor;
    char * server_version;
    char * server_spec_version;

    notify_get_server_info(
        &server_name, &server_vendor, &server_version, &server_spec_version);

    print_debug_warn("libnotify server %s %s, v%s - Notification Spec %s\n",
        server_name, server_vendor, server_version, server_spec_version);
    g_free(server_name);
    g_free(server_vendor);
    g_free(server_version);
    g_free(server_spec_version);
#    endif

    return notify_init(app_name);
}

void
chlsdl_notify_uninit()
{
    return notify_uninit();
}

chlsdl_notification
__chlsdl_notify_notification_create(
    const struct __chlsdl_notify_notification_create_args * args)
{
    static gint notification_id;

    chlsdl_notification r = g_object_new(NOTIFY_TYPE_NOTIFICATION, "summary",
        !args->summary ? "" : args->summary, "body", args->body, "app-icon",
        NULL, "icon-name", NULL, "id", notification_id++, NULL);

    if (!r)
        return NULL;

    /* switch (args->category) { */
    /* case chlsdl_notify_category_im: */
    /*     notify_notification_set_category(r, "im"); */
    /* default: */
    /* } */

    if (args->category != chlsdl_notify_category_none) {
        print_error("categories are not supported\n");
        assert(0);
    }

    if (args->urgency != chlsdl_notify_urgency_normal)
        notify_notification_set_urgency(r, args->urgency);

    if (args->timeout > 0)
        notify_notification_set_timeout(r, args->timeout);
    return r;
}

void
chlsdl_notify_notification_destroy(chlsdl_notification noti)
{
    g_object_unref(G_OBJECT(noti));
}

bool
__chlsdl_notify_notification_set(
    const struct __chlsdl_notify_notification_set_args * args)
{
    /**
     * TODO: unfuck this. this needs to be able to change the rest of the
     * attributes supported by `__chlsdl_notify_notification_create()`. also, is
     * it possible to not overwrite everything on every call?
     */
    return notify_notification_update(
        args->noti, !args->summary ? "" : args->summary, args->body, NULL);
}

bool
chlsdl_notify_notification_show(const chlsdl_notification noti)
{
    return notify_notification_show(noti, NULL);
}

#endif
