# Basic complier flags, overridable by caller.
CFLAGS=-O2 -Wall

# Prevent using any stand library. We put some fake headers in the `stubs`
# directory just to convince our image library to compile.
NO_STANDARD_LIBS=-nostdlib -nostdinc -isystemstubs

# Configure WASM linkage:
# - Don't expect a main() function.
# - Have the module import a WebAssembly.Memory to use, rather than create its
#   own. This gives us more control over memory limits from the JS side.
WASM_LINKER_FLAGS=-Wl,--no-entry -Wl,--import-memory -Wl,--export-dynamic

resizer.wasm: main.c bootstrap.h
	clang $(CFLAGS) --target=wasm32-unknown-unknown-wasm \
	    $(NO_STANDARD_LIBS) $(WASM_LINKER_FLAGS) \
	    main.c -o resizer.wasm

clean:
	rm -f resizer.wasm

# If you want to use `make upload`, you'll need to store your API keys in this
# file. This is a simple text file with three lines like:
#    ACCOUNT_ID="<account id>"
#    API_EMAIL="<account email>"
#    API_KEY="<key>"
# This creates a script in your account called "wasm-demo". Only enterprise
# zones can have multiple, named scripts. If you do not have an enterprise
# account, you can upload through the single-script API endpoint with the
# same format, but as this will replace your existing script, we don't
# provide a shortcut for it here.
CF_KEYFILE=.cf-keys
upload: resizer.wasm
	(. $(CF_KEYFILE) && curl -X PUT -H "X-Auth-Email: $$API_EMAIL" -H "X-Auth-Key: $$API_KEY" \
	  "https://api.cloudflare.com/client/v4/accounts/$$ACCOUNT_ID/workers/scripts/wasm-demo" \
		-F metadata=@worker-metadata.json \
		-F script=@worker.js \
		-F wasm=@resizer.wasm)
