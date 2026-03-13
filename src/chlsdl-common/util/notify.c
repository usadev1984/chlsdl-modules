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

#endif
