#pragma once

#include <memory>
#include <vsomeip.hpp>
#include <cstdio>
#include <iostream>

#define LOG_INF(msg) std::cout << "[INFO] " << msg << std::endl

constexpr vsomeip::service_t service_id = 0x1234;
constexpr vsomeip::instance_t instance_id = 0x5678;
constexpr vsomeip::method_t method_id = 0x0421;
constexpr vsomeip::event_t event_id = 0x1235;

void onMessageEventHandler(const std::shared_ptr<vsomeip::message> &_response) 
{
    LOG_INF("Hola!\n");
}

void onServiceAvailable(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _isAvail) 
{
    /* only proceed if serivce is available + both service && instance IDs match */
    if (_service == service_id && _instance == instance_id && _isAvail) 
    {
        /* log to know this function got called */
        LOG_INF("Service available! Bonjour!\n");
        
        /* get a reference on the app */
        auto app = vsomeip::runtime::get()->get_application("client");

        /* only request the service once it is available */
        app->request_service(
            service_id, 
            instance_id
        );
        
        /* subscribe to the event with the given ID */
        app->subscribe(
            service_id,
            instance_id,
            event_id
        );

        /* create a request to send for service */
        auto request = vsomeip::runtime::get()->create_request(); 

        /* set request service id */
        request->set_service(service_id);
        
        /* set instance service id */
        request->set_instance(instance_id);
        
        /* set method id */
        request->set_method(method_id);

        /* prompt the user to enter a number */
        uint32_t input_num = 0;
        LOG_INF("Enter a number to send to the service: \n");
        std::cin >> input_num;

        /* create a variable to hold payload data for the request */
        std::vector<vsomeip::byte_t> payload_data(sizeof(uint32_t));

        /* use std::copy to insert data into variable while keeping track of endianness */
        /* reinterpret_cast is used with memory layout manipulation shenanigans */
        /* there is an alternative to this line and it is std::bit_cast but it is supported in C++20 and I want to be as close to C++11 as possible */
        std::copy(
            reinterpret_cast<const uint8_t*>(&input_num),
            reinterpret_cast<const uint8_t*>(&input_num) + sizeof(uint32_t),
            payload_data.begin()
        );

        /* create payload */
        auto payload = vsomeip::runtime::get()->create_payload();

        /* set payload data to the variable above */
        payload->set_data(payload_data);

        /* set request payload to the payload variable above */
        request->set_payload(payload);

        /* send request */
        app->send(request);
    }
}

class client_app
{
    private:
        /* shared ptr for app instance */
        std::shared_ptr<vsomeip::application> _app_ptr;

    public:
        bool init(void)
        {
            /* create an instance for the app with name "client" */
            _app_ptr = vsomeip::runtime::get()->create_application("client");

            /* once a service with given id is available, call the method onServiceAvailable */
            _app_ptr->register_availability_handler(
                service_id,
                instance_id,
                onServiceAvailable
            );

            /* once a message from the service with given id is received, call the method onMessageEventHandler */
            _app_ptr->register_message_handler(
                service_id,
                instance_id, 
                method_id,
                onMessageEventHandler
            );

            /* call standard vsomeip app init */
            _app_ptr->init();
            
            return true;
        }

        void start(void)
        {
            /* call standard vsomeip app start */
            _app_ptr->start();
        }

        void stop(void) 
        {
            /* undo availability handler shenanigans */
            _app_ptr->unregister_availability_handler(
                service_id, 
                instance_id
            );
            
            /* undo message handler shenanigans */
            _app_ptr->unregister_message_handler(
                service_id, 
                instance_id, 
                method_id
            );

            /* call standard vsomeip app stop */
            _app_ptr->stop();
        }
};