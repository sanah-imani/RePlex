#include "plugin.hpp"

#include <iostream>

void init()
{
    std::cout << "Plugin init\n";
}

void update(float dt)
{
    std::cout << "Plugin running, dt = " << dt << "\n";
}

void shutdown()
{
    std::cout << "Plugin shutdown\n";
}
