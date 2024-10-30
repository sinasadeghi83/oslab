obj-m+=task.o
KV:=6.11.3-200.fc40.x86_64
all:
	make -C /lib/modules/$(KV)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KV)/build M=$(PWD) clean