#pragma once
#include <memory>
#include <mutex>
#include "PassManager.h"

class PipelineHolder {
public:
    void SetPipeline(std::shared_ptr<PassManager> newPipeline) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pipeline = newPipeline;
    }

    std::shared_ptr<PassManager> GetPipeline() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pipeline;
    }

    bool HasPipeline() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pipeline != nullptr;
    }

private:
    mutable std::mutex m_mutex;
    std::shared_ptr<PassManager> m_pipeline;
};
