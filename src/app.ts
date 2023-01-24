import ffi from './ffi.ts';

import { Struct } from "https://deno.land/x/struct@1.0.0/mod.ts";

export const encoder = new TextEncoder();
export function toCString(str: string): Uint8Array {
  return encoder.encode(str + "\0");
}

/** Native type representing a raw uSockets struct us_listen_socket_t.
 * Careful with this one, it is entirely unchecked and native so invalid usage will blow up.
 */
type us_listen_socket = Deno.PointerValue;

enum ListenOptions {
  LIBUS_LISTEN_DEFAULT = 0,
  LIBUS_LISTEN_EXCLUSIVE_PORT = 1
}

interface ListenConfig {
  port: number
  host: string
  options: ListenOptions
}

/** Options used when constructing an app. Especially for SSLApp.
 * These are options passed directly to uSockets, C layer.
 */
interface AppOptions {
  key_file_name: string
  cert_file_name: string
  passphrase: string
  dh_params_file_name: string
  ca_file_name: string
  ssl_ciphers: string
  ssl_prefer_low_memory_usage: boolean
}

const {
  uws_create_app,

  uws_app_run,

  uws_app_listen,
  uws_app_listen_with_config,

  uws_app_get,
  uws_app_post,
  uws_app_options,
  uws_app_delete,
  uws_app_patch,
  uws_app_put,
  uws_app_head,
  uws_app_connect,
  uws_app_trace,
  uws_app_any,

  uws_ws_publish_with_options,

  uws_res_end,

  uws_listen_handler,
  uws_method_handler
} = ffi;

function getAppOptionsBuffer(options: AppOptions): Uint8Array {
  return Struct.pack(
    ">llllll?",
    [
      Deno.UnsafePointer.of(toCString(options.keyFileName)),
      Deno.UnsafePointer.of(toCString(options.certFileName)),
      Deno.UnsafePointer.of(toCString(options.passphrase)),
      Deno.UnsafePointer.of(toCString(options.dhParamsFileName)),
      Deno.UnsafePointer.of(toCString(options.caFileName)),
      Deno.UnsafePointer.of(toCString(options.sslCiphers)),
      !!options.sslPreferLowMemoryUsage
    ]
  )
}

function getListenConfigBuffer(config: ListenConfig) {
  return Struct.pack(
    ">ll?",
    [
      config.port ?? 0,
      Deno.UnsafePointer.of(toCString(options.host)),
      config.options ?? 0
    ]
  )
}

class WebSocket<UserData> {
  #handler: Deno.PointerValue;

  constructor(handler: Deno.PointerValue) {
    this.#handler = handler;
  }

  /** Sends a message. Returns 1 for success, 2 for dropped due to backpressure limit, and 0 for built up backpressure that will drain over time. You can check backpressure before or after sending by calling getBufferedAmount().
   *
   * Make sure you properly understand the concept of backpressure. Check the backpressure example file.
   */
  send(message: string, isBinary?: boolean, compress?: boolean) : number;

  /** Returns the bytes buffered in backpressure. This is similar to the bufferedAmount property in the browser counterpart.
   * Check backpressure example.
   */
  getBufferedAmount() : number;

  /** Gracefully closes this WebSocket. Immediately calls the close handler.
   * A WebSocket close message is sent with code and shortMessage.
   */
  end(code?: number, shortMessage?: string) : void;

  /** Forcefully closes this WebSocket. Immediately calls the close handler.
   * No WebSocket close message is sent.
   */
  close() : void;

  /** Sends a ping control message. Returns sendStatus similar to WebSocket.send (regarding backpressure). This helper function correlates to WebSocket::send(message, uWS::OpCode::PING, ...) in C++. */
  ping(message?: string) : number;

  /** Subscribe to a topic. */
  subscribe(topic: string) : boolean;

  /** Unsubscribe from a topic. Returns true on success, if the WebSocket was subscribed. */
  unsubscribe(topic: string) : boolean;

