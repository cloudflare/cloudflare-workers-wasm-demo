# Cloudflare Workers WebAssembly Demo

This repository implements image resizing that runs on Cloudflare's edge network using
[WebAssembly in a Cloudflare Worker](https://blog.cloudflare.com/webassembly-on-cloudflare-workers).

For this demo, we sought to minimize the dependencies of the WASM code, so that we could completely
avoid the need for a C standard library. A more practical approach to building WASM applications
would be to use [Emscripten](http://emscripten.org) to provide a working library. However, as of
this writing, Emscripten still needs some tweaks to correctly target Cloudflare Workers.

In order to minimize dependencies, we implement image encoding, decoding, and resizing using the
[stb library](https://github.com/nothings/stb).

## What's in this repository

* `main.c` -- The C code to be complied to WebAssembly.
* `stb` -- Git submodule pointing to [the stb library](https://github.com/nothings/stb), which
  provides single-file implementations of image-handling operations.
* `bootstrap.h` -- Minimal implementation of C library functions needed by stb.
* `stubs` -- Fake, empty C header files, just to satisfy the `#include`s in stb.
* `worker.js` -- Cloudflare Worker JavaScript that loads and runs the WASM.
* `worker-metadata.json` -- Used when uploading the worker via the API, to bind the worker to the
   WASM. Not needed when uploading via the UI.

## How to build

1. Install Clang >= 8.0.0
2. `make`

## How to deploy

Via the UI:

1. Create a script in the "Workers" tab of the Cloudflare dashboard.
2. Copy the contents of `worker.js` into the script editor.
3. Click on `Resources` at the top of the script editor.
4. Create a WebAssembly binding named `RESIZER_WASM`.
5. Upload `resizer.wasm` to this binding.
6. Save the script.
7. Create a route where your script will run.
8. Now any image file under that route will accept a `?width=` query parameter to
   downscale the image.

Via the API -- free/pro/biz (single-script) users -- THIS WILL REPLACE YOUR EXISTING SCRIPT:

    curl -X PUT -H "X-Auth-Email: $API_EMAIL" -H "X-Auth-Key: $API_KEY" \
	      "https://api.cloudflare.com/client/v4/zones/$ZONE_ID/workers/script" \
		    -F metadata=@worker-metadata.json \
		    -F script=@worker.js \
		    -F wasm=@resizer.wasm

Via the API -- enterprise (multi-script) users -- creates a script named "wasm-demo":

    curl -X PUT -H "X-Auth-Email: $API_EMAIL" -H "X-Auth-Key: $API_KEY" \
	      "https://api.cloudflare.com/client/v4/accounts/$ACCOUNT_ID/workers/scripts/wasm-demo" \
		    -F metadata=@worker-metadata.json \
		    -F script=@worker.js \
		    -F wasm=@resizer.wasm
