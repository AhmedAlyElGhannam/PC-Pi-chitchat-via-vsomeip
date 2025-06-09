#pragma once

#include <memory>
#include <vsomeip.hpp>
#include <cstdio>
#include <iostream>

#define LOG_INF(msg) std::cout << "[INFO] " << msg << std::endl

constexpr vsomeip::service_t server_id = 0x0100;
constexpr vsomeip::instance_t instance_id = 0x8888;
constexpr vsomeip::method_t method_id = 0x4022;

void on_message_handler(const std::shared_ptr<vsomeip::message> &_request) 
{
    LOG_INF("Received request! Privet!\n");
    auto app = vsomeip::runtime::get()->get_application("server");
    auto response = vsomeip::runtime::get()->create_response(_request);
    response->set_payload(_request->get_payload());
    app->send(response);
}

void on_message_available(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _isAvail)
{
    LOG_INF("Service availability changed. Hi!\n");
}

class server_app
{
    private:
        std::shared_ptr<vsomeip::application> _app_ptr;

    public:
        bool init(void)
        {
            _app_ptr = vsomeip::runtime::get()->create_application("server");
            
            _app_ptr->register_availability_handler(
                server_id,
                instance_id,
                on_message_available
            );

            _app_ptr->register_message_handler(
                server_id,
                instance_id,
                method_id,
                on_message_handler
            );

            _app_ptr->init();
            _app_ptr->offer_service(server_id, instance_id); // offer the service

            return true;
        }

        void start(void)
        {
            _app_ptr->start();
        }

        void stop(void) 
        {
            _app_ptr->unregister_availability_handler(server_id, instance_id);
            _app_ptr->unregister_message_handler(vsomeip::ANY_SERVICE, instance_id, method_id);
            _app_ptr->stop();
        }
};