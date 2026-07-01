# base64-cpp

A high-performance Base64 encoding/decoding library implemented as a C++26 module.

## Features

- **C++20 Module** — uses `export module` for clean, modular imports
- **Standard & URL-safe** — supports both standard Base64 and URL-safe Base64 (`-` and `_`)
- **Compile-time tables** — encode/decode lookup tables are `constexpr`, zero runtime init cost
- **Cross-platform** — tested with MSVC and GCC/Clang; platform-specific `FORCE_INLINE` / branch hints
- **Benchmark included** — ships with a built-in benchmark tool for throughput measurement

## Requirements

- CMake >= 4.3
- C++26 compatible compiler:
  - MSVC 17.x+
  - GCC 15+ (with `-fmodules-ts` or full module support)
  - Clang 18+

## Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Usage

### As a module (recommended)

```cpp
import base64;

#include <iostream>

int main() {
    // Standard Base64
    std::string encoded = base64::encode("Hello, World!");
    std::string decoded = base64::decode(encoded);

    std::cout << encoded << std::endl; // SGVsbG8sIFdvcmxkIQ==
    std::cout << decoded << std::endl; // Hello, World!

    // URL-safe Base64
    std::string url_encoded = base64::encodeURI("data?param=a+b/c");
    std::cout << url_encoded << std::endl;

    return 0;
}
```

### API Reference

| Function | Description |
|---|---|
| `base64::encode(std::string_view input)` | Encode to standard Base64 |
| `base64::decode(std::string_view input)` | Decode standard or URL-safe Base64 |
| `base64::encodeURI(std::string_view input)` | Encode to URL-safe Base64 |
| `base64::encodeFromBytes(const std::string& bytes)` | Encode raw bytes |
| `base64::decodeToBytes(std::string_view input)` | Decode to raw bytes |

## Benchmark

The project includes a benchmark tool that measures encode+decode throughput across various data sizes (512 B to 10 MB).

```bash
./build/base64_cpp
```

## License

[MIT](LICENSE)
