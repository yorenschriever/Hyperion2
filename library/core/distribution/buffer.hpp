#pragma once
#include <vector>

class Buffer : public std::vector<uint8_t>{
    public:
    Buffer(int size): std::vector<uint8_t>(size){
    }

    template<typename T>
    T* as() { return reinterpret_cast<T*>(data()); }

    template<typename T>
    const T* as() const { return reinterpret_cast<const T*>(data()); }

    template<typename T = uint8_t>
    void clear() {
        std::fill((T*)data(), (T*)data() + this->size() / sizeof(T), T());
    }
};