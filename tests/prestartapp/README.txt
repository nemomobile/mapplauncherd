To start the application to prestarted state:

dbus-send --dest=com.nokia.prestartapp --type="method_call" /org/maemo/m com.nokia.MApplicationIf.ping

To launch prestarted application:

dbus-send --dest=com.nokia.prestartapp --type="method_call" /org/maemo/m com.nokia.MApplicationIf.launch