  /** Returns whether this websocket is subscribed to topic. */
  isSubscribed(topic: string) : boolean;

  /** Returns a list of topics this websocket is subscribed to. */
  getTopics() : string[];

  /** Publish a message under topic. Backpressure is managed according to maxBackpressure, closeOnBackpressureLimit settings.
   * Order is guaranteed since v20.
  */
  publish(topic: string, message: string, isBinary?: boolean, compress?: boolean) : boolean;

  /** See HttpResponse.cork. Takes a function in which the socket is corked (packing many sends into one single syscall/SSL block) */
  cork(cb: () => void) : WebSocket<UserData>;

  /** Returns the remote IP address. Note that the returned IP is binary, not text.
   *
   * IPv4 is 4 byte long and can be converted to text by printing every byte as a digit between 0 and 255.
   * IPv6 is 16 byte long and can be converted to text in similar ways, but you typically print digits in HEX.
   *
   * See getRemoteAddressAsText() for a text version.
   */
  getRemoteAddress() : ArrayBuffer;

  /** Returns the remote IP address as text. See string. */
  getRemoteAddressAsText() : ArrayBuffer;

  /** Returns the UserData object. */
  getUserData() : UserData;
}

/** An HttpResponse is valid until either onAborted callback or any of the .end/.tryEnd calls succeed. You may attach user data to this object. */
class HttpResponse {
  /** Writes the HTTP status message such as "200 OK".
   * This has to be called first in any response, otherwise
   * it will be called automatically with "200 OK".
   *
   * If you want to send custom headers in a WebSocket
   * upgrade response, you have to call writeStatus with
   * "101 Switching Protocols" before you call writeHeader,
   * otherwise your first call to writeHeader will call
   * writeStatus with "200 OK" and the upgrade will fail.
   *
   * As you can imagine, we format outgoing responses in a linear
   * buffer, not in a hash table. You can read about this in
   * the user manual under "corking".
  */

  #ssl: number;
  #handler: Deno.PointerValue;

  constructor(ssl: number, handler: Deno.PointerValue) {
    this.#ssl = ssl;
    this.#handler = handler;
  }

  /** Pause http body streaming (throttle) */
  pause() : void;

  /** Resume http body streaming (unthrottle) */
  resume() : void;

