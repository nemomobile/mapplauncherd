execute_process(COMMAND echo "Creating symlinks for helloworld..")
execute_process(COMMAND ln -v -s /usr/bin/helloworld $ENV{DESTDIR}/usr/bin/helloworld_1)
execute_process(COMMAND ln -v -s /usr/bin/helloworld $ENV{DESTDIR}/usr/bin/helloworld_2)
execute_process(COMMAND ln -v -s /usr/bin/helloworld.launch $ENV{DESTDIR}/usr/bin/helloworld_1.launch)
execute_process(COMMAND ln -v -s /usr/bin/helloworld.launch $ENV{DESTDIR}/usr/bin/helloworld_2.launch)

