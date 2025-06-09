#pragma once

#include <memory>
#include <vsomeip.hpp>
#include <cstdio>
#include <iostream>

#define LOG_INF(msg) std::cout << "[INFO] " << msg << std::endl

constexpr vsomeip::service_t server_id = 0x0100;
constexpr vsomeip::instance_t instance_id = 0x8888;
constexpr vsomeip::method_t method_id = 0x4022;

void on_message_handler(const std::shared_ptr<vsomeip::message> &_response) 
{
    LOG_INF("Hola!\n");
}

void on_message_available(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _isAvail) 
{
    if (_service == server_id && _instance == instance_id && _isAvail) 
    {
        LOG_INF("Service available! Bonjour!\n");
        auto app = vsomeip::runtime::get()->get_application("client");
        app->request_service(server_id, instance_id);
        // Send a request to the server
        auto request = vsomeip::runtime::get()->create_request(); 
        request->set_service(server_id);
        request->set_instance(instance_id);
        request->set_method(method_id);
        app->send(request);
    }
}

class client_app
{
    private:
        std::shared_ptr<vsomeip::application> _app_ptr;

    public:
        bool init(void)
        {
            _app_ptr = vsomeip::runtime::get()->create_application("client");

            _app_ptr->register_availability_handler(
                vsomeip::ANY_SERVICE,
                vsomeip::ANY_INSTANCE,
                on_message_available
            );

            _app_ptr->register_message_handler(
                vsomeip::ANY_SERVICE, 
                vsomeip::ANY_INSTANCE, 
                vsomeip::ANY_METHOD,
                on_message_handler
            );

            _app_ptr->init();
            
            return true;
        }

        void start(void)
        {
            _app_ptr->start();
        }

        void stop(void) 
        {
            _app_ptr->unregister_availability_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE);
            _app_ptr->unregister_message_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE, vsomeip::ANY_METHOD);
            _app_ptr->stop();
        }
};