/*
 * Copyright 2022 Ciro Spaciari
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <thread>
#include <mutex>
#include <condition_variable>

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "libusockets.h"
#include <string_view>
#include "App.h"
#include <optional>

#include <uv.h>

struct Worker {
    int ssl;
    std::shared_ptr<uWS::App> app;
    struct uWS::Loop* loop;
    std::shared_ptr<std::thread> thread;
};

extern "C"
{
    struct uws_worket_s;
    typedef uws_worket_s uws_worker_t;

    uws_worker_t* uws_create_app() {
        Worker* worker = new Worker();
        std::mutex m;
        std::condition_variable cv;
        worker->thread = std::make_shared<std::thread>([worker, &cv](){
            worker->ssl = 0;
            uv_loop_t* uv_loop = uv_default_loop();
            uv_async_t async;
            // we keep one task in the queue so that uv loop starts processing precb, wakecb, post cb, on which uws is running
            uv_async_init(uv_loop, &async, [](uv_async_t* handle){});
            worker->loop = uWS::Loop::get(uv_loop);
            worker->loop->integrate();
            worker->app = std::make_shared<uWS::App>();
            cv.notify_one();
            while (true) {
                uv_run(uv_loop, UV_RUN_DEFAULT);
                // should we gracefuly shutdown?
            }
        });
        worker->thread->detach();
        // waiting while w->loop initialized, so deno can add tasks to it
        std::unique_lock lk(m);
        cv.wait(lk);
        return (uws_worker_t*) worker;
    }

    void uws_app_get(uws_worker_t* worker, const char *pattern, uws_method_handler handler,)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([w]() {
            w->app->get("/*", [](auto *res, auto *req) {
            });
        });
    }

    void uws_app_listen(uws_worker_t* worker)
    {
        Worker* w = (Worker*) worker;
        w->loop->defer([w]() {
            w->app->listen(9001, [](us_listen_socket_t *sock) {
            });
        });
    }
}