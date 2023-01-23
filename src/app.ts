import ffi from './ffi.ts';

import { Struct } from "https://deno.land/x/struct@1.0.0/mod.ts";

export const encoder = new TextEncoder();
export function toCString(str: string): Uint8Array {
  return encoder.encode(str + "\0");
}

interface AppOptions {
  keyFileName: string
  certFileName: string
  passphrase: string
  dhParamsFileName: string
  caFileName: string
  sslCiphers: string
  sslPreferLowMemoryUsage: boolean
}

const {
  uws_create_app
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

export class App {
  #handle: Deno.PointerValue
  #ssl = 0;

  /** Unsafe Raw (pointer) to the uws_app object */
  get unsafeHandle(): Deno.PointerValue {
    return this.#handle;
  }

  constructor(options: AppOptions) {
    const optionsBuffer = getAppOptionsBuffer(options);
    this.#handle = uws_create_app(this.#ssl, optionsBuffer);
  }
}