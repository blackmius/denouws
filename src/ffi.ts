import { prepare } from "https://deno.land/x/plug@0.5.2/plug.ts";
import meta from "../deno.json" assert { type: "json" };


// struct us_socket_context_options_t {
//   const char *key_file_name;
//   const char *cert_file_name;
//   const char *passphrase;
//   const char *dh_params_file_name;
//   const char *ca_file_name;
//   const char *ssl_ciphers;
//   int ssl_prefer_low_memory_usage; /* Todo: rename to prefer_low_memory_usage and apply for TCP as well */
// };
const us_socket_context_options_t: Deno.NativeType[] = ["pointer", "pointer", "pointer", "pointer", "pointer", "pointer", "u8"];

// struct uws_app_listen_config_t {
//   int port;
//   const char *host;
//   int options;
// };
const uws_app_listen_config_t: Deno.NativeType[] = ["u16", "pointer", "u8"];

// struct uws_socket_behavior_t {
//     uws_compress_options_t compression;
//     /* Maximum message size we can receive */
//     unsigned int maxPayloadLength;
//     /* 2 minutes timeout is good */
//     unsigned short idleTimeout;
//     /* 64kb backpressure is probably good */
//     unsigned int maxBackpressure;
//     bool closeOnBackpressureLimit;
//     /* This one depends on kernel timeouts and is a bad default */
//     bool resetIdleTimeoutOnSend;
//     /* A good default, esp. for newcomers */
//     bool sendPingsAutomatically;
//     /* Maximum socket lifetime in seconds before forced closure (defaults to disabled) */
//     unsigned short maxLifetime;
//     uws_websocket_upgrade_handler upgrade;
//     uws_websocket_handler open;
//     uws_websocket_message_handler message;
//     uws_websocket_handler drain;
//     uws_websocket_ping_pong_handler ping;
//     uws_websocket_ping_pong_handler pong;
//     uws_websocket_close_handler close;
//     uws_websocket_subscription_handler subscription;
// };
const uws_socket_behavior_t: Deno.NativeType[] = ["u32", "u32", "u32", "u32", "u32", "u8", "u8", "u8", "u32", "pointer", "pointer", "pointer", "pointer", "pointer", "pointer", "pointer", "pointer"];

// struct uws_try_end_result_t {
//   bool ok;
//   bool has_responded;
// };
const uws_try_end_result_t: Deno.NativeType[] = ["u8", "u8"];

