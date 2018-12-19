# S2E_plugins
Custom C++ plugins for s2e symbolic execution engine (`GoogleCTFUnbreakable` taken from https://adrianherrera.github.io/).
To use them, clone the repo in 

`s2e/source/s2e/libs2eplugins/src`

add the plugin paths in 

`s2e/source/s2e/libs2eplugins/src/CMakeLists.txt` 

and recompile s2e (using s2e-env: `s2e build --rebuild-components libs2e`).
