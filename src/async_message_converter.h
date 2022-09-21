//
// Created by max on 20.09.2022.
//

#ifndef COINBASE_CLIENT_ASYNC_MESSAGE_CONVERTER_H
#define COINBASE_CLIENT_ASYNC_MESSAGE_CONVERTER_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "common_types.h"


class AsyncMessageConverter
{
public:
    using ProcessedHandler = std::function<void(std::shared_ptr<std::vector<char>>)>;

    explicit AsyncMessageConverter(ProcessedHandler handler);
    void enqueue_message(WebSocketMsgPtr msg);

    void run();

private:
    void process_queue();

    ProcessedHandler m_on_processed;
    std::queue<WebSocketMsgPtr> m_queue;
    std::thread m_processing_thread;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::atomic_bool m_stopped;

    static constexpr int32_t PROTO_VERSION = 1;
};


#endif //COINBASE_CLIENT_ASYNC_MESSAGE_CONVERTER_H