  writeStatus(status: string) : HttpResponse;
  /** Writes key and value to HTTP response.
   * See writeStatus and corking.
  */
  writeHeader(key: string, value: string) : HttpResponse;
  /** Enters or continues chunked encoding mode. Writes part of the response. End with zero length write. Returns true if no backpressure was added. */
  write(chunk: string) : boolean;
  /** Ends this response by copying the contents of body. */
  end(body?: string, closeConnection?: boolean) : HttpResponse {
    const data = encoder.encode(body);
    uws_res_end(this.#ssl, this.#handler, Deno.UnsafePointer.of(data), data.length, +!!closeConnection);
    return this;
  }
  /** Ends this response without a body. */
  endWithoutBody(reportedContentLength?: number, closeConnection?: boolean) : HttpResponse;
  /** Ends this response, or tries to, by streaming appropriately sized chunks of body. Use in conjunction with onWritable. Returns tuple [ok, hasResponded].*/
  tryEnd(fullBodyOrChunk: string, totalSize: number) : [boolean, boolean];

  /** Immediately force closes the connection. Any onAborted callback will run. */
  close() : HttpResponse;

  /** Returns the global byte write offset for this response. Use with onWritable. */
  getWriteOffset() : number;

  /** Registers a handler for writable events. Continue failed write attempts in here.
   * You MUST return true for success, false for failure.
   * Writing nothing is always success, so by default you must return true.
   */
  onWritable(handler: (offset: number) => boolean) : HttpResponse;

  /** Every HttpResponse MUST have an attached abort handler IF you do not respond
   * to it immediately inside of the callback. Returning from an Http request handler
   * without attaching (by calling onAborted) an abort handler is ill-use and will terminate.
   * When this event emits, the response has been aborted and may not be used. */
  onAborted(handler: () => void) : HttpResponse;

  /** Handler for reading data from POST and such requests. You MUST copy the data of chunk if isLast is not true. We Neuter ArrayBuffers on return, making it zero length.*/
  onData(handler: (chunk: ArrayBuffer, isLast: boolean) => void) : HttpResponse;

  /** Returns the remote IP address in binary format (4 or 16 bytes). */
  getRemoteAddress() : ArrayBuffer;

  /** Returns the remote IP address as text. */
  getRemoteAddressAsText() : ArrayBuffer;

  /** Returns the remote IP address in binary format (4 or 16 bytes), as reported by the PROXY Protocol v2 compatible proxy. */
  getProxiedRemoteAddress() : ArrayBuffer;

  /** Returns the remote IP address as text, as reported by the PROXY Protocol v2 compatible proxy. */
  getProxiedRemoteAddressAsText() : ArrayBuffer;

  /** Corking a response is a performance improvement in both CPU and network, as you ready the IO system for writing multiple chunks at once.
   * By default, you're corked in the immediately executing top portion of the route handler. In all other cases, such as when returning from
   * await, or when being called back from an async database request or anything that isn't directly executing in the route handler, you'll want
   * to cork before calling writeStatus, writeHeader or just write. Corking takes a callback in which you execute the writeHeader, writeStatus and
   * such calls, in one atomic IO operation. This is important, not only for TCP but definitely for TLS where each write would otherwise result
   * in one TLS block being sent off, each with one send syscall.
   *
   * Example usage:
   *
   * res.cork(() => {
   *   res.writeStatus("200 OK").writeHeader("Some", "Value").write("Hello world!");
   * });
   */
  cork(cb: () => void) : HttpResponse;

  /** Upgrades a HttpResponse to a WebSocket. See UpgradeAsync, UpgradeSync example files. */
  upgrade<UserData>(userData : UserData, secWebSocketKey: string, secWebSocketProtocol: string, secWebSocketExtensions: string, context: us_socket_context_t) : void;

  /** Arbitrary user data may be attached to this object */
  [key: string]: any;
}

/** An HttpRequest is stack allocated and only accessible during the callback invocation. */
class HttpRequest {
  #handler: Deno.PointerValue;

  constructor(handler: Deno.PointerValue) {
    this.#handler = handler;
  }