const symbols = {
  // uws_app_t *uws_create_app(int ssl, struct us_socket_context_options_t options);
  uws_create_app: { parameters: ["u8", { struct: us_socket_context_options_t }], result: "pointer" },
  // void uws_wait_app(uws_worker_t *worker);
  uws_wait_app: { parameters: ["pointer"], result: "void", nonblocking: true },
  // void uws_app_get(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_get: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_post(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_post: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_options(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_options: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_delete(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_delete: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_patch(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_patch: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_put(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_put: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_head(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_head: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_connect(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_connect: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_trace(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_trace: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_app_any(int ssl, uws_worker_t *worker, const char *pattern, uws_method_handler handler);
  uws_app_any: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },

  // void uws_app_listen(int ssl, uws_worker_t *worker, int port, uws_listen_handler handler);
  uws_app_listen: { parameters: ["u8", "pointer", "u16", "function"], result: "void" },
  // void uws_app_listen_with_config(int ssl, uws_worker_t *worker, uws_app_listen_config_t config, uws_listen_handler handler);
  uws_app_listen_with_config: { parameters: ["u8", "pointer", { struct: uws_app_listen_config_t }, "function"], result: "void" },
  // void uws_app_listen_domain(int ssl, uws_worker_t *worker, const char *domain, size_t domain_length, uws_listen_domain_handler handler);
  uws_app_listen_domain: { parameters: ["u8", "pointer", "pointer", "usize", "function"], result: "void" },
  // void uws_app_listen_domain_with_options(int ssl, uws_worker_t *worker, const char *domain,size_t domain_length, int options, uws_listen_domain_handler handler);
  uws_app_listen_domain_with_options: { parameters: ["u8", "pointer", "pointer", "usize", "u64", "function"], result: "void" },
  // void uws_app_domain(int ssl, uws_worker_t *worker, const char* server_name, size_t server_name_length);
  uws_app_domain: { parameters: ["u8", "pointer", "pointer", "usize"], result: "void" },
  
  // unsigned int uws_num_subscribers(int ssl, uws_worker_t *worker, const char *topic, size_t topic_length);
  uws_num_subscribers: { parameters: ["u8", "pointer", "pointer", "usize"], result: "u32" },
  // bool uws_publish(int ssl, uws_worker_t *worker, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress);
  uws_publish: { parameters: ["u8", "pointer", "pointer", "usize", "pointer", "usize", "u8", "u8"], result: "u32" },
  // void uws_remove_server_name(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length);
  uws_remove_server_name: { parameters: ["u8", "pointer", "pointer", "usize"], result: "function" },
  // void uws_add_server_name(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length);
  uws_add_server_name: { parameters: ["u8", "pointer", "pointer", "usize"], result: "function" },
  // void uws_add_server_name_with_options(int ssl, uws_worker_t *worker, const char *hostname_pattern, size_t hostname_pattern_length, struct us_socket_context_options_t options);
  uws_add_server_name_with_options: { parameters: ["u8", "pointer", "pointer", "usize", { struct: us_socket_context_options_t }], result: "function" },
  // void uws_missing_server_name(int ssl, uws_worker_t *worker, uws_missing_server_handler handler);
  uws_missing_server_name: { parameters: ["u8", "pointer", "function"], result: "function" },
  // void uws_filter(int ssl, uws_worker_t *worker, uws_filter_handler handler);
  uws_filter: { parameters: ["u8", "pointer", "function"], result: "function" },

  // WebSocket
  
  // void uws_ws(int ssl, uws_worker_t *worker, const char *pattern, uws_socket_behavior_t behavior);
  uws_ws: { parameters: ["u8", "pointer", "pointer", { struct: uws_socket_behavior_t }], result: "void" },
  // void uws_ws_close(int ssl, uws_worker_t *worker, uws_websocket_t *ws);
  uws_ws_close: { parameters: ["u8", "pointer", "pointer"], result: "void" },
  // uws_sendstatus_t uws_ws_send(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode);
  uws_ws_send: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "u8"], result: "u8" },
  // uws_sendstatus_t uws_ws_send_with_options(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode, bool compress, bool fin);
  uws_ws_send_with_options: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "u8", "u8", "u8"], result: "u8" },
  // void uws_ws_end(int ssl, uws_worker_t *worker, uws_websocket_t *ws, int code, const char *message, size_t length);
  uws_ws_end: { parameters: ["u8", "pointer",  "pointer", "u16", "pointer", "usize"], result: "void" },
  // void uws_ws_cork(int ssl, uws_worker_t *worker, uws_websocket_t *ws, void (*handler)());
  uws_ws_cork: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },

  // bool uws_ws_subscribe(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length);
  uws_ws_subscribe: { parameters: ["u8", "pointer", "pointer", "pointer", "usize"], result: "u8" },
  // bool uws_ws_unsubscribe(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length);
  uws_ws_unsubscribe: { parameters: ["u8", "pointer", "pointer", "pointer", "usize"], result: "u8" },
  // bool uws_ws_is_subscribed(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t length);
  uws_ws_is_subscribed: { parameters: ["u8", "pointer", "pointer", "pointer", "usize"], result: "u8" },
  // void uws_ws_iterate_topics(int ssl, uws_worker_t *worker, uws_websocket_t *ws, void (*callback)(const char *topic, size_t length));
  uws_ws_iterate_topics: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // bool uws_ws_publish(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length);
  uws_ws_publish: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "pointer", "usize"], result: "u8" },
  // bool uws_ws_publish_with_options(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress);
  uws_ws_publish_with_options: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "pointer", "usize", "u8", "u8"], result: "u8" },
  // unsigned int uws_ws_get_buffered_amount(int ssl, uws_worker_t *worker, uws_websocket_t *ws);
  uws_ws_get_buffered_amount: { parameters: ["u8", "pointer", "pointer"], result: "u32" },
  // size_t uws_ws_get_remote_address(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char **dest);
  uws_ws_get_remote_address: { parameters: ["u8", "pointer", "pointer", "pointer"], result: "usize" },
  // size_t uws_ws_get_remote_address_as_text(int ssl, uws_worker_t *worker, uws_websocket_t *ws, const char **dest);
  uws_ws_get_remote_address_as_text: { parameters: ["u8", "pointer", "pointer", "pointer"], result: "usize" },

  // size_t uws_res_get_proxied_remote_address(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
  uws_res_get_proxied_remote_address: { parameters: ["u8", "pointer", "pointer", "pointer"], result: "usize" },
  // size_t uws_res_get_proxied_remote_address_as_text(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
  uws_res_get_proxied_remote_address_as_text: { parameters: ["u8", "pointer", "pointer", "pointer"], result: "usize" },

  // Response
  
  // void uws_res_end(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length, bool close_connection);
  uws_res_end: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "u8"], result: "void" },
  // uws_try_end_result_t uws_res_try_end(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length, uintmax_t total_size, bool close_connection);
  uws_res_try_end: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "usize", "u8"], result: { struct: uws_try_end_result_t } },
  // void uws_res_cork(int ssl, uws_worker_t *worker, uws_res_t *res, void(*callback)(uws_res_t *res));
  uws_res_cork: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_res_pause(int ssl, uws_worker_t *worker, uws_res_t *res);
  uws_res_pause: { parameters: ["u8", "pointer", "pointer"], result: "void" },
  // void uws_res_resume(int ssl, uws_worker_t *worker, uws_res_t *res);
  uws_res_resume: { parameters: ["u8", "pointer", "pointer"], result: "void" },
  // void uws_res_write_continue(int ssl, uws_worker_t *worker, uws_res_t *res);
  uws_res_write_continue: { parameters: ["u8", "pointer", "pointer"], result: "void" },
  // void uws_res_write_status(int ssl, uws_worker_t *worker, uws_res_t *res, const char *status, size_t length);
  uws_res_write_status: { parameters: ["u8", "pointer", "pointer", "pointer", "usize"], result: "void" },
  // void uws_res_write_header(int ssl, uws_worker_t *worker, uws_res_t *res, const char *key, size_t key_length, const char *value, size_t value_length);
  uws_res_write_header: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "pointer", "usize"], result: "void" },
  
  // void uws_res_write_header_int(int ssl, uws_worker_t *worker, uws_res_t *res, const char *key, size_t key_length, uint64_t value);
  uws_res_write_header_int: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "u64"], result: "void" },
  // void uws_res_end_without_body(int ssl, uws_worker_t *worker, uws_res_t *res, bool close_connection);
  uws_res_end_without_body: { parameters: ["u8", "pointer", "pointer", "u8"], result: "void" },
  // bool uws_res_write(int ssl, uws_worker_t *worker, uws_res_t *res, const char *data, size_t length);
  uws_res_write: { parameters: ["u8", "pointer", "pointer", "pointer", "usize"], result: "u8" },
  // uintmax_t uws_res_get_write_offset(int ssl, uws_worker_t *worker, uws_res_t *res);
  uws_res_get_write_offset: { parameters: ["u8", "pointer", "pointer"], result: "usize" },
  // void uws_res_override_write_offset(int ssl, uws_worker_t *worker, uws_res_t *res, uintmax_t offset);
  uws_res_override_write_offset: { parameters: ["u8", "pointer", "pointer", "usize"], result: "void" },
  // bool uws_res_has_responded(int ssl, uws_worker_t *worker, uws_res_t *res);
  uws_res_has_responded: { parameters: ["u8", "pointer", "pointer"], result: "u8" },
  // void uws_res_on_writable(int ssl, uws_worker_t *worker, uws_res_t *res, bool (*handler)(uws_res_t *res, uintmax_t));
  uws_res_on_writable: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_res_on_aborted(int ssl, uws_worker_t *worker, uws_res_t *res, void (*handler)(uws_res_t *res));
  uws_res_on_aborted: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_res_on_data(int ssl, uws_worker_t *worker, uws_res_t *res, void (*handler)(uws_res_t *res, const char *chunk, size_t chunk_length, bool is_end));
  uws_res_on_data: { parameters: ["u8", "pointer", "pointer", "function"], result: "void" },
  // void uws_res_upgrade(int ssl, uws_worker_t *worker, uws_res_t *res, void *data, const char *sec_web_socket_key, size_t sec_web_socket_key_length, const char *sec_web_socket_protocol, size_t sec_web_socket_protocol_length, const char *sec_web_socket_extensions, size_t sec_web_socket_extensions_length, uws_socket_context_t *ws);
  uws_res_upgrade: { parameters: ["u8", "pointer", "pointer", "pointer", "pointer", "usize", "pointer", "usize", "pointer", "usize", "pointer"], result: "void" },
  // size_t uws_res_get_remote_address(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
  uws_res_get_remote_address: { parameters: ["u8", "pointer", "pointer", "pointer"], result: "usize" },
  // size_t uws_res_get_remote_address_as_text(int ssl, uws_worker_t *worker, uws_res_t *res, const char **dest);
  uws_res_get_remote_address_as_text: { parameters: ["u8", "pointer", "pointer", "pointer"], result: "usize" },

  //Request
  
  // bool uws_req_is_ancient(uws_worker_t *worker, uws_req_t *res);
  uws_req_is_ancient: { parameters: ["pointer", "pointer"], result: "u8" },
  // bool uws_req_get_yield(uws_worker_t *worker, uws_req_t *res);
  uws_req_get_yield: { parameters: ["pointer", "pointer"], result: "u8" },
  // void uws_req_set_field(uws_worker_t *worker, uws_req_t *res, bool yield);
  uws_req_set_field: { parameters: ["pointer", "pointer", "u8"], result: "void" },
  // size_t uws_req_get_url(uws_worker_t *worker, uws_req_t *res, const char **dest);
  uws_req_get_url: { parameters: ["pointer", "pointer", "pointer"], result: "usize" },
  // size_t uws_req_get_full_url(uws_worker_t *worker, uws_req_t *res, const char **dest);
  uws_req_get_full_url: { parameters: ["pointer", "pointer", "pointer"], result: "usize" },
  // size_t uws_req_get_method(uws_worker_t *worker, uws_req_t *res, const char **dest);
  uws_req_get_method: { parameters: ["pointer", "pointer", "pointer"], result: "usize" },
  // size_t uws_req_get_case_sensitive_method(uws_worker_t *worker, uws_req_t *res, const char **dest);
  uws_req_get_case_sensitive_method: { parameters: ["pointer", "pointer", "pointer"], result: "usize" },

  // size_t uws_req_get_header(uws_worker_t *worker, uws_req_t *res, const char *lower_case_header, size_t lower_case_header_length, const char **dest);
  uws_req_get_header: { parameters: ["pointer", "pointer", "pointer", "usize", "pointer"], result: "usize" },
  // void uws_req_for_each_header(uws_worker_t *worker, uws_req_t *res, uws_get_headers_server_handler handler);
  uws_req_for_each_header: { parameters: ["pointer", "pointer", "function"], result: "void" },
  // size_t uws_req_get_query(uws_worker_t *worker, uws_req_t *res, const char *key, size_t key_length, const char **dest);
  uws_req_get_query: { parameters: ["pointer", "pointer", "pointer", "usize", "pointer"], result: "usize" },
  // size_t uws_req_get_parameter(uws_worker_t *worker, uws_req_t *res, unsigned short index, const char **dest);
  uws_req_get_parameter: { parameters: ["pointer", "pointer", "u16", "pointer"], result: "usize" },
} as const;

