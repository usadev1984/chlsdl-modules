#ifndef NOTIFY_H_
#define NOTIFY_H_

#ifdef USE_LIBNOTIFY /* ... */

extern bool
chlsdl_notify_init(const char * app_name);

extern void
chlsdl_notify_uninit();

#endif

#endif // NOTIFY_H_
