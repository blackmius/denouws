/*
 * based on Ciro Spaciari uws capi
 */

#ifndef LIBUWS_CAPI_HEADER
#define LIBUWS_CAPI_HEADER

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "libusockets.h"
#include <uv.h>

#include <thread>
#include <mutex>
#include <condition_variable>

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef _WIN32
#  define DLL_EXPORT __declspec( dllexport )
#else
#  define DLL_EXPORT
#endif

    DLL_EXPORT typedef enum
    {
        /* These are not actual compression options */
        _COMPRESSOR_MASK = 0x00FF,
        _DECOMPRESSOR_MASK = 0x0F00,
        /* Disabled, shared, shared are "special" values */
        DISABLED = 0,
        SHARED_COMPRESSOR = 1,
        SHARED_DECOMPRESSOR = 1 << 8,
        /* Highest 4 bits describe decompressor */
        DEDICATED_DECOMPRESSOR_32KB = 15 << 8,
        DEDICATED_DECOMPRESSOR_16KB = 14 << 8,
        DEDICATED_DECOMPRESSOR_8KB = 13 << 8,
        DEDICATED_DECOMPRESSOR_4KB = 12 << 8,
        DEDICATED_DECOMPRESSOR_2KB = 11 << 8,
        DEDICATED_DECOMPRESSOR_1KB = 10 << 8,
        DEDICATED_DECOMPRESSOR_512B = 9 << 8,
        /* Same as 32kb */
        DEDICATED_DECOMPRESSOR = 15 << 8,

        /* Lowest 8 bit describe compressor */
        DEDICATED_COMPRESSOR_3KB = 9 << 4 | 1,
        DEDICATED_COMPRESSOR_4KB = 9 << 4 | 2,
        DEDICATED_COMPRESSOR_8KB = 10 << 4 | 3,
        DEDICATED_COMPRESSOR_16KB = 11 << 4 | 4,
        DEDICATED_COMPRESSOR_32KB = 12 << 4 | 5,
        DEDICATED_COMPRESSOR_64KB = 13 << 4 | 6,
        DEDICATED_COMPRESSOR_128KB = 14 << 4 | 7,
        DEDICATED_COMPRESSOR_256KB = 15 << 4 | 8,
        /* Same as 256kb */
        DEDICATED_COMPRESSOR = 15 << 4 | 8
    } uws_compress_options_t;

    DLL_EXPORT typedef enum
    {
        CONTINUATION = 0,
        TEXT = 1,
        BINARY = 2,
        CLOSE = 8,
        PING = 9,
        PONG = 10
    } uws_opcode_t;

    DLL_EXPORT typedef enum
    {
        BACKPRESSURE,
        SUCCESS,
        DROPPED
    } uws_sendstatus_t;

    DLL_EXPORT typedef struct
    {

        int port;
        const char *host;
        int options;
    } uws_app_listen_config_t;

    DLL_EXPORT typedef struct {
        bool ok;
        bool has_responded;
    } uws_try_end_result_t;

    DLL_EXPORT struct uws_worker_s;
    DLL_EXPORT struct uws_app_s;
    DLL_EXPORT struct uws_req_s;
    DLL_EXPORT struct uws_res_s;
    DLL_EXPORT struct uws_websocket_s;
    DLL_EXPORT struct uws_header_iterator_s;
    DLL_EXPORT typedef struct uws_worker_s uws_worker_t;
    DLL_EXPORT typedef struct uws_app_s uws_app_t;
    DLL_EXPORT typedef struct uws_req_s uws_req_t;
    DLL_EXPORT typedef struct uws_res_s uws_res_t;
    DLL_EXPORT typedef struct uws_socket_context_s uws_socket_context_t;
    DLL_EXPORT typedef struct uws_websocket_s uws_websocket_t;

    DLL_EXPORT typedef void (*uws_websocket_handler)(uws_websocket_t *ws);
    DLL_EXPORT typedef void (*uws_websocket_message_handler)(uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode);
    DLL_EXPORT typedef void (*uws_websocket_ping_pong_handler)(uws_websocket_t *ws, const char *message, size_t length);
    DLL_EXPORT typedef void (*uws_websocket_close_handler)(uws_websocket_t *ws, int code, const char *message, size_t length);
    DLL_EXPORT typedef void (*uws_websocket_upgrade_handler)(uws_res_t *response, uws_req_t *request, uws_socket_context_t *context);
    DLL_EXPORT typedef void (*uws_websocket_subscription_handler)(uws_websocket_t *ws, const char *topic_name, size_t topic_name_length, int new_number_of_subscriber, int old_number_of_subscriber);

    DLL_EXPORT typedef struct
    {
        uws_compress_options_t compression;
        /* Maximum message size we can receive */
        unsigned int maxPayloadLength;
        /* 2 minutes timeout is good */
        unsigned short idleTimeout;
        /* 64kb backpressure is probably good */
        unsigned int maxBackpressure;
        bool closeOnBackpressureLimit;
        /* This one depends on kernel timeouts and is a bad default */
        bool resetIdleTimeoutOnSend;
        /* A good default, esp. for newcomers */
        bool sendPingsAutomatically;
        /* Maximum socket lifetime in seconds before forced closure (defaults to disabled) */
        unsigned short maxLifetime;
        uws_websocket_upgrade_handler upgrade;
        uws_websocket_handler open;
        uws_websocket_message_handler message;
        uws_websocket_handler drain;
        uws_websocket_ping_pong_handler ping;
        uws_websocket_ping_pong_handler pong;
        uws_websocket_close_handler close;
        uws_websocket_subscription_handler subscription;
    } uws_socket_behavior_t;

    DLL_EXPORT typedef void (*uws_listen_handler)(struct us_listen_socket_t *listen_socket, uws_app_listen_config_t config);
    DLL_EXPORT typedef void (*uws_listen_domain_handler)(struct us_listen_socket_t *listen_socket, const char* domain, size_t domain_length, int options);
    DLL_EXPORT typedef void (*uws_method_handler)(uws_res_t *response, uws_req_t *request);
    DLL_EXPORT typedef void (*uws_filter_handler)(uws_res_t *response, int);
    DLL_EXPORT typedef void (*uws_missing_server_handler)(const char *hostname, size_t hostname_length);
    DLL_EXPORT typedef void (*uws_get_headers_server_handler)(const char *header_name, size_t header_name_size, const char *header_value, size_t header_value_size);
    //Basic HTTP
    DLL_EXPORT uws_worker_t *uws_create_app(int ssl, struct us_socket_context_options_t options);
    DLL_EXPORT void uws_wait_app(uws_worker_t *worker);
    DLL_EXPORT void uws_app_get(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_post(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_options(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_delete(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_patch(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_put(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_head(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_connect(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_trace(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
    DLL_EXPORT void uws_app_any(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);

    DLL_EXPORT void uws_app_listen(int ssl, uws_worker_t *worker, int port, uws_listen_handler handler);
    DLL_EXPORT void uws_app_listen_with_config(int ssl, uws_worker_t *worker, uws_app_listen_config_t config, uws_listen_handler handler);
    DLL_EXPORT void uws_app_listen_domain(int ssl, uws_worker_t *worker, const char *domain, size_t domain_length, uws_listen_domain_handler handler);
    DLL_EXPORT void uws_app_listen_domain_with_options(int ssl, uws_worker_t *worker, const char *domain,size_t domain_length, int options, uws_listen_domain_handler handler);
    DLL_EXPORT void uws_app_domain(int ssl, uws_worker_t *worker, const char* server_name, size_t server_name_length);

    DLL_EXPORT unsigned int uws_num_subscribers(int ssl, uws_worker_t *worker, const char *topic, size_t topic_length);
    DLL_EXPORT bool uws_publish(int ssl, uws_worker_t *worker, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress);
    DLL_EXPORT void uws_remove_server_name(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length);
    DLL_EXPORT void uws_add_server_name(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length);
    DLL_EXPORT void uws_add_server_name_with_options(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length, struct us_socket_context_options_t options);
    DLL_EXPORT void uws_missing_server_name(int ssl, uws_worker_t *worker, uws_missing_server_handler handler);
    DLL_EXPORT void uws_filter(int ssl, uws_worker_t *worker, uws_filter_handler handler);

    //WebSocket
    DLL_EXPORT void uws_ws(int ssl, uws_worker_t *worker, const char *pattern, uws_socket_behavior_t behavior);
    DLL_EXPORT void uws_ws_close(int ssl, uws_worker_t *worker, uws_websocket_t *ws);
    DLL_EXPORT uws_sendstatus_t uws_ws_send(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode);
    DLL_EXPORT uws_sendstatus_t uws_ws_send_with_options(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode, bool compress, bool fin);
    DLL_EXPORT uws_sendstatus_t uws_ws_send_fragment(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, bool compress);
    DLL_EXPORT uws_sendstatus_t uws_ws_send_first_fragment(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, bool compress);
    DLL_EXPORT uws_sendstatus_t uws_ws_send_first_fragment_with_opcode(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode, bool compress);
    DLL_EXPORT uws_sendstatus_t uws_ws_send_last_fragment(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, bool compress);
    DLL_EXPORT void uws_ws_end(int ssl, uws_worker_t *worker, uws_websocket_t *ws, int code, const char *message, size_t length);
    DLL_EXPORT void uws_ws_cork(int ssl, uws_worker_t *worker, uws_websocket_t *ws, void (*handler)());

    DLL_EXPORT bool uws_ws_subscribe(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length);
    DLL_EXPORT bool uws_ws_unsubscribe(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length);
    DLL_EXPORT bool uws_ws_is_subscribed(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length);
    DLL_EXPORT void uws_ws_iterate_topics(int ssl, uws_worker_t *worker, uws_websocket_t *ws, void (*callback)(const char *topic, size_t length));
    DLL_EXPORT bool uws_ws_publish(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length);
    DLL_EXPORT bool uws_ws_publish_with_options(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress);
    DLL_EXPORT unsigned int uws_ws_get_buffered_amount(int ssl, uws_worker_t *worker, uws_websocket_t *ws);
    DLL_EXPORT size_t uws_ws_get_remote_address(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char **dest);
    DLL_EXPORT size_t uws_ws_get_remote_address_as_text(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char **dest);

    //Response
    DLL_EXPORT void uws_res_end(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length, bool close_connection);
    DLL_EXPORT uws_try_end_result_t uws_res_try_end(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length, uintmax_t total_size, bool close_connection);
    DLL_EXPORT void uws_res_cork(int ssl, uws_worker_t *worker, uws_res_t *res, void(*callback)(uws_res_t *res));
    DLL_EXPORT void uws_res_pause(int ssl, uws_worker_t *worker, uws_res_t *res);
    DLL_EXPORT void uws_res_resume(int ssl, uws_worker_t *worker, uws_res_t *res);
    DLL_EXPORT void uws_res_write_continue(int ssl, uws_worker_t *worker, uws_res_t *res);
    DLL_EXPORT void uws_res_write_status(int ssl, uws_worker_t *worker, uws_res_t *res, const char *status, size_t length);
    DLL_EXPORT void uws_res_write_header(int ssl, uws_worker_t *worker, uws_res_t *res, const char *key, size_t key_length, const char *value, size_t value_length);

    DLL_EXPORT void uws_res_write_header_int(int ssl, uws_worker_t *worker, uws_res_t *res, const char *key, size_t key_length, uint64_t value);
    DLL_EXPORT void uws_res_end_without_body(int ssl, uws_worker_t *worker, uws_res_t *res, bool close_connection);
    DLL_EXPORT bool uws_res_write(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length);
    DLL_EXPORT uintmax_t uws_res_get_write_offset(int ssl, uws_worker_t *worker, uws_res_t *res);
    DLL_EXPORT void uws_res_override_write_offset(int ssl, uws_worker_t *worker, uws_res_t *res, uintmax_t offset);
    DLL_EXPORT bool uws_res_has_responded(int ssl, uws_worker_t *worker, uws_res_t *res);
    DLL_EXPORT void uws_res_on_writable(int ssl, uws_worker_t *worker, uws_res_t *res, bool (*handler)(uws_res_t *res, uintmax_t));
    DLL_EXPORT void uws_res_on_aborted(int ssl, uws_worker_t *worker, uws_res_t *res, void (*handler)(uws_res_t *res));
    DLL_EXPORT void uws_res_on_data(int ssl, uws_worker_t *worker, uws_res_t *res, void (*handler)(uws_res_t *res, const char *chunk, size_t chunk_length, bool is_end));
    DLL_EXPORT void uws_res_upgrade(int ssl, uws_worker_t *worker, uws_res_t *res, void *data, const char *sec_web_socket_key, size_t sec_web_socket_key_length, const char *sec_web_socket_protocol, size_t sec_web_socket_protocol_length, const char *sec_web_socket_extensions, size_t sec_web_socket_extensions_length, uws_socket_context_t *ws);
    DLL_EXPORT size_t uws_res_get_remote_address(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
    DLL_EXPORT size_t uws_res_get_remote_address_as_text(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
    DLL_EXPORT size_t uws_res_get_proxied_remote_address(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
    DLL_EXPORT size_t uws_res_get_proxied_remote_address_as_text(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
    DLL_EXPORT void *uws_res_get_native_handle(int ssl, uws_worker_t *worker, uws_res_t *res);

    //Request
    DLL_EXPORT bool uws_req_is_ancient(uws_worker_t *worker, uws_req_t *res);
    DLL_EXPORT bool uws_req_get_yield(uws_worker_t *worker, uws_req_t *res);
    DLL_EXPORT void uws_req_set_field(uws_worker_t *worker, uws_req_t *res, bool yield);
    DLL_EXPORT size_t uws_req_get_url(uws_worker_t *worker, uws_req_t *res, const char **dest);
    DLL_EXPORT size_t uws_req_get_full_url(uws_worker_t *worker, uws_req_t *res, const char **dest);
    DLL_EXPORT size_t uws_req_get_method(uws_worker_t *worker, uws_req_t *res, const char **dest);
    DLL_EXPORT size_t uws_req_get_case_sensitive_method(uws_worker_t *worker, uws_req_t *res, const char **dest);

    DLL_EXPORT size_t uws_req_get_header(uws_worker_t *worker, uws_req_t *res, const char *lower_case_header, size_t lower_case_header_length, const char **dest);
    DLL_EXPORT void uws_req_for_each_header(uws_worker_t *worker, uws_req_t *res, uws_get_headers_server_handler handler);
    DLL_EXPORT size_t uws_req_get_query(uws_worker_t *worker, uws_req_t *res, const char *key, size_t key_length, const char **dest);
    DLL_EXPORT size_t uws_req_get_parameter(uws_worker_t *worker, uws_req_t *res, unsigned short index, const char **dest);
#ifdef __cplusplus
}
#endif

#endif