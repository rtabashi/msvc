NAME = change_priv.exe
SRCS = *.c
OBJS = *.obj
LIBS = Advapi32.lib

all:$(NAME)

$(OBJS):
	cl /c $(SRCS)

$(NAME): $(OBJS)
	link /OUT:$(NAME) $(OBJS) $(LIBS)

clean:
	del $(OBJS)

fclean: clean
	del $(NAME)

re: fclean all
