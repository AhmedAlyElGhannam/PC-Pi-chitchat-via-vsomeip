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
    LOG_INF("Received request! Privet!");

    auto app = vsomeip::runtime::get()->get_application("service");

    std::shared_ptr<vsomeip::payload> pl = _request->get_payload();

    const vsomeip::byte_t* data_ptr = pl->get_data();
    std::size_t data_len = pl->get_length();

    uint32_t received_num = 0;
    if (data_len >= sizeof(uint32_t)) {
        std::copy(
            data_ptr,
            data_ptr + sizeof(uint32_t),
            reinterpret_cast<vsomeip::byte_t*>(&received_num)
        );
    }

    std::cout << "[INFO] Received number: " << received_num << std::endl;

    uint32_t result = received_num + 90;

    std::vector<vsomeip::byte_t> event_data(sizeof(uint32_t));
    std::copy(
        reinterpret_cast<vsomeip::byte_t*>(&result),
        reinterpret_cast<vsomeip::byte_t*>(&result) + sizeof(uint32_t),
        event_data.begin()
    );

    std::shared_ptr<vsomeip::payload> event_payload = vsomeip::runtime::get()->create_payload();
    event_payload->set_data(event_data);

    app->notify(service_id, instance_id, event_id, event_payload);
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

            std::set<vsomeip::eventgroup_t> event_groups = { 0x01 }; 
            _app_ptr->offer_event(
                service_id,
                instance_id, 
                event_id, 
                event_groups,
                vsomeip::event_type_e::ET_FIELD,
                std::chrono::milliseconds::zero(),
                true,  // is_reliable
                false  // is_lazy
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