  /** Returns the lowercased header value or empty string. */
  getHeader(lowerCaseKey: string) : string;
  /** Returns the parsed parameter at index. Corresponds to route. */
  getParameter(index: number) : string;
  /** Returns the URL including initial /slash */
  getUrl() : string;
  /** Returns the lowercased HTTP method, useful for "any" routes. */
  getMethod() : string;
  /** Returns the HTTP method as-is. */
  getCaseSensitiveMethod() : string;
  /** Returns the raw querystring (the part of URL after ? sign) or empty string. */
  getQuery() : string;
  /** Returns a decoded query parameter value or empty string. */
  getQuery(key: string) : string;
  /** Loops over all headers. */
  forEach(cb: (key: string, value: string) => void) : void;
  /** Setting yield to true is to say that this route handler did not handle the route, causing the router to continue looking for a matching route handler, or fail. */
  setYield(yield: boolean) : HttpRequest;
}

/** A structure holding settings and handlers for a WebSocket URL route handler. */
export interface WebSocketBehavior<UserData> {
  /** Maximum length of received message. If a client tries to send you a message larger than this, the connection is immediately closed. Defaults to 16 * 1024. */
  maxPayloadLength?: number;
  /** Whether or not we should automatically close the socket when a message is dropped due to backpressure. Defaults to false. */
  closeOnBackpressureLimit?: number;
  /** Maximum number of minutes a WebSocket may be connected before being closed by the server. 0 disables the feature. */
  maxLifetime?: number;
  /** Maximum amount of seconds that may pass without sending or getting a message. Connection is closed if this timeout passes. Resolution (granularity) for timeouts are typically 4 seconds, rounded to closest.
   * Disable by using 0. Defaults to 120.
   */
  idleTimeout?: number;
  /** What permessage-deflate compression to use. uWS.DISABLED, uWS.SHARED_COMPRESSOR or any of the uWS.DEDICATED_COMPRESSOR_xxxKB. Defaults to uWS.DISABLED. */
  compression?: CompressOptions;
  /** Maximum length of allowed backpressure per socket when publishing or sending messages. Slow receivers with too high backpressure will be skipped until they catch up or timeout. Defaults to 64 * 1024. */
  maxBackpressure?: number;
  /** Whether or not we should automatically send pings to uphold a stable connection given whatever idleTimeout. */
  sendPingsAutomatically?: boolean;
  /** Upgrade handler used to intercept HTTP upgrade requests and potentially upgrade to WebSocket.
   * See UpgradeAsync and UpgradeSync example files.
   */
  upgrade?: (res: HttpResponse, req: HttpRequest, context: us_socket_context_t) => void;
  /** Handler for new WebSocket connection. WebSocket is valid from open to close, no errors. */
  open?: (ws: WebSocket<UserData>) => void;
  /** Handler for a WebSocket message. Messages are given as ArrayBuffer no matter if they are binary or not. Given ArrayBuffer is valid during the lifetime of this callback (until first await or return) and will be neutered. */
  message?: (ws: WebSocket<UserData>, message: ArrayBuffer, isBinary: boolean) => void;
  /** Handler for when WebSocket backpressure drains. Check ws.getBufferedAmount(). Use this to guide / drive your backpressure throttling. */
  drain?: (ws: WebSocket<UserData>) => void;
  /** Handler for close event, no matter if error, timeout or graceful close. You may not use WebSocket after this event. Do not send on this WebSocket from within here, it is closed. */
  close?: (ws: WebSocket<UserData>, code: number, message: ArrayBuffer) => void;
  /** Handler for received ping control message. You do not need to handle this, pong messages are automatically sent as per the standard. */
  ping?: (ws: WebSocket<UserData>, message: ArrayBuffer) => void;
  /** Handler for received pong control message. */
  pong?: (ws: WebSocket<UserData>, message: ArrayBuffer) => void;
  /** Handler for subscription changes. */
  subscription?: (ws: WebSocket<UserData>, topic: ArrayBuffer, newCount: number, oldCount: number) => void;
}

/** TemplatedApp is either an SSL or non-SSL app. See App for more info, read user manual. */
class TemplatedApp {
  #handle: Deno.PointerValue
  #ssl = 0;

  /** Unsafe Raw (pointer) to the uws_app object */
  get unsafeHandle(): Deno.PointerValue {
    return this.#handle;
  }

  constructor(ssl: number, options?: AppOptions) {
    this.#ssl = ssl;
    if (this.#ssl) {
      const optionsBuffer = getAppOptionsBuffer(options!);
      this.#handle = uws_create_app(this.#ssl, optionsBuffer);
    } else {
      this.#handle = uws_create_app(this.#ssl, null);
    }
  }

  /** Listens to hostname & port. Callback hands either false or a listen socket. */
  listen(host: string, port: number, cb: (listenSocket: us_listen_socket) => void): TemplatedApp;
  /** Listens to port. Callback hands either false or a listen socket. */
  listen(port: number, cb: (listenSocket: us_listen_socket | false) => void): TemplatedApp;
  /** Listens to port and sets Listen Options. Callback hands either false or a listen socket. */
  listen(port: number, options: ListenOptions, cb: (listenSocket: us_listen_socket | false) => void): TemplatedApp;

