NAME = prog1

CC = gcc

UNAME := $(shell uname)

CFLAGS = -Wall

ifeq ($(UNAME), Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL
endif
ifeq ($(UNAME), Linux)
  GLFLAGS = -lglut -lGL
endif

$(NAME): $(NAME).c dataset.o
	gcc $(CFLAGS) $(GLFLAGS) -o $(NAME) $(NAME).c dataset.o

clean:
	rm -rf $(NAME) *.o
