#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace catusse {

inline std::ostream& log() { return std::cout << "[INFO] "; }
inline std::ostream& err() { return std::cerr << "[ERRORE] "; }

template <typename T, typename Compare = std::less<T>>
class QuantumHeap {
    std::vector<T> nodes_;
    Compare cmp_;

public:
    explicit QuantumHeap(Compare cmp = Compare()) : cmp_(std::move(cmp)) {}

    void reserve(std::size_t n) { nodes_.reserve(n); }

    void push(T val) {
        nodes_.push_back(std::move(val));
        std::push_heap(nodes_.begin(), nodes_.end(), cmp_);
    }

    const T& top() const {
        if (nodes_.empty())
            throw std::out_of_range("catusse::QuantumHeap::top() su heap vuoto");
        return nodes_.front();
    }

    T pop() {
        if (nodes_.empty())
            throw std::out_of_range("catusse::QuantumHeap::pop() su heap vuoto");
        std::pop_heap(nodes_.begin(), nodes_.end(), cmp_);
        T val = std::move(nodes_.back());
        nodes_.pop_back();
        return val;
    }

    void clear() { nodes_.clear(); }
    bool empty() const noexcept { return nodes_.empty(); }
    std::size_t size() const noexcept { return nodes_.size(); }
};

std::string encodeRunLength(const std::string& input) {
    if (input.empty()) return "";
    std::ostringstream out;
    char cur = input[0];
    std::size_t count = 1;
    auto flush = [&]() {
        if (cur >= '0' && cur <= '9')
            throw std::invalid_argument("catusse::encodeRunLength: cifre non ammesse nell'input");
        out << count << cur;
    };
    for (std::size_t i = 1; i < input.size(); ++i) {
        if (input[i] == cur) {
            ++count;
        } else {
            flush();
            cur = input[i];
            count = 1;
        }
    }
    flush();
    return out.str();
}

std::string decodeRunLength(const std::string& encoded) {
    std::string out;
    std::size_t i = 0;
    while (i < encoded.size()) {
        if (!(encoded[i] >= '0' && encoded[i] <= '9'))
            throw std::invalid_argument("catusse::decodeRunLength: atteso un conteggio");
        std::size_t count = 0;
        while (i < encoded.size() && encoded[i] >= '0' && encoded[i] <= '9') {
            std::size_t digit = static_cast<std::size_t>(encoded[i] - '0');
            if (count > (SIZE_MAX - digit) / 10)
                throw std::overflow_error("catusse::decodeRunLength: conteggio troppo grande");
            count = count * 10 + digit;
            ++i;
        }
        if (i >= encoded.size())
            throw std::invalid_argument("catusse::decodeRunLength: carattere mancante");
        if (count == 0)
            throw std::invalid_argument("catusse::decodeRunLength: conteggio zero");
        if (count > out.max_size() - out.size())
            throw std::length_error("catusse::decodeRunLength: output troppo grande");
        out.append(count, encoded[i]);
        ++i;
    }
    return out;
}

std::vector<int> sieveOfEratosthenes(int limit) {
    if (limit < 2) return {};
    const std::size_t n = static_cast<std::size_t>(limit);
    std::vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (std::size_t i = 2; i * i <= n; ++i)
        if (is_prime[i])
            for (std::size_t j = i * i; j <= n; j += i) is_prime[j] = false;
    std::vector<int> primes;
    for (std::size_t i = 2; i <= n; ++i)
        if (is_prime[i]) primes.push_back(static_cast<int>(i));
    return primes;
}

