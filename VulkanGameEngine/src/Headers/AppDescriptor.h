#pragma once

#include "EngineDevice.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace app {

    class AppDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(EngineDevice& engineDevice) : device{ engineDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<AppDescriptorSetLayout> build() const;

        private:
            EngineDevice& device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        AppDescriptorSetLayout(
            EngineDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~AppDescriptorSetLayout();
        AppDescriptorSetLayout(const AppDescriptorSetLayout&) = delete;
        AppDescriptorSetLayout& operator=(const AppDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        EngineDevice& device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class AppDescriptorWriter;
    };

    class AppDescriptorPool {
    public:
        class Builder {
        public:
            Builder(EngineDevice& engineDevice) : device{ engineDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<AppDescriptorPool> build() const;

        private:
            EngineDevice& device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        AppDescriptorPool(
            EngineDevice& engineDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~AppDescriptorPool();
        AppDescriptorPool(const AppDescriptorPool&) = delete;
        AppDescriptorPool& operator=(const AppDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        EngineDevice& device;
        VkDescriptorPool descriptorPool;

        friend class AppDescriptorWriter;
    };

    class AppDescriptorWriter {
    public:
        AppDescriptorWriter(AppDescriptorSetLayout& setLayout, AppDescriptorPool& pool);

        AppDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        AppDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        AppDescriptorSetLayout& setLayout;
        AppDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} 