const handlers_symbols = {
  // void (*uws_listen_handler)(struct us_listen_socket_t *listen_socket, uws_app_listen_config_t config);
  uws_listen_handler: { parameters: ["pointer", { struct: uws_app_listen_config_t }], result: "void" },
  // void (*uws_listen_domain_handler)(struct us_listen_socket_t *listen_socket, const char* domain, size_t domain_length, int options);
  uws_listen_domain_handler: { parameters: ["pointer", "pointer", "usize", "u64"], result: "void" },
  // void (*uws_method_handler)(uws_res_t *response, uws_req_t *request);
  uws_method_handler: { parameters: ["pointer", "pointer"], result: "void" },
  // void (*uws_filter_handler)(uws_res_t *response, int);
  uws_filter_handler: { parameters: ["pointer", "u64"], result: "void" },
  // void (*uws_missing_server_handler)(const char *hostname, size_t hostname_length);
  uws_missing_server_handler: { parameters: ["pointer", "usize"], result: "void" },
  // void (*uws_get_headers_server_handler)(const char *header_name, size_t header_name_size, const char *header_value, size_t header_value_size);
  uws_get_headers_server_handler: { parameters: ["pointer", "usize", "pointer", "usize"], result: "void" },
  
  // void (*handler)()
  uws_ws_cork_callback: { parameters: ["pointer"], result: "void" },

  // void (*callback)(const char *topic, size_t length)
  uws_ws_iterate_topics_handler: { parameters: ["pointer", "usize"], result: "void" },

  // void (*callback)(uws_res_t *res)
  uws_res_cork_callback: { parameters: ["pointer"], result: "void" },
  
  // bool (*handler)(uws_res_t *res, uintmax_t)
  uws_res_on_writable_handler: { parameters: ["pointer", "usize"], result: "u8" },

  // void (*handler)(uws_res_t *res)
  uws_res_on_aborted_handler: { parameters: ["pointer"], result: "void" },
  
  // void (*handler)(uws_res_t *res, const char *chunk, size_t chunk_length, bool is_end)
  uws_res_on_data_handler: { parameters: ["pointer", "pointer", "usize", "u8"], result: "void" },

  // void (*uws_websocket_handler)(uws_websocket_t *ws);
  uws_websocket_handler: { parameters: ["pointer"], result: "void" },
  // void (*uws_websocket_message_handler)(uws_websocket_t *ws, const char *message, size_t length, uws_opcode_t opcode);
  uws_websocket_message_handler: { parameters: ["pointer", "pointer", "usize", "u8"], result: "void" },
  // void (*uws_websocket_ping_pong_handler)(uws_websocket_t *ws, const char *message, size_t length);
  uws_websocket_ping_pong_handler: { parameters: ["pointer", "pointer", "usize"], result: "void" },
  // void (*uws_websocket_close_handler)(uws_websocket_t *ws, int code, const char *message, size_t length);
  uws_websocket_close_handler: { parameters: ["pointer", "u8", "pointer", "usize"], result: "void" },
  // void (*uws_websocket_upgrade_handler)(uws_res_t *response, uws_req_t *request, uws_socket_context_t *context);
  uws_websocket_upgrade_handler: { parameters: ["pointer", "pointer", "pointer"], result: "void" },
  // void (*uws_websocket_subscription_handler)(uws_websocket_t *ws, const char *topic_name, size_t topic_name_length, int new_number_of_subscriber, int old_number_of_subscriber);
  uws_websocket_subscription_handler: { parameters: ["pointer", "pointer", "usize", "u64", "u64"], result: "void" },
} as const;

