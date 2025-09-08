#include "OffsetRegistry.h"
#include "Logger.h"

namespace hooklib {

    OffsetRegistry &OffsetRegistry::instance() {
        static OffsetRegistry inst;
        return inst;
    }

    void OffsetRegistry::set(const std::string &name, uintptr_t offset) {
        std::lock_guard <std::mutex> lock(mtx);
        map[name] = offset;
        LOGD("OffsetRegistry: %s (rel) = 0x%lx", name.c_str(), (unsigned long) offset);
    }

    std::optional <uintptr_t> OffsetRegistry::get(const std::string &name) const {
        std::lock_guard <std::mutex> lock(mtx);
        auto it = map.find(name);
        if (it == map.end()) return std::nullopt;
        return it->second;
    }

    void OffsetRegistry::setImageBase(uintptr_t base) {
        std::lock_guard <std::mutex> lock(mtx);
        imageBase_ = base;
        LOGD("OffsetRegistry: imageBase = %p", (void *) imageBase_);
    }

    std::optional <uintptr_t> OffsetRegistry::getImageBase() const {
        std::lock_guard <std::mutex> lock(mtx);
        if (imageBase_ == 0) return std::nullopt;
        return imageBase_;
    }

    std::optional <uintptr_t> OffsetRegistry::getAbsolute(const std::string &name) const {
        std::lock_guard <std::mutex> lock(mtx);
        auto it = map.find(name);
        if (it == map.end() || imageBase_ == 0) return std::nullopt;
        return imageBase_ + it->second;
    }

    void OffsetRegistry::clear() {
        std::lock_guard <std::mutex> lock(mtx);
        map.clear();
        imageBase_ = 0;
    }

} // namespace hooklib