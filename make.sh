#gcc -O3 x11fb.c -o x11fb -lX11 -lxcb -lXdmcp -lXau -pthread -static
gcc -fPIC -shared -O3 x11fb.c -o libx11fb.so -lX11 -lxcb -lXdmcp -lXau -DLIBRARY

#echo "-----"
#gcc x11fb.c -o x11fb -Wl,-Bstatic -lX11 -lxcb -lXdmcp -lXau -pthread -Wl,-Bdynamic 
