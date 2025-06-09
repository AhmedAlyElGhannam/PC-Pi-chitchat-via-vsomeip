#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#define SAMPLE_SERVICE_ID  0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID   0x0421
#define SAMPLE_EVENT_ID    0x1235

std::shared_ptr<vsomeip::application> app;
std::mutex mutex;
std::condition_variable condition;

void send_message() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock); // wait for service availability

    std::shared_ptr<vsomeip::message> msg = vsomeip::runtime::get()->create_request();
    msg->set_service(SAMPLE_SERVICE_ID);
    msg->set_instance(SAMPLE_INSTANCE_ID);
    msg->set_method(SAMPLE_METHOD_ID);

    std::vector<vsomeip::byte_t> payload_data;
    payload_data.push_back(60); // send 60
    auto payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(payload_data);
    msg->set_payload(payload);

    app->send(msg);
    std::cout << "CLIENT: Sent 60 to service" << std::endl;
}

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "CLIENT: Service is " << (_is_available ? "available" : "not available") << std::endl;
    if (_is_available) {
        app->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID);
        condition.notify_one(); // trigger sending
    }
}

void on_event(const std::shared_ptr<vsomeip::message> &_msg) {
    auto payload = _msg->get_payload();
    std::vector<vsomeip::byte_t> data = payload->get_data();
    int result = static_cast<int>(data[0]);

    std::cout << "CLIENT: Received result: " << result << std::endl;
}

int main() {
    app = vsomeip::runtime::get()->create_application("Client");
    app->init();

    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, on_event);

    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

    std::thread sender(send_message);
    app->start();
}

