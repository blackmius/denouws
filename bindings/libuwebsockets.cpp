/*
 * based on Ciro Spaciari uws capi
 */

#include "libuwebsockets.h"
#include <string_view>
#include "App.h"
#include <optional>
extern "C"
{
    struct Worker {
        uws_app_t *app;
        struct uWS::Loop *loop;
        std::shared_ptr<std::thread> thread;
        std::condition_variable is_running;
    };

    uws_worker_t *uws_create_app(int ssl, struct us_socket_context_options_t options) {
        Worker *worker = new Worker();
        std::mutex m;
        std::condition_variable cv;
        worker->thread = std::make_shared<std::thread>([worker, &cv, ssl, &options](){
            uv_loop_t *uv_loop = uv_default_loop();
            uv_async_t async;
            // we keep one task in the queue so that uv loop starts processing precb, wakecb, post cb, on which uws is running
            uv_async_init(uv_loop, &async, [](uv_async_t *handle){});
            worker->loop = uWS::Loop::get(uv_loop);
            worker->loop->integrate();

            if (ssl)
            {
                uWS::SocketContextOptions sco;
                sco.ca_file_name = options.ca_file_name;
                sco.cert_file_name = options.cert_file_name;
                sco.dh_params_file_name = options.dh_params_file_name;
                sco.key_file_name = options.key_file_name;
                sco.passphrase = options.passphrase;
                sco.ssl_prefer_low_memory_usage = options.ssl_prefer_low_memory_usage;
                sco.ssl_ciphers = options.ssl_ciphers;
                
                worker->app = (uws_app_t *) new uWS::SSLApp(sco);
            }
            else {
                worker->app = (uws_app_t *) new uWS::App();
            }
            cv.notify_one();
            while (true) {
                uv_run(uv_loop, UV_RUN_DEFAULT);
                // should we gracefuly shutdown?
            }
            worker->is_running.notify_one();
        });
        worker->thread->detach();
        // waiting while w->loop initialized, so deno can add tasks to it
        std::unique_lock lk(m);
        cv.wait(lk);
        return (uws_worker_t*) worker;
    }

    void uws_wait_app(uws_worker_t *worker)
    {
        Worker* w = (Worker*) worker;
        std::mutex m;
        std::unique_lock lk(m);
        w->is_running.wait(lk);
    }

    void uws_app_get(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->get(pattern, nullptr);
                    return;
                }
                uwsApp->get(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->get(pattern, nullptr);
                    return;
                }
                uwsApp->get(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_post(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->post(pattern, nullptr);
                    return;
                }
                uwsApp->post(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->post(pattern, nullptr);
                    return;
                }
                uwsApp->post(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_options(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->options(pattern, nullptr);
                    return;
                }
                uwsApp->options(pattern, [handler](auto *res, auto *req)
                                { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->options(pattern, nullptr);
                    return;
                }
                uwsApp->options(pattern, [handler](auto *res, auto *req)
                                { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_delete(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->del(pattern, nullptr);
                    return;
                }
                uwsApp->del(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->del(pattern, nullptr);
                    return;
                }
                uwsApp->del(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_patch(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->patch(pattern, nullptr);
                    return;
                }
                uwsApp->patch(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->patch(pattern, nullptr);
                    return;
                }
                uwsApp->patch(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_put(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->put(pattern, nullptr);
                    return;
                }
                uwsApp->put(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->put(pattern, nullptr);
                    return;
                }
                uwsApp->put(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_head(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->head(pattern, nullptr);
                    return;
                }
                uwsApp->head(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->head(pattern, nullptr);
                    return;
                }
                uwsApp->head(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_connect(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->connect(pattern, nullptr);
                    return;
                }
                uwsApp->connect(pattern, [handler](auto *res, auto *req)
                                { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->connect(pattern, nullptr);
                    return;
                }
                uwsApp->connect(pattern, [handler](auto *res, auto *req)
                                { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }

    void uws_app_trace(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->trace(pattern, nullptr);
                    return;
                }
                uwsApp->trace(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->trace(pattern, nullptr);
                    return;
                }
                uwsApp->trace(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }
    void uws_app_any(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->any(pattern, nullptr);
                    return;
                }
                uwsApp->any(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                if (handler == nullptr)
                {
                    uwsApp->any(pattern, nullptr);
                    return;
                }
                uwsApp->any(pattern, [handler](auto *res, auto *req)
                            { handler((uws_res_t *)res, (uws_req_t *)req); });
            }
        });
    }

    void uws_app_listen(int ssl, uws_worker_t *worker, int port, uws_listen_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, port, handler]() {
            uws_app_listen_config_t config;
            config.port = port;
            config.host = nullptr;
            config.options = 0;

            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->listen(port, [handler, config](struct us_listen_socket_t *listen_socket)
                            { handler((struct us_listen_socket_t *)listen_socket, config); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;

                uwsApp->listen(port, [handler, config](struct us_listen_socket_t *listen_socket)
                            { handler((struct us_listen_socket_t *)listen_socket, config); });
            }
        });
    }

    void uws_app_listen_with_config(int ssl, uws_worker_t *worker, uws_app_listen_config_t config, uws_listen_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, config, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->listen(config.host, config.port, config.options, [handler, config](struct us_listen_socket_t *listen_socket)
                            { handler((struct us_listen_socket_t *)listen_socket, config); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->listen(config.host, config.port, config.options, [handler, config](struct us_listen_socket_t *listen_socket)
                            { handler((struct us_listen_socket_t *)listen_socket, config); });
            }
        });
    }

    /* callback, path to unix domain socket */
    void uws_app_listen_domain(int ssl, uws_worker_t *worker, const char *domain, size_t domain_length, uws_listen_domain_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, domain, domain_length, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->listen([handler, domain, domain_length](struct us_listen_socket_t *listen_socket)
                            { handler((struct us_listen_socket_t *)listen_socket, domain, domain_length, 0); },
                            std::string(domain, domain_length));
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;

                uwsApp->listen([handler, domain, domain_length](struct us_listen_socket_t *listen_socket)
                            { handler((struct us_listen_socket_t *)listen_socket, domain, domain_length, 0); },
                            std::string(domain, domain_length));
            }
        });
    }

    /* callback, path to unix domain socket */
    void uws_app_listen_domain_with_options(int ssl, uws_worker_t *worker, const char *domain, size_t domain_length, int options, uws_listen_domain_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, domain, domain_length, options, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->listen(
                    options, [handler, domain, domain_length, options](struct us_listen_socket_t *listen_socket)
                    { handler((struct us_listen_socket_t *)listen_socket, domain, domain_length, options); },
                    std::string(domain, domain_length));
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;

                uwsApp->listen(
                    options, [handler, domain, domain_length, options](struct us_listen_socket_t *listen_socket)
                    { handler((struct us_listen_socket_t *)listen_socket, domain, domain_length, options); },
                    std::string(domain, domain_length));
            }
        });
    }
    void uws_app_domain(int ssl, uws_worker_t *worker, const char *server_name, size_t server_name_length)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, server_name, server_name_length]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->domain(std::string(server_name, server_name_length));
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->domain(std::string(server_name, server_name_length));
            }
        });
    }

    unsigned int uws_num_subscribers(int ssl, uws_worker_t *worker, const char *topic, size_t topic_length)
    {
        Worker* w = (Worker*) worker;
        if (ssl)
        {
            uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
            return uwsApp->numSubscribers(std::string_view(topic, topic_length));
        }
        uWS::App *uwsApp = (uWS::App *)w->app;
        return uwsApp->numSubscribers(std::string_view(topic, topic_length));
    }
    bool uws_publish(int ssl, uws_worker_t *worker, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress)
    {
        std::mutex m;
        std::condition_variable cv;
        bool result;
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, topic, topic_length, message, message_length, opcode, compress, &result, &cv]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                result = uwsApp->publish(std::string_view(topic, topic_length), std::string_view(message, message_length), (uWS::OpCode)(unsigned char)opcode, compress);
            }
            else {
                uWS::App *uwsApp = (uWS::App *)w->app;
                result = uwsApp->publish(std::string_view(topic, topic_length), std::string_view(message, message_length), (uWS::OpCode)(unsigned char)opcode, compress);
            }
            cv.notify_one();
        });
        std::unique_lock lk(m);
        cv.wait(lk);
        return result;
    }

    void uws_remove_server_name(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, hostname_pattern, hostname_pattern_length]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->removeServerName(std::string(hostname_pattern, hostname_pattern_length));
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->removeServerName(std::string(hostname_pattern, hostname_pattern_length));
            }
        });
    }
    void uws_add_server_name(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, hostname_pattern, hostname_pattern_length]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->addServerName(std::string(hostname_pattern, hostname_pattern_length));
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->addServerName(std::string(hostname_pattern, hostname_pattern_length));
            }
        });
    }
    void uws_add_server_name_with_options(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length, struct us_socket_context_options_t options)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, hostname_pattern, hostname_pattern_length, options]() {
            uWS::SocketContextOptions sco;
            sco.ca_file_name = options.ca_file_name;
            sco.cert_file_name = options.cert_file_name;
            sco.dh_params_file_name = options.dh_params_file_name;
            sco.key_file_name = options.key_file_name;
            sco.passphrase = options.passphrase;
            sco.ssl_prefer_low_memory_usage = options.ssl_prefer_low_memory_usage;

            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->addServerName(std::string(hostname_pattern, hostname_pattern_length), sco);
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->addServerName(std::string(hostname_pattern, hostname_pattern_length), sco);
            }
        });
    }

    void uws_missing_server_name(int ssl, uws_worker_t *worker, uws_missing_server_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->missingServerName([handler](auto hostname)
                                        { handler(hostname, strlen(hostname)); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->missingServerName([handler](auto hostname)
                                        { handler(hostname, strlen(hostname)); });
            }
        });
    }
    void uws_filter(int ssl, uws_worker_t *worker, uws_filter_handler handler)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, handler]() {
            if (ssl)
            {
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;
                uwsApp->filter([handler](auto res, auto i)
                            { handler((uws_res_t *)res, i); });
            }
            else
            {
                uWS::App *uwsApp = (uWS::App *)w->app;

                uwsApp->filter([handler](auto res, auto i)
                            { handler((uws_res_t *)res, i); });
            }
        });
    }

    void uws_ws(int ssl, uws_worker_t *worker, const char *pattern, uws_socket_behavior_t behavior)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, w, pattern, behavior]() {
            if (ssl)
            {
                auto generic_handler = uWS::SSLApp::WebSocketBehavior<void *>{
                    .compression = (uWS::CompressOptions)(uint64_t)behavior.compression,
                    .maxPayloadLength = behavior.maxPayloadLength,
                    .idleTimeout = behavior.idleTimeout,
                    .maxBackpressure = behavior.maxBackpressure,
                    .closeOnBackpressureLimit = behavior.closeOnBackpressureLimit,
                    .resetIdleTimeoutOnSend = behavior.resetIdleTimeoutOnSend,
                    .sendPingsAutomatically = behavior.sendPingsAutomatically,
                    .maxLifetime = behavior.maxLifetime,
                };

                if (behavior.upgrade)
                    generic_handler.upgrade = [behavior](auto *res, auto *req, auto *context)
                    {
                        behavior.upgrade((uws_res_t *)res, (uws_req_t *)req, (uws_socket_context_t *)context);
                    };
                if (behavior.open)
                    generic_handler.open = [behavior](auto *ws)
                    {
                        behavior.open((uws_websocket_t *)ws);
                    };
                if (behavior.message)
                    generic_handler.message = [behavior](auto *ws, auto message, auto opcode)
                    {
                        behavior.message((uws_websocket_t *)ws, message.data(), message.length(), (uws_opcode_t)opcode);
                    };
                if (behavior.drain)
                    generic_handler.drain = [behavior](auto *ws)
                    {
                        behavior.drain((uws_websocket_t *)ws);
                    };
                if (behavior.ping)
                    generic_handler.ping = [behavior](auto *ws, auto message)
                    {
                        behavior.ping((uws_websocket_t *)ws, message.data(), message.length());
                    };
                if (behavior.pong)
                    generic_handler.pong = [behavior](auto *ws, auto message)
                    {
                        behavior.pong((uws_websocket_t *)ws, message.data(), message.length());
                    };
                if (behavior.close)
                    generic_handler.close = [behavior](auto *ws, int code, auto message)
                    {
                        behavior.close((uws_websocket_t *)ws, code, message.data(), message.length());
                    };
                if (behavior.subscription)
                    generic_handler.subscription = [behavior](auto *ws, auto topic, int subscribers, int old_subscribers){
                        behavior.subscription((uws_websocket_t *)ws, topic.data(), topic.length(), subscribers, old_subscribers);

                    };
                uWS::SSLApp *uwsApp = (uWS::SSLApp *)w->app;

                uwsApp->ws<void *>(pattern, std::move(generic_handler));
            }
            else
            {
                auto generic_handler = uWS::App::WebSocketBehavior<void *>{
                    .compression = (uWS::CompressOptions)(uint64_t)behavior.compression,
                    .maxPayloadLength = behavior.maxPayloadLength,
                    .idleTimeout = behavior.idleTimeout,
                    .maxBackpressure = behavior.maxBackpressure,
                    .closeOnBackpressureLimit = behavior.closeOnBackpressureLimit,
                    .resetIdleTimeoutOnSend = behavior.resetIdleTimeoutOnSend,
                    .sendPingsAutomatically = behavior.sendPingsAutomatically,
                    .maxLifetime = behavior.maxLifetime,
                };

                if (behavior.upgrade)
                    generic_handler.upgrade = [behavior](auto *res, auto *req, auto *context)
                    {
                        behavior.upgrade((uws_res_t *)res, (uws_req_t *)req, (uws_socket_context_t *)context);
                    };
                if (behavior.open)
                    generic_handler.open = [behavior](auto *ws)
                    {
                        behavior.open((uws_websocket_t *)ws);
                    };
                if (behavior.message)
                    generic_handler.message = [behavior](auto *ws, auto message, auto opcode)
                    {
                        behavior.message((uws_websocket_t *)ws, message.data(), message.length(), (uws_opcode_t)opcode);
                    };
                if (behavior.drain)
                    generic_handler.drain = [behavior](auto *ws)
                    {
                        behavior.drain((uws_websocket_t *)ws);
                    };
                if (behavior.ping)
                    generic_handler.ping = [behavior](auto *ws, auto message)
                    {
                        behavior.ping((uws_websocket_t *)ws, message.data(), message.length());
                    };
                if (behavior.pong)
                    generic_handler.pong = [behavior](auto *ws, auto message)
                    {
                        behavior.pong((uws_websocket_t *)ws, message.data(), message.length());
                    };
                if (behavior.close)
                    generic_handler.close = [behavior](auto *ws, int code, auto message)
                    {
                        behavior.close((uws_websocket_t *)ws, code, message.data(), message.length());
                    };
                if (behavior.subscription)
                    generic_handler.subscription = [behavior](auto *ws, auto topic, int subscribers, int old_subscribers){
                        behavior.subscription((uws_websocket_t *)ws, topic.data(), topic.length(), subscribers, old_subscribers);

                    };
                uWS::App *uwsApp = (uWS::App *)w->app;
                uwsApp->ws<void *>(pattern, std::move(generic_handler));
            }
        });
    }

    void uws_ws_close(int ssl, uws_worker_t *worker, uws_websocket_t *ws)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            uws->close();
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            uws->close();
        }
    }

    uws_sendstatus_t uws_ws_send(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            return (uws_sendstatus_t)uws->send(std::string_view(message, length), (uWS::OpCode)(unsigned char)opcode);
        }
        else {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            return (uws_sendstatus_t)uws->send(std::string_view(message, length), (uWS::OpCode)(unsigned char)opcode);
        }
    }

    uws_sendstatus_t uws_ws_send_with_options(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode, bool compress, bool fin)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            return (uws_sendstatus_t)uws->send(std::string_view(message, length), (uWS::OpCode)(unsigned char)opcode, compress, fin);
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            return (uws_sendstatus_t)uws->send(std::string_view(message, length), (uWS::OpCode)(unsigned char)opcode, compress, fin);
        }
    }

    void uws_ws_end(int ssl, uws_worker_t *worker, uws_websocket_t *ws, int code, const char *message, size_t length)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            uws->end(code, std::string_view(message, length));
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            uws->end(code, std::string_view(message, length));
        }
    }

    void uws_ws_cork(int ssl, uws_worker_t *worker, uws_websocket_t *ws, void (*handler)())
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            uws->cork([handler]()
                    { handler(); });
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;

            uws->cork([handler]()
                    { handler(); });
        }
    }
    bool uws_ws_subscribe(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            return uws->subscribe(std::string_view(topic, length));
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            return uws->subscribe(std::string_view(topic, length));
        }
    }
    bool uws_ws_unsubscribe(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            return uws->unsubscribe(std::string_view(topic, length));
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            return uws->unsubscribe(std::string_view(topic, length));
        }
    }

    bool uws_ws_is_subscribed(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            return uws->isSubscribed(std::string_view(topic, length));
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            return uws->isSubscribed(std::string_view(topic, length));
        }
    }
    void uws_ws_iterate_topics(int ssl, uws_worker_t *worker, uws_websocket_t *ws, void (*callback)(const char *topic, size_t length))
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            uws->iterateTopics([callback](auto topic)
                            { callback(topic.data(), topic.length()); });
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            uws->iterateTopics([callback](auto topic)
                            { callback(topic.data(), topic.length()); });
        }
    }

    bool uws_ws_publish(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length)
    {
        std::mutex m;
        std::condition_variable cv;
        bool result;
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, ws, topic, topic_length, message, message_length, &cv, &result]() {
            if (ssl)
            {
                uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
                result = uws->publish(std::string_view(topic, topic_length), std::string_view(message, message_length));
            }
            else
            {
                uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
                result = uws->publish(std::string_view(topic, topic_length), std::string_view(message, message_length));
            }
            cv.notify_one();
        });
        std::unique_lock lk(m);
        cv.wait(lk);
        return result;
    }

    bool uws_ws_publish_with_options(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress)
    {
        std::mutex m;
        std::condition_variable cv;
        bool result;
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, ws, topic, topic_length, message, message_length, opcode, compress, &cv, &result]() {
            if (ssl)
            {
                uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
                result = uws->publish(std::string_view(topic, topic_length), std::string_view(message, message_length), (uWS::OpCode)(unsigned char)opcode, compress);
            }
            else
            {
                uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
                result = uws->publish(std::string_view(topic, topic_length), std::string_view(message, message_length), (uWS::OpCode)(unsigned char)opcode, compress);
            }
            cv.notify_one();
        });
        std::unique_lock lk(m);
        cv.wait(lk);
        return result;
    }

    unsigned int uws_ws_get_buffered_amount(int ssl, uws_worker_t *worker, uws_websocket_t *ws)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            return uws->getBufferedAmount();
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            return uws->getBufferedAmount();
        }
    }


    size_t uws_ws_get_remote_address(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char **dest)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            std::string_view value = uws->getRemoteAddress();
            *dest = value.data();
            return value.length();
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            std::string_view value = uws->getRemoteAddress();
            *dest = value.data();
            return value.length();
        }
    }

    size_t uws_ws_get_remote_address_as_text(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char **dest)
    {
        if (ssl)
        {
            uWS::WebSocket<true, true, void *> *uws = (uWS::WebSocket<true, true, void *> *)ws;
            std::string_view value = uws->getRemoteAddressAsText();
            *dest = value.data();
            return value.length();
        }
        else
        {
            uWS::WebSocket<false, true, void *> *uws = (uWS::WebSocket<false, true, void *> *)ws;
            std::string_view value = uws->getRemoteAddressAsText();
            *dest = value.data();
            return value.length();
        }
    }

    void uws_res_end(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length, bool close_connection)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->end(std::string_view(data, length), close_connection);
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->end(std::string_view(data, length), close_connection);
        }
    }

    size_t uws_res_get_remote_address(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            std::string_view value = uwsRes->getRemoteAddress();
            *dest = value.data();
            return value.length();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            std::string_view value = uwsRes->getRemoteAddress();
            *dest = value.data();
            return value.length();
        }
    }

    size_t uws_res_get_remote_address_as_text(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            std::string_view value = uwsRes->getRemoteAddressAsText();
            *dest = value.data();
            return value.length();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            std::string_view value = uwsRes->getRemoteAddressAsText();
            *dest = value.data();
            return value.length();
        }
    }

    size_t uws_res_get_proxied_remote_address(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            std::string_view value = uwsRes->getProxiedRemoteAddress();
            *dest = value.data();
            return value.length();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            std::string_view value = uwsRes->getProxiedRemoteAddress();
            *dest = value.data();
            return value.length();
        }
    }

    size_t uws_res_get_proxied_remote_address_as_text(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            std::string_view value = uwsRes->getProxiedRemoteAddressAsText();
            *dest = value.data();
            return value.length();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            std::string_view value = uwsRes->getProxiedRemoteAddressAsText();
            *dest = value.data();
            return value.length();
        }
    }

    uws_try_end_result_t uws_res_try_end(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length, uintmax_t total_size, bool close_connection)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            std::pair<bool, bool> result = uwsRes->tryEnd(std::string_view(data, length), total_size, close_connection);
            return uws_try_end_result_t{
                .ok = result.first,
                .has_responded = result.second,
            };
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            std::pair<bool, bool> result = uwsRes->tryEnd(std::string_view(data, length), total_size);
            return uws_try_end_result_t{
                .ok = result.first,
                .has_responded = result.second,
            };
        }
    }

    void uws_res_cork(int ssl, uws_worker_t *worker, uws_res_t *res, void (*callback)(uws_res_t *res))
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->cork([=]()
                        { callback(res); });
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->cork([=]()
                        { callback(res); });
        }
    }

    void uws_res_pause(int ssl, uws_worker_t *worker, uws_res_t *res)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->pause();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->pause();
        }
    }

    void uws_res_resume(int ssl, uws_worker_t *worker, uws_res_t *res)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->pause();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->pause();
        }
    }

    void uws_res_write_continue(int ssl, uws_worker_t *worker, uws_res_t *res)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->writeContinue();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->writeContinue();
        }
    }

    void uws_res_write_status(int ssl, uws_worker_t *worker, uws_res_t *res, const char *status, size_t length)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->writeStatus(std::string_view(status, length));
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->writeStatus(std::string_view(status, length));
        }
    }

    void uws_res_write_header(int ssl, uws_worker_t *worker, uws_res_t *res, const char *key, size_t key_length, const char *value, size_t value_length)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->writeHeader(std::string_view(key, key_length), std::string_view(value, value_length));
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->writeHeader(std::string_view(key, key_length), std::string_view(value, value_length));
        }
    }
    void uws_res_write_header_int(int ssl, uws_worker_t *worker, uws_res_t *res, const char *key, size_t key_length, uint64_t value)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->writeHeader(std::string_view(key, key_length), value);
        }
        else
        {

            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->writeHeader(std::string_view(key, key_length), value);
        }
    }

    void uws_res_end_without_body(int ssl, uws_worker_t *worker, uws_res_t *res, bool close_connection)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->endWithoutBody(std::nullopt, close_connection);
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->endWithoutBody(std::nullopt, close_connection);
        }
    }

    bool uws_res_write(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            return uwsRes->write(std::string_view(data, length));
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            return uwsRes->write(std::string_view(data, length));
        }
    }
    uintmax_t uws_res_get_write_offset(int ssl, uws_worker_t *worker, uws_res_t *res)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            return uwsRes->getWriteOffset();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            return uwsRes->getWriteOffset();
        }
    }
    void uws_res_override_write_offset(int ssl, uws_worker_t *worker, uws_res_t *res, uintmax_t offset)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            uwsRes->overrideWriteOffset(offset);
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            uwsRes->overrideWriteOffset(offset);
        }
    }
    bool uws_res_has_responded(int ssl, uws_worker_t *worker, uws_res_t *res)
    {
        if (ssl)
        {
            uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
            return uwsRes->hasResponded();
        }
        else
        {
            uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
            return uwsRes->hasResponded();
        }
    }

    void uws_res_on_writable(int ssl, uws_worker_t *worker, uws_res_t *res, bool (*handler)(uws_res_t *res, uintmax_t))
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, res, handler]() {
            if (ssl)
            {
                uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
                uwsRes->onWritable([handler, res](uintmax_t a)
                                { return handler(res, a); });
            }
            else
            {
                uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
                uwsRes->onWritable([handler, res](uintmax_t a)
                                { return handler(res, a); });
            }
        });
    }

    void uws_res_on_aborted(int ssl, uws_worker_t *worker, uws_res_t *res, void (*handler)(uws_res_t *res))
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, res, handler]() {
            if (ssl)
            {
                uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
                uwsRes->onAborted([handler, res]
                                { handler(res); });
            }
            else
            {
                uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
                uwsRes->onAborted([handler, res]
                                { handler(res); });
            }
        });
    }

    void uws_res_on_data(int ssl, uws_worker_t *worker, uws_res_t *res, void (*handler)(uws_res_t *res, const char *chunk, size_t chunk_length, bool is_end))
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([ssl, res, handler]() {
            if (ssl)
            {
                uWS::HttpResponse<true> *uwsRes = (uWS::HttpResponse<true> *)res;
                uwsRes->onData([handler, res](auto chunk, bool is_end)
                            { handler(res, chunk.data(), chunk.length(), is_end); });
            }
            else
            {
                uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
                uwsRes->onData([handler, res](auto chunk, bool is_end)
                            { handler(res, chunk.data(), chunk.length(), is_end); });
            }
        });
    }

    bool uws_req_is_ancient(uws_worker_t *worker, uws_req_t *res)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        return uwsReq->isAncient();
    }

    bool uws_req_get_yield(uws_worker_t *worker, uws_req_t *res)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        return uwsReq->getYield();
    }

    void uws_req_set_field(uws_worker_t *worker, uws_req_t *res, bool yield)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        return uwsReq->setYield(yield);
    }

    size_t uws_req_get_url(uws_worker_t *worker, uws_req_t *res, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getUrl();
        *dest = value.data();
        return value.length();
    }

    size_t uws_req_get_full_url(uws_worker_t *worker, uws_req_t *res, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getFullUrl();
        *dest = value.data();
        return value.length();
    }

    size_t uws_req_get_method(uws_worker_t *worker, uws_req_t *res, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getMethod();
        *dest = value.data();
        return value.length();
    }

    size_t uws_req_get_case_sensitive_method(uws_worker_t *worker, uws_req_t *res, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getCaseSensitiveMethod();
        *dest = value.data();
        return value.length();
    }

    void uws_req_for_each_header(uws_worker_t *worker, uws_req_t *res, uws_get_headers_server_handler handler)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        for (auto header : *uwsReq)
        {
            handler(header.first.data(), header.first.length(), header.second.data(), header.second.length());
        }
    }

    size_t uws_req_get_header(uws_worker_t *worker, uws_req_t *res, const char *lower_case_header, size_t lower_case_header_length, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getHeader(std::string_view(lower_case_header, lower_case_header_length));
        *dest = value.data();
        return value.length();
    }

    size_t uws_req_get_query(uws_worker_t *worker, uws_req_t *res, const char *key, size_t key_length, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getQuery(std::string_view(key, key_length));
        *dest = value.data();
        return value.length();
    }

    size_t uws_req_get_parameter(uws_worker_t *worker, uws_req_t *res, unsigned short index, const char **dest)
    {
        uWS::HttpRequest *uwsReq = (uWS::HttpRequest *)res;
        std::string_view value = uwsReq->getParameter(index);
        *dest = value.data();
        return value.length();
    }

    void uws_res_upgrade(int ssl, uws_worker_t *worker, uws_res_t *res, void *data, const char *sec_web_socket_key, size_t sec_web_socket_key_length, const char *sec_web_socket_protocol, size_t sec_web_socket_protocol_length, const char *sec_web_socket_extensions, size_t sec_web_socket_extensions_length, uws_socket_context_t *ws)
    {
        uWS::HttpResponse<false> *uwsRes = (uWS::HttpResponse<false> *)res;
        uwsRes->template upgrade<void *>(data ? std::move(data) : NULL,
                                        std::string_view(sec_web_socket_key, sec_web_socket_key_length),
                                        std::string_view(sec_web_socket_protocol, sec_web_socket_protocol_length),
                                        std::string_view(sec_web_socket_extensions, sec_web_socket_extensions_length),
                                        (struct us_socket_context_t *)ws);
    }
}