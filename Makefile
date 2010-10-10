NAME = prog1

CC = gcc

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Cocoa
endif
ifeq ($(UNAME), Linux)
  GLFLAGS = -lGL -lGLU -lglut
endif

$(NAME): $(NAME).c dataset.o
	gcc $(GLFLAGS) -o $(NAME) $(NAME).c dataset.o

clean:
	rm -rf $(NAME) *.o
