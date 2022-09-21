//
// Created by max on 20.09.2022.
//

#include <boost/json.hpp>
#include <utility>

#include "async_message_converter.h"

#include "tick.pb.h"


void AsyncMessageConverter::enqueue_message(WebSocketMsgPtr msg)
{
    std::lock_guard lk(m_mtx);
    m_queue.push(std::move(msg));
    m_cv.notify_one();
}

AsyncMessageConverter::AsyncMessageConverter(ProcessedHandler handler) :
    m_on_processed(std::move(handler)),
    m_stopped(false)
{
}

void AsyncMessageConverter::process_queue()
{
    while (!m_stopped) {
        std::unique_lock lk(m_mtx);
        m_cv.wait(lk, [this] { return m_stopped || !m_queue.empty(); });
        if (m_stopped)
            return;

        auto msg = m_queue.front();
        m_queue.pop();
        lk.unlock();

        boost::system::error_code ec;
        auto json = boost::json::parse(msg->get_payload(), ec);
        if (ec) {
            std::cerr << "Error processing json: " << ec.message() << std::endl;
            continue;
        }

        Tick tick;
        try {
            std::string type = json.at("type").as_string().data();
            if (type != "ticker")
                continue;

            std::string ticker = json.at("product_id").as_string().data();
            std::string best_bid = json.at("best_bid").as_string().data();
            std::string best_ask = json.at("best_ask").as_string().data();
            tick.set_version(PROTO_VERSION);
            tick.set_ticker(ticker);
            tick.set_best_bid(best_bid);
            tick.set_best_ask(best_ask);
        }
        catch (std::exception& ex) {
            std::cerr << "Error parsing data: " << ex.what() << std::endl;
            continue;
        }

        auto data = std::make_shared<std::vector<char>>(tick.ByteSizeLong() + 1);
        (*data)[0] = static_cast<char>(tick.ByteSizeLong()); // Our ticks < 127

        tick.SerializeToArray(data->data() + 1, (int)tick.ByteSizeLong());

        m_on_processed(std::move(data));
    }
}

void AsyncMessageConverter::run()
{
    m_processing_thread = std::thread(&AsyncMessageConverter::process_queue, this);
}