  listen(): TemplatedApp {
    if (arguments.length === 2) {
      const [port, cb] = arguments;
      const listen_handler = uws_listen_handler((listen_socket: us_listen_socket) => cb(listen_socket));
      uws_app_listen(this.#ssl, this.#handle, port, listen_handler.pointer, null);
    }

    if (arguments.length === 3) {
      let [host, port, cb] = arguments;
      const config: ListenConfig = {};
      if (typeof host === 'number') {
        config.options = port;
        config.port = host;
      } else {
        config.host = host;
        config.port = port;
      }
      const listen_handler = uws_listen_handler((listen_socket: us_listen_socket) => cb(listen_socket));
      const configBuffer = getListenConfigBuffer(config);
      uws_app_listen_with_config(this.#ssl, this.#handle, configBuffer, listen_handler.pointer, null);
    }

    uws_app_run(this.#ssl, this.#handle);

    return this;
  }

  #generateHTTPHandler(
    method: typeof uws_app_any, // all http handler methods has same interface
    pattern: string,
    handler: (res: HttpResponse, req: HttpRequest) => void
  ) {
    const _handler = uws_method_handler(
      (res: Deno.PointerValue, req: Deno.PointerValue) => handler(new HttpResponse(this.#ssl, res), new HttpRequest(req))
    );
    method(this.#ssl, this.#handle, Deno.UnsafePointer.of(toCString(pattern)), _handler.pointer, null);
    return this;
  }

  /** Registers an HTTP GET handler matching specified URL pattern. */
  get(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_get, pattern, handler);
  }
  /** Registers an HTTP POST handler matching specified URL pattern. */
  post(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_post, pattern, handler);
  }
  /** Registers an HTTP OPTIONS handler matching specified URL pattern. */
  options(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_options, pattern, handler);
  }
  /** Registers an HTTP DELETE handler matching specified URL pattern. */
  del(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_delete, pattern, handler);
  }
  /** Registers an HTTP PATCH handler matching specified URL pattern. */
  patch(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_patch, pattern, handler);
  }
  /** Registers an HTTP PUT handler matching specified URL pattern. */
  put(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_put, pattern, handler);
  }
  /** Registers an HTTP HEAD handler matching specified URL pattern. */
  head(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_head, pattern, handler);
  }
  /** Registers an HTTP CONNECT handler matching specified URL pattern. */
  connect(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_connect, pattern, handler);
  }
  /** Registers an HTTP TRACE handler matching specified URL pattern. */
  trace(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_trace, pattern, handler);
  }
  /** Registers an HTTP handler matching specified URL pattern on any HTTP method. */
  any(pattern: string, handler: (res: HttpResponse, req: HttpRequest) => void) : TemplatedApp {
    return this.#generateHTTPHandler(uws_app_any, pattern, handler);
  }

  /** Registers a handler matching specified URL pattern where WebSocket upgrade requests are caught. */
  ws<UserData>(pattern: string, behavior: WebSocketBehavior<UserData>) : TemplatedApp;
  /** Publishes a message under topic, for all WebSockets under this app. See WebSocket.publish. */
  publish(topic: string, message: string, isBinary?: boolean, compress?: boolean) : boolean {
    int ssl, uws_websocket_t *ws, const char *topic, size_t topic_length, const char *message, size_t message_length, uws_opcode_t opcode, bool compress
    uws_ws_publish_with_options(this.#ssl)
  }
  /** Returns number of subscribers for this topic. */
  numSubscribers(topic: string) : number;
  /** Adds a server name. */
  addServerName(hostname: string, options: AppOptions): TemplatedApp;
  /** Removes a server name. */
  removeServerName(hostname: string): TemplatedApp;
  /** Registers a synchronous callback on missing server names. See /examples/ServerName.js. */
  missingServerName(cb: (hostname: string) => void): TemplatedApp;
}

export function App(options?: AppOptions) {
  return new TemplatedApp(0, options);
}
export function SSLApp(options: AppOptions) {
  return new TemplatedApp(1, options);
}