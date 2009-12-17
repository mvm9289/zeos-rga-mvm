obj-m += syscallsMonitor.o syscallsConsultant.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install: syscallsMonitor.o syscallsConsultant.o
	insmod syscallsMonitor.ko
	insmod syscallsConsultant.ko
	mknod /dev/consultant c 256 0

remove:
	rm /dev/consultant
	rmmod syscallsConsultant
	rmmod syscallsMonitor

jp: JP/syscallsMonitorJP.c JP/syscallsConsultantJP.c
	gcc -o syscallsMonitorJP JP/syscallsMonitorJP.c
	gcc -o syscallsConsultantJP JP/syscallsConsultantJP.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm *~ include/*~ JP/*~ syscallsMonitorJP syscallsConsultantJP