interface ForeignLibraryCallbacksInterface {
  [name: string]: Deno.UnsafeCallbackDefinition;
}

type Handlers<S extends ForeignLibraryCallbacksInterface> = {
  [K in keyof S]: (cb: Deno.UnsafeCallback<S[K]>["callback"]) => Deno.UnsafeCallback<S[K]>;
}

const handlers = Object.fromEntries(Object.entries(handlers_symbols).map(([name, def]) => [name, cb => new Deno.UnsafeCallback(def, cb)])) as Handlers<typeof handlers_symbols>;

let lib: Deno.DynamicLibrary<typeof symbols>["symbols"];

try {
  const customPath = Deno.env.get("DENO_UWS_PATH");
  if (customPath) {
    lib = Deno.dlopen(customPath, symbols).symbols;
  } else {
    const url = `${meta.github}/releases/download/${meta.version}/`;
    lib = (await prepare({
      name: "uwebsockets",
      urls: {
        darwin: {
          aarch64: url + "libuwebsockets.dylib",
          x86_64: url + "libuwebsockets.dylib",
        },
        linux: url + "libuwebsockets.so",
        windows: url + "uwebsockets.dll",
      },
    }, symbols)).symbols;
  }
} catch (e) {
  if (e instanceof Deno.errors.PermissionDenied) {
    throw e;
  }
  
  const error = new Error("Failed to load uWebSockets Dynamic Library");
  error.cause = e;
  
  throw error;
}

export default Object.assign({}, lib, handlers);