// SPDX-FileCopyrightText: 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/device_memory_manager.h"

namespace VideoCore {
class RasterizerInterface;
}

namespace Tegra {

struct MaxwellDeviceTraits {
    static constexpr bool supports_pinning = true;
    static constexpr size_t device_virtual_bits = 34;
    using DeviceInterface = typename VideoCore::RasterizerInterface;
};

using MaxwellDeviceMemoryManager = Core::DeviceMemoryManager<MaxwellDeviceTraits>;

} // namespace Tegra