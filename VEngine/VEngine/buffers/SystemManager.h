#pragma once

#include <assert.h>
#include <memory>
#include <unordered_map>

#include "MutableBufferOptions.hpp"
#include "System.h"

namespace buf
{

class SystemManager
{

    public:

        template <typename T>
        auto register_system() -> std::shared_ptr<T>;

        template <typename T>
        void set_signature(Signature signature);

        void id_destroyed(Id id);
        void id_signature_changed(Id id, Signature newSignature);

    private:

        std::unordered_map<const char*, Signature>               m_signatures{};
        std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
};

}; // namespace buf

#include "SystemManager.ipp"