struct Point {
    double x = 0.0, y = 0.0;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

double crossProduct(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

std::vector<Point> convexHull(std::vector<Point> points) {
    std::sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    points.erase(std::unique(points.begin(), points.end()), points.end());

    const std::size_t n = points.size();
    if (n < 3) return points;

    std::vector<Point> hull;
    hull.reserve(2 * n);
    for (std::size_t i = 0; i < n; ++i) {
        while (hull.size() >= 2 &&
               crossProduct(hull[hull.size() - 2], hull.back(), points[i]) <= 0)
            hull.pop_back();
        hull.push_back(points[i]);
    }
    const std::size_t lower = hull.size() + 1;
    for (std::size_t i = n - 1; i-- > 0;) {
        while (hull.size() >= lower &&
               crossProduct(hull[hull.size() - 2], hull.back(), points[i]) <= 0)
            hull.pop_back();
        hull.push_back(points[i]);
    }
    hull.pop_back();
    return hull;
}

double polygonArea(const std::vector<Point>& poly) {
    if (poly.size() < 3) return 0.0;
    double s = 0.0;
    for (std::size_t i = 0; i < poly.size(); ++i) {
        const Point& a = poly[i];
        const Point& b = poly[(i + 1) % poly.size()];
        s += a.x * b.y - b.x * a.y;
    }
    return std::abs(s) / 2.0;
}

template <typename Container>
void printAll(const char* label, const Container& c) {
    log() << label << ": ";
    for (const auto& v : c) std::cout << v << ' ';
    std::cout << '\n';
}

int g_failures = 0;

void check(bool ok, const char* name) {
    std::cout << "[TEST] " << (ok ? "OK    " : "FAIL  ") << name << '\n';
    if (!ok) ++g_failures;
}

bool throws_invalid(const std::function<void()>& fn) {
    try { fn(); } catch (const std::exception&) { return true; }
    return false;
}

int runSelfTests() {
    log() << "--- self-test ---\n";

    QuantumHeap<int> mx;
    for (int v : {5, 1, 9, 3}) mx.push(v);
    check(mx.top() == 9 && mx.pop() == 9 && mx.pop() == 5, "heap max ordine");
    QuantumHeap<int, std::greater<int>> mn;
    for (int v : {5, 1, 9, 3}) mn.push(v);
    check(mn.pop() == 1 && mn.pop() == 3, "heap min ordine");
    QuantumHeap<int> vuoto;
    check(throws_invalid([&] { vuoto.pop(); }), "heap vuoto lancia eccezione");

    check(encodeRunLength("aaabbbccddddeeee") == "3a3b2c4d4e", "RLE encode");
    check(decodeRunLength("3a3b2c4d4e") == "aaabbbccddddeeee", "RLE decode");
    check(decodeRunLength(encodeRunLength("zzzzzzzzzzzzy")) == "zzzzzzzzzzzzy", "RLE round-trip (conteggio >9)");
    check(encodeRunLength("").empty(), "RLE stringa vuota");
    check(throws_invalid([] { encodeRunLength("a1"); }), "RLE rifiuta cifre");
    check(throws_invalid([] { decodeRunLength("3"); }), "RLE decode malformato");

    check(sieveOfEratosthenes(100).size() == 25, "25 primi fino a 100");
    check(sieveOfEratosthenes(1).empty() && sieveOfEratosthenes(-5).empty(), "crivello limiti bassi");

    std::vector<Point> quad{{0, 0}, {4, 0}, {4, 4}, {0, 4}, {2, 2}, {2, 0}, {0, 0}};
    auto h = convexHull(quad);
    check(h.size() == 4 && polygonArea(h) == 16.0, "hull quadrato con interni/duplicati/collineari");
    check(convexHull({{1, 1}, {1, 1}}).size() == 1, "hull punti identici");

    log() << "--- fine self-test: " << (g_failures ? "ERRORI" : "tutto ok") << " ---\n";
    return g_failures;
}

}

int main(int argc, char** argv) {
    using namespace catusse;
    try {
        std::uint64_t seed = static_cast<std::uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        bool selftest = true;
        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];
            if (a == "--seed" && i + 1 < argc) {
                seed = std::stoull(argv[++i]);
            } else if (a == "--no-selftest") {
                selftest = false;
            } else {
                err() << "argomento sconosciuto: " << a << "\n";
                return 2;
            }
        }
        std::mt19937_64 rng(seed);
        log() << "avvio (seed=" << seed << ")\n";

        if (selftest && runSelfTests() != 0) return 1;

        QuantumHeap<int> maxHeap;
        maxHeap.reserve(10);
        std::uniform_int_distribution<int> dist(1, 1000);
        for (int i = 0; i < 10; ++i) maxHeap.push(dist(rng));
        log() << "Max-Heap drain: ";
        while (!maxHeap.empty()) std::cout << maxHeap.pop() << ' ';
        std::cout << '\n';

        const std::string sample = "aaabbbccddddeeee";
        const std::string enc = encodeRunLength(sample);
        log() << "RLE di \"" << sample << "\": " << enc
              << " -> decodifica: " << decodeRunLength(enc) << '\n';

        printAll("Primi fino a 100", sieveOfEratosthenes(100));

        std::vector<Point> pts;
        std::uniform_real_distribution<double> pdist(0.0, 100.0);
        for (int i = 0; i < 20; ++i) pts.push_back({pdist(rng), pdist(rng)});
        auto hull = convexHull(pts);
        log() << "Convex hull: " << hull.size() << " vertici, area "
              << polygonArea(hull) << '\n';

        std::vector<int> nums(20);
        std::iota(nums.begin(), nums.end(), 1);
        std::shuffle(nums.begin(), nums.end(), rng);
        printAll("Shuffled", nums);

        std::map<int, int> freq;
        for (int n : nums) freq[n % 5]++;
        log() << "Frequenza mod 5: ";
        for (const auto& [k, v] : freq) std::cout << k << ':' << v << ' ';
        std::cout << '\n';

        log() << "fine\n";
        return 0;
    } catch (const std::exception& e) {
        err() << e.what() << '\n';
        return 1;
    }
}
