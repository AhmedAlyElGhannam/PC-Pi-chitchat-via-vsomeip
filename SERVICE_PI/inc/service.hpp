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

void onClientRequestHandler(const std::shared_ptr<vsomeip::message> &_request) 
{
    LOG_INF("Received request! Privet!\n");
    
    auto app = vsomeip::runtime::get()->get_application("service");
    
    std::shared_ptr<vsomeip::payload> pl = _request->get_payload();

    std::vector<vsomeip::byte_t> &payload_data = pl->get_data();

    uint32_t received_num = 0;

    std::copy(
        payload_data.begin(),
        payload_data.begin() + sizeof(uint32_t),
        reinterpret_cast<uint8_t*>(&received_num)
    );

    std::cout << "[INFO] Received number: " << received_num << std::endl;

    uint32_t result = input_number + 90; 
    
    /*
    // Prepare event
    std::shared_ptr<vsomeip::payload> event_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> event_data;
    event_data.push_back(static_cast<vsomeip::byte_t>(result));
    event_payload->set_data(event_data);

    // Send event to all subscribers
    app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, event_payload);
    */
}

class service_app
{
    private:
        std::shared_ptr<vsomeip::application> _app_ptr;

    public:
        bool init(void)
        {
            _app_ptr = vsomeip::runtime::get()->create_application("service");
            
            _app_ptr->register_message_handler(
                service_id,
                instance_id,
                method_id,
                onClientRequestHandler
            );

            _app_ptr->init();

            _app_ptr->offer_service(
                service_id, 
                instance_id
            ); 

            app->offer_event(
                service_id,
                instance_id, 
                event_id, 
                vsomeip::event_type_e::ET_FIELD,
                true
            );

            return true;
        }

        void start(void)
        {
            _app_ptr->start();
        }

        void stop(void) 
        {
            _app_ptr->unregister_availability_handler(service_id, instance_id);
            _app_ptr->unregister_message_handler(vsomeip::ANY_SERVICE, instance_id, method_id);
            _app_ptr->stop();
        }
};