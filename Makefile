# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Исходники C
SRCS_C=main.c grep.c usage.c arguments.c


TARGET=grep
PREFIX=/urs/local/bin/

# Объектные файлы
OBJS=$(SRCS_C:.c=.o)

# Компиляторы
GCC=gcc -Wall


# Цели

.PHONY: all clear #install uninstall

all: $(TARGET)

clear:
	rm -f "$(TARGET)" *.o

#install:
#	install $(TARGET) $(PREFIX)

#uninstall:
#	rm -f $(PREFIX)/$(TARGET)

# Простая версия
simple: simple-main.c io.o copy.o other.o
	$(GCC) -o simple-cp io.o copy.o other.o simple-main.c

# Конечная цель
$(TARGET): $(OBJS)
	$(GCC) -o $@ $^

# Неявные преобразования
%.o: %.c
	$(GCC) -o $@ -c $<
