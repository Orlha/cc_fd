#include <dlfcn.h>
#include <cstddef>
#include <iostream>
#include <cstdlib>

extern "C" typedef void (*hello_t)( int value );

using namespace std;

int main() {
    void* lib = dlopen("./libDic.so", RTLD_NOW);

    if(lib == NULL){
        cerr << dlerror() << endl;
        exit(-1);
    }
    
    hello_t hello = (hello_t)dlsym( lib, "setX" );

    //typedef void (*hello_t)();

    hello(5);
/*
    dlclose(lib);*/
}