// #include <iostream>
// #include <vector>
// #include <string>
// import base64;
//
// int main() {
//     std::cout << "=== Base64 编码测试 ===" << std::endl;
//
//     std::vector<std::string> test_cases = {
//         "",
//         "Hello",
//         "Hello, World!",
//         "Base64 Encoding in C++",
//         "1234567890",
//         "Special chars: !@#$%^&*()",
//         "中文测试",
//         "Mixed: Hello 世界 123"
//     };
//
//     for (const auto& test : test_cases) {
//         std::string encoded = base64::encode(test);
//         std::string decoded = base64::decode(encoded);
//
//         std::cout << "\n原始数据: " << test << std::endl;
//         std::cout << "编码结果: " << encoded << std::endl;
//         std::cout << "解码结果: " << decoded << std::endl;
//         std::cout << "验证: " << (test == decoded ? "✓ 通过" : "✗ 失败") << std::endl;
//     }
//
//     std::cout << "\n=== URL Safe Base64 测试 ===" << std::endl;
//     std::string url_test = "Hello+World/Test";
//     std::string url_encoded = base64::encodeURI(url_test);
//     std::string url_decoded = base64::decode(url_encoded);
//
//     std::cout << "原始数据: " << url_test << std::endl;
//     std::cout << "URL编码: " << url_encoded << std::endl;
//     std::cout << "URL解码: " << url_decoded << std::endl;
//     std::cout << "验证: " << (url_test == url_decoded ? "✓ 通过" : "✗ 失败") << std::endl;
//
//     return 0;
// }
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <iomanip>
import base64;

std::string generate_random_string(size_t length) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()_+-=[]{}|;:',.<>?/~`";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(gen)];
    }
    return result;
}

struct BenchmarkResult {
    std::string name;
    size_t data_size;
    int iterations;
    double avg_time_ms;
    double throughput_mbps;
    bool passed;
};

BenchmarkResult benchmark(const std::string& name, size_t data_size, int iterations) {
    std::string test_data = generate_random_string(data_size);

    auto start = std::chrono::high_resolution_clock::now();

    std::string encoded;
    std::string decoded;

    for (int i = 0; i < iterations; ++i) {
        encoded = base64::encode(test_data);
        decoded = base64::decode(encoded);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    double avg_time = elapsed.count() / iterations;
    double throughput = (data_size * 2 * iterations) / (elapsed.count() / 1000.0) / (1024 * 1024);

    return {name, data_size, iterations, avg_time, throughput, test_data == decoded};
}

int main() {
    std::vector<size_t> data_sizes = {
        512, 1024, 2048, 4096, 8192,
        16384, 32768, 65536, 131072, 262144,
        524288, 1048576, 2097152, 5242880, 10485760
    };

    std::vector<BenchmarkResult> results;

    std::cout << "=== Base64 性能测试 ===" << std::endl;
    std::cout << std::endl;

    for (size_t size : data_sizes) {
        int iter = 1000;
        if (size > 100000) iter = 100;
        if (size > 1000000) iter = 100;

        auto result = benchmark("C++ base64", size, iter);
        results.push_back(result);

        std::string size_str;
        if (size >= 1048576)
            size_str = std::to_string(size / 1048576) + " MB";
        else if (size >= 1024)
            size_str = std::to_string(size / 1024) + " KB";
        else
            size_str = std::to_string(size) + " B";

        std::cout << std::left << std::setw(12) << result.name
                  << "数据大小: " << std::setw(10) << size_str
                  << "迭代: " << std::setw(6) << result.iterations
                  << "平均耗时: " << std::fixed << std::setprecision(3) << std::setw(10) << result.avg_time_ms << " ms"
                  << "吞吐量: " << std::setw(10) << result.throughput_mbps << " MB/s"
                  << "验证: " << (result.passed ? "PASS" : "FAIL")
                  << std::endl;
    }

    std::cout << "\n\n=== JSON 输出（用于图表）===" << std::endl;
    std::cout << "[" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        std::cout << "  {\"size\": " << r.data_size
                  << ", \"avg_time\": " << std::fixed << std::setprecision(3) << r.avg_time_ms
                  << ", \"throughput\": " << std::fixed << std::setprecision(2) << r.throughput_mbps
                  << ", \"passed\": " << (r.passed ? "true" : "false") << "}";
        if (i < results.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;

    return 0;
}

