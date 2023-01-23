import { prepare } from "https://deno.land/x/plug@0.5.2/plug.ts";
import meta from "../deno.json" assert { type: "json" };

const symbols = {
  uws_create_app: { parameters: ["u8", "buffer"], result: "pointer" },
  uws_app_destroy: { parameters: ["u8", "pointer"], result: "void"},
  uws_app_get: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_post: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_options: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_delete: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_patch: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_put: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_head: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_connect: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_trace: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},
  uws_app_any: { parameters: ["u8", "pointer", "pointer", "function", "pointer"], result: "void"},

  uws_app_run: { parameters: ["u8", "pointer"], result: "void" },

  uws_app_listen: { parameters: ["u8", "pointer", "u16", "function", "pointer"], result: "void" },
  uws_app_listen_with_config: { parameters: ["u8", "pointer", "buffer", "function", "pointer"], result: "void" },
  uws_app_listen_domain: { parameters: ["u8", "pointer", "pointer", "usize", "function", "pointer"], result: "void" },
  uws_app_listen_domain_with_options: { parameters: ["u8", "pointer", "pointer", "usize", "u64", "function", "pointer"], result: "void" },
  uws_app_domain: { parameters: ["u8", "pointer", "pointer", "usize"], result: "void" },

  uws_constructor_failed: { parameters: ["u8", "pointer"], result: "u8" },
  uws_num_subscribers: { parameters: ["u8", "pointer", "pointer", "usize"], result: "u32" },

  uws_publish: { parameters: ["u8", "pointer", "pointer", "usize", "pointer", "usize", "u8", "u8"], result: "u32" },
  uws_get_native_handle: { parameters: ["u8", "pointer"], result: "function" },
  uws_remove_server_name: { parameters: ["u8", "pointer", "pointer", "usize"], result: "function" },
  uws_add_server_name: { parameters: ["u8", "pointer", "pointer", "usize"], result: "function" },
  uws_add_server_name_with_options: { parameters: ["u8", "pointer", "pointer", "usize", "buffer"], result: "function" },
  uws_missing_server_name: { parameters: ["u8", "pointer", "function", "pointer"], result: "function" },
  uws_filter: { parameters: ["u8", "pointer", "function", "pointer"], result: "function" },

  uws_ws: { parameters: ["u8", "pointer", "pointer", "buffer", "pointer"], result: "void" },
  uws_ws_get_user_data: { parameters: ["u8", "pointer"], result: "pointer" },
  uws_ws_close: { parameters: ["u8", "pointer"], result: "void" },
  uws_ws_send: { parameters: ["u8", "pointer", "pointer", "usize", "u8"], result: "u8" },
  uws_ws_send_with_options: { parameters: ["u8", "pointer", "pointer", "usize", "u8", "u8", "u8"], result: "u8" },
  uws_ws_send_fragment: { parameters: ["u8", "pointer", "pointer", "usize", "u8"], result: "u8" },
  uws_ws_send_first_fragment: { parameters: ["u8", "pointer", "pointer", "usize", "u8"], result: "u8" },
  uws_ws_send_first_fragment_with_opcode: { parameters: ["u8", "pointer", "pointer", "usize", "u8", "u8"], result: "u8" },
  uws_ws_send_last_fragment: { parameters: ["u8", "pointer", "pointer", "usize", "u8"], result: "u8" },
  uws_ws_end: { parameters: ["u8", "pointer", "u16", "pointer", "usize"], result: "void" },
  uws_ws_cork: { parameters: ["u8", "pointer", "function", "pointer"], result: "void" },

  uws_ws_subscribe: { parameters: ["u8", "pointer", "pointer", "usize"], result: "u8" },
  uws_ws_unsubscribe: { parameters: ["u8", "pointer", "pointer", "usize"], result: "u8" },
  uws_ws_is_subscribed: { parameters: ["u8", "pointer", "pointer", "usize"], result: "u8" },
  uws_ws_iterate_topics: { parameters: ["u8", "pointer", "function", "pointer"], result: "void" },
  uws_ws_publish: { parameters: ["u8", "pointer", "pointer", "usize", "pointer", "usize"], result: "u8" },
  uws_ws_publish_with_options: { parameters: ["u8", "pointer", "pointer", "usize", "pointer", "usize", "u8", "u8"], result: "u8" },
  uws_ws_get_buffered_amount: { parameters: ["u8", "pointer"], result: "u32" },
  uws_ws_get_remote_address: { parameters: ["u8", "pointer", "pointer"], result: "usize" },
  uws_ws_get_remote_address_as_text: { parameters: ["u8", "pointer", "pointer"], result: "usize" },
  
  uws_res_end: { parameters: ["u8", "pointer", "pointer", "usize", "u8"], result: "void" },
  uws_res_try_end: { parameters: ["u8", "pointer", "pointer", "usize", "usize", "u8"], result: "buffer" },
  uws_res_cork: { parameters: ["u8", "pointer", "function", "pointer"], result: "void" },
  uws_res_pause: { parameters: ["u8", "pointer"], result: "void" },
  uws_res_resume: { parameters: ["u8", "pointer"], result: "void" },
  uws_res_write_continue: { parameters: ["u8", "pointer"], result: "void" },
  uws_res_write_status: { parameters: ["u8", "pointer", "pointer", "usize"], result: "void" },
  uws_res_write_header: { parameters: ["u8", "pointer", "pointer", "usize", "pointer", "usize"], result: "void" },


  uws_res_write_header_int: { parameters: ["u8", "pointer", "pointer", "usize", "u64"], result: "void" },
  uws_res_end_without_body: { parameters: ["u8", "pointer", "u8"], result: "void" },
  uws_res_write: { parameters: ["u8", "pointer", "pointer", "usize"], result: "u8" },
  uws_res_get_write_offset: { parameters: ["u8", "pointer"], result: "usize" },
  uws_res_override_write_offset: { parameters: ["u8", "pointer", "usize"], result: "void" },
  uws_res_has_responded: { parameters: ["u8", "pointer"], result: "u8" },
  uws_res_on_writable: { parameters: ["u8", "pointer", "function", "pointer"], result: "void" },
  uws_res_on_aborted: { parameters: ["u8", "pointer", "function", "pointer"], result: "void" },
  uws_res_on_data: { parameters: ["u8", "pointer", "function", "pointer"], result: "void" },
  uws_res_upgrade: { parameters: ["u8", "pointer", "pointer", "pointer", "usize", "pointer", "usize", "pointer", "usize", "pointer"], result: "void" },
  uws_res_get_remote_address: { parameters: ["u8", "pointer", "pointer"], result: "usize" },
  uws_res_get_remote_address_as_text: { parameters: ["u8", "pointer", "pointer"], result: "usize" },
  uws_res_get_native_handle: { parameters: ["u8", "pointer"], result: "function" },

  uws_req_is_ancient: { parameters: ["pointer"], result: "u8" },
  uws_req_get_yield: { parameters: ["pointer"], result: "u8" },
  uws_req_set_field: { parameters: ["pointer", "u8"], result: "void" },
  uws_req_get_url: { parameters: ["pointer", "pointer"], result: "usize" },
  uws_req_get_full_url: { parameters: ["pointer", "pointer"], result: "usize" },
  uws_req_get_method: { parameters: ["pointer", "pointer"], result: "usize" },
  uws_req_get_case_sensitive_method: { parameters: ["pointer", "pointer"], result: "usize" },

  uws_req_get_header: { parameters: ["pointer", "pointer", "usize", "pointer"], result: "usize" },
  uws_req_for_each_header: { parameters: ["pointer", "function", "pointer"], result: "void" },
  uws_req_get_query: { parameters: ["pointer", "pointer", "usize", "pointer"], result: "usize" },
  uws_req_get_parameter: { parameters: ["pointer", "u16", "pointer"], result: "usize" },

  uws_get_loop: { parameters: [], result: "pointer" },
  uws_get_loop_with_native: { parameters: ["pointer"], result: "pointer" },

} as const;

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

export default lib;