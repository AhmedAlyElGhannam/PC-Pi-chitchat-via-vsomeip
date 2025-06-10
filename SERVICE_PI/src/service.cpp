#include "service.hpp"

int main(void) 
{
    service_app _service;

    _service.init();
    _service.start();

    return 0;
}
