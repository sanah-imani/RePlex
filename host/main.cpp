#include <iostream>
#include <thread>
#include <chrono>
#include <dlfcn.h>

using UpdateFunc = void (*)(float);

int main() {
    std::cout << "Replex host starting ... \n";

   // load the shared library 
   
   void* handle = dlopen("./build/libplugin.so", RTLD_LAZY);

   if (!handle){
    std::cerr << "Failed to load plugin: " << dlerror() << "\n";
    return 1;
   }

   // clear previous errors 
   dlerror();

   UpdateFunc update = (UpdateFunc) dlsym(handle, "update");

   char* error = dlerror();
   if (error != nullptr){
    std::cerr << "Failed to load symbol: " << error << "\n";
    return 1;
   }

   std::cout << "Plugin loaded successfully!\n";

   while (true){
    update(0.016f);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   }

   dlclose(handle);
}