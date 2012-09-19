all:
	mkdir -p lib/
	gcc -o lib/port-binder.so -shared src/port-binder.c -ldl
