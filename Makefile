TARGET=devmem

$(TARGET) : devmem.o
	$(CC) -o $@ $^

.PHONY:
all : $(TARGET)

.PHONY:
deploy : all
	scp $(TARGET) root@$(BOARD_IP):/home/root/

.PHONY:
clean : 
	rm -f *.o $(TARGET)
