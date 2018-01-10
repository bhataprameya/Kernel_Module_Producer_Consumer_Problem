CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_ALL=n
obj-m +=kernel_code.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc consumer.c -o consumer.out
	gcc producer.c -o producer.out
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *.out
