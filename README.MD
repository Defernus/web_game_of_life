# Web browser cellar automate

## run

You need [emscripten](https://emscripten.org/) to compile it.

Then just build project with command...
```console
emcc --embed-file assets ./*.cpp -s USE_SDL=2 -s FULL_ES2=1 -s -o out.html
```
