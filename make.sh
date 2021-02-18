gcc -O3 x11fb.c -o x11fb -lX11 -lxcb -lXdmcp -lXau -pthread -static


#echo "-----"
#gcc x11fb.c -o x11fb -Wl,-Bstatic -lX11 -lxcb -lXdmcp -lXau -pthread -Wl,-Bdynamic 
