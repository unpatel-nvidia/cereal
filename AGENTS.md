# Agents

## Cursor Cloud specific instructions

This is **cereal** — a header-only C++11 serialization library. There are no runtime services, databases, or containers.

### Build & Test

The build directory is at `/workspace/build`. After the update script runs, it is already built and ready.

```bash
cd /workspace/build
ctest --output-on-failure        # run all 40 unit tests
./sandbox/sandbox                # binary/XML/portable-binary demo
./sandbox/sandbox_json           # JSON archive demo
```

### Key Gotchas

- **Default `c++` points to clang++** which cannot link (`-lstdc++` not found). The update script forces `g++` via `update-alternatives`. If you hit linker errors about `-lstdc++`, re-run `sudo update-alternatives --set c++ /usr/bin/g++`.
- **GCC 13 `-Wdangling-reference`** fires false positives in `polymorphic_impl.hpp`. The build uses `-DWITH_WERROR=OFF` to avoid breaking on these warnings. Do not re-enable `-Werror` unless the upstream code is patched.
- **Portability tests** (32-bit) are skipped because `multilib` packages are not installed and `-m32` is unsupported.
- **Boost performance comparison** is skipped (`-DSKIP_PERFORMANCE_COMPARISON=ON`) because `libboost-serialization-dev` is not installed.
- **Documentation** build is disabled (`-DBUILD_DOC=OFF`) because Doxygen is not installed.

### Reconfiguring

If you need to reconfigure (e.g. to change the C++ standard):

```bash
rm -rf /workspace/build && mkdir /workspace/build && cd /workspace/build
cmake -DSKIP_PORTABILITY_TEST=ON -DSKIP_PERFORMANCE_COMPARISON=ON -DBUILD_DOC=OFF \
      -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DWITH_WERROR=OFF ..
make -j$(nproc)
```
