// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 yuzu Emulator Project
// Copyright 2014 The Android Open Source Project
// Parts of this implementation were base on:
// https://cs.android.com/android/platform/superproject/+/android-5.1.1_r38:frameworks/native/include/gui/BufferQueueProducer.h

#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>

#include "common/common_funcs.h"
#include "core/hle/service/nvdrv/nvdata.h"
#include "core/hle/service/nvflinger/binder.h"
#include "core/hle/service/nvflinger/buffer_queue_defs.h"
#include "core/hle/service/nvflinger/buffer_slot.h"
#include "core/hle/service/nvflinger/graphic_buffer_producer.h"
#include "core/hle/service/nvflinger/pixel_format.h"
#include "core/hle/service/nvflinger/status.h"
#include "core/hle/service/nvflinger/window.h"

namespace Kernel {
class KernelCore;
class KEvent;
class KReadableEvent;
class KWritableEvent;
} // namespace Kernel

namespace Service::KernelHelpers {
class ServiceContext;
} // namespace Service::KernelHelpers

namespace Service::android {

class BufferQueueCore;
class IProducerListener;

class BufferQueueProducer final : public IBinder {
public:
    explicit BufferQueueProducer(Service::KernelHelpers::ServiceContext& service_context_,
                                 std::shared_ptr<BufferQueueCore> buffer_queue_core_);
    ~BufferQueueProducer();

    void Transact(Kernel::HLERequestContext& ctx, android::TransactionId code, u32 flags) override;

    Kernel::KReadableEvent& GetNativeHandle() override;

public:
    Status RequestBuffer(s32 slot, std::shared_ptr<GraphicBuffer>* buf);
    Status SetBufferCount(s32 buffer_count);
    Status DequeueBuffer(s32* out_slot, android::Fence* out_fence, bool async, u32 width,
                         u32 height, PixelFormat format, u32 usage);
    Status DetachBuffer(s32 slot);
    Status DetachNextBuffer(std::shared_ptr<GraphicBuffer>* out_buffer, Fence* out_fence);
    Status AttachBuffer(s32* outSlot, const std::shared_ptr<GraphicBuffer>& buffer);
    Status QueueBuffer(s32 slot, const QueueBufferInput& input, QueueBufferOutput* output);
    void CancelBuffer(s32 slot, const Fence& fence);
    Status Query(NativeWindow what, s32* out_value);
    Status Connect(const std::shared_ptr<IProducerListener>& listener, NativeWindowApi api,
                   bool producer_controlled_by_app, QueueBufferOutput* output);

    Status Disconnect(NativeWindowApi api);
    Status SetPreallocatedBuffer(s32 slot, const std::shared_ptr<GraphicBuffer>& buffer);

private:
    BufferQueueProducer(const BufferQueueProducer&) = delete;

    Status WaitForFreeSlotThenRelock(bool async, s32* found, Status* return_flags) const;

    Kernel::KEvent* buffer_wait_event{};
    Service::KernelHelpers::ServiceContext& service_context;

    std::shared_ptr<BufferQueueCore> core;
    BufferQueueDefs::SlotsType& slots;
    u32 sticky_transform{};
    std::mutex callback_mutex;
    s32 next_callback_ticket{};
    s32 current_callback_ticket{};
    std::condition_variable_any callback_condition;
};

} // namespace Service::android
