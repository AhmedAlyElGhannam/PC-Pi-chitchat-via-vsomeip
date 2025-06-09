#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <vector>
#include <memory>

#define SAMPLE_SERVICE_ID  0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID   0x0421
#define SAMPLE_EVENT_ID    0x1235

std::shared_ptr<vsomeip::application> app;

void on_message(const std::shared_ptr<vsomeip::message> &_request) {
    std::shared_ptr<vsomeip::payload> pl = _request->get_payload();
    std::vector<vsomeip::byte_t> data = pl->get_data();

    // Extract number
    int input_number = static_cast<int>(data[0]); // assuming number fits in 1 byte

    // Apply equation: result = input + 90
    int result = input_number + 90;

    std::cout << "SERVICE: Received " << input_number << ", computed " << result << std::endl;

    // Prepare event
    std::shared_ptr<vsomeip::payload> event_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> event_data;
    event_data.push_back(static_cast<vsomeip::byte_t>(result));
    event_payload->set_data(event_data);

    // Send event to all subscribers
    app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, event_payload);
}

int main() {
    app = vsomeip::runtime::get()->create_application("Service");
    app->init();

    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);

    app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, vsomeip::event_type_e::ET_FIELD, true);

    app->start();
}

