#include <random>


namespace nrand {
    struct nrand {
        protected: static unsigned int randint(const unsigned int a, const unsigned int b) {
            std::random_device seed;
            std::mt19937_64 generator(seed());
            std::uniform_int_distribution<> range(a, b);
            return range(generator);
        }
    };
}
