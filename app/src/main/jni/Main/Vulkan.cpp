#include "Vulkan.h"
#include "Menu.h"

#include <android/log.h>
#include <jni.h>

#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#include <mutex>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_android.h>
#include <vulkan/vulkan_core.h>

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_android.h"
#include "ImGui/backends/imgui_impl_vulkan.h"
#include "fonts/Roboto-Regular.h"

#include "dobby.h"
#include "xdl.h"

#define DEBUG_MENU 0
#define DEBUG_LOG 0
#define DEBUG_LOG_HOOK 0
#define DEBUG_LOG_ERR 1
#define DEBUG_LOG_CALL 0

#define LOG_TAG "ImGuiVulkanHook"

#if DEBUG_LOG
#  define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#  define LOGD(...)
#endif
#if DEBUG_LOG_HOOK
#  define LOG_HOOK(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#  define LOG_HOOK(...)
#endif
#if DEBUG_LOG_ERR
#  define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else 
#  define LOGE(...)
#endif
#if DEBUG_LOG_CALL
#  define LOG_CALL(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[Called]: "#__VA_ARGS__)
#else
#  define LOG_CALL(...)
#endif


/* --- Global vars --- */
VkInstance g_Instance = VK_NULL_HANDLE;
VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
VkDevice g_Device = VK_NULL_HANDLE;
VkQueue g_Queue = VK_NULL_HANDLE;
VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
VkCommandPool g_CommandPool = VK_NULL_HANDLE;
VkRenderPass g_RenderPass = VK_NULL_HANDLE;
VkExtent2D g_ScreenExtent = {0, 0};
VkFormat g_SurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;

std::vector<VkFramebuffer> g_Framebuffers;

/* Sync */
std::mutex g_StateMutex;
bool g_ImGuiNewFrame = false;
uint32_t g_ImageIndex = 0;
uint32_t g_MinImageCount = 2;

/* --- original functions --- */
PFN_vkCreateInstance vkCreateInstanceOrigin = nullptr;
PFN_vkCreateDevice vkCreateDeviceOrigin = nullptr;
PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHROrigin = nullptr;
PFN_vkCreateFramebuffer vkCreateFramebufferOrigin = nullptr;
PFN_vkDestroyFramebuffer vkDestroyFramebufferOrigin = nullptr;
PFN_vkQueueSubmit vkQueueSubmitOrigin = nullptr;
PFN_vkAcquireNextImageKHR vkAcquireNextImageKHROrigin = nullptr;

PFN_vkVoidFunction (*vkGetDeviceProcAddrOrigin)(VkDevice device, const char* pName) = nullptr;
PFN_vkVoidFunction (*vkGetInstanceProcAddrOrigin)(VkInstance inst, const char* pName) = nullptr;

/* --- flags --- */
bool g_ImGuiInitialized = false;
bool g_InitInProgress = false;
static bool g_InSubmit = false;
#if DEBUG_LOG
static int g_FrameCounter = 0;
#endif

struct RenderContext {
    VkCommandBuffer commandBuffer;
    VkFence fence;
};
static RenderContext g_RenderContexts[3];
static uint32_t g_CurrentContextIndex = 0;

/* --- helper functions --- */
uint32_t findGraphicsQueueFamily() {
    uint32_t count = 0;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties fn;
    fn = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)
        vkGetInstanceProcAddrOrigin(g_Instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    fn(g_PhysicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    fn(g_PhysicalDevice, &count, families.data());
    for (uint32_t i = 0; i < count; i++) {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) return i;
    }
    return 0;
}

VkCommandBuffer createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = g_CommandPool;
    allocInfo.commandBufferCount = 1;
    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(g_Device, &allocInfo, &cmd);
    return cmd;
}

void initRenderContexts() {
    for (int i = 0; i < 3; i++) {
        g_RenderContexts[i].commandBuffer = createCommandBuffer();
        VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT};
        vkCreateFence(g_Device, &fenceInfo, nullptr, &g_RenderContexts[i].fence);
    }
}

bool createDescriptorPool() {
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
    };
    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    return vkCreateDescriptorPool(g_Device, &pool_info, nullptr, &g_DescriptorPool) == VK_SUCCESS;
}

bool uploadFonts() {
    VkCommandBuffer cmd = createCommandBuffer();
    VkCommandBufferBeginInfo begin = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr};
    vkBeginCommandBuffer(cmd, &begin);
    ImGui_ImplVulkan_CreateFontsTexture();
    vkEndCommandBuffer(cmd);
    VkSubmitInfo submit = {VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 0, nullptr, nullptr, 1, &cmd, 0, nullptr};
    vkQueueSubmit(g_Queue, 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(g_Queue);
    vkFreeCommandBuffers(g_Device, g_CommandPool, 1, &cmd);
    return true;
}

bool initializeImGui() {
    if (g_Instance == VK_NULL_HANDLE || g_Device == VK_NULL_HANDLE) return false;
    if (g_Framebuffers.size() < g_MinImageCount) return false;

    LOGD("INIT: Starting ImGui Init. Screen: %dx%d, FBs: %zu", g_ScreenExtent.width, g_ScreenExtent.height, g_Framebuffers.size());

    if (!createDescriptorPool()) return false;
    if (g_RenderPass == VK_NULL_HANDLE) {
      LOGE("INIT: No game render pass captured!");
      return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2((float)g_ScreenExtent.width, (float)g_ScreenExtent.height);

    // Font
    /*
    int systemScale = (1.0 / g_ScreenWidth) * g_ScreenWidth;
    ImFontConfig font_cfg;
    font_cfg.SizePixels = systemScale * 22.0f;
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, systemScale * 30.0, 40.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui::GetStyle().ScaleAllSizes(2);
    */
    io.FontGlobalScale = 2.0f;

    ImGui::StyleColorsDark();
    ImGui_ImplAndroid_Init(nullptr);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = g_Instance;
    init_info.PhysicalDevice = g_PhysicalDevice;
    init_info.Device = g_Device;
    init_info.QueueFamily = findGraphicsQueueFamily();
    init_info.Queue = g_Queue;
    init_info.DescriptorPool = g_DescriptorPool;
    init_info.MinImageCount = g_MinImageCount;
    init_info.ImageCount = (uint32_t)g_Framebuffers.size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.RenderPass = g_RenderPass;

    if (!ImGui_ImplVulkan_Init(&init_info)) return false;
    return uploadFonts();
}
#if DEBUG_MENU
void DebugMenu() {
    static int counter;
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("DEBUG MENU")) {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Screen: %dx%d", g_ScreenExtent.width, g_ScreenExtent.height);
        ImGui::Text("Idx: %u ; Ctx: %u", g_ImageIndex, g_CurrentContextIndex);
        ImGui::Text("FB Ptr: %p", (void*)g_Framebuffers[g_ImageIndex]);
        ImGui::Text("FBs: %lu", g_Framebuffers.size());
        ImGui::Text("Click count: %d", counter);
        if(ImGui::Button("CLICK"))
            counter += 1;
    }
    ImGui::End();
}
#endif
/* --- Hooks --- */
VkResult vkCreateFramebufferReplace(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) {
    LOG_CALL("vkCreateFramebufferReplace");
    VkResult res = vkCreateFramebufferOrigin(device, pCreateInfo, pAllocator, pFramebuffer);
    if (res == VK_SUCCESS) {
        LOGD("FBLOG: checking: %dx%d", pCreateInfo->width, pCreateInfo->height);
        /* in portait mode WIDTH = 1080, in landscape HEIGHT = 1080 */
        if (pCreateInfo->height >= 1080) {
            std::lock_guard<std::mutex> lock(g_StateMutex);
            g_Framebuffers.push_back(*pFramebuffer);
            g_ScreenExtent.width = pCreateInfo->width;
            g_ScreenExtent.height = pCreateInfo->height;
            LOGD("FBLOG: pushed %dx%d", pCreateInfo->width, pCreateInfo->height);
        }
    }
    return res;
}

void vkDestroyFramebufferReplace(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) {
    LOG_CALL("vkDestroyFramebufferReplace");
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);

        for (auto it = g_Framebuffers.begin(); it != g_Framebuffers.end();) {
            if (*it == framebuffer) {
                it = g_Framebuffers.erase(it);
                LOGD("FBLOG: Removed Framebuffer %p via Destroy. Size: %zu", framebuffer, g_Framebuffers.size());
            } else {
                ++it;
            }
        }
    }

    vkDestroyFramebufferOrigin(device, framebuffer, pAllocator);
}

VkResult vkAcquireNextImageKHRReplace(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) {
    LOG_CALL("vkAcquireNextImageKHRReplace");
    VkResult res = vkAcquireNextImageKHROrigin(device, swapchain, timeout, semaphore, fence, pImageIndex);
    if (res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR) {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        g_ImageIndex = *pImageIndex;
        g_ImGuiNewFrame = true;
        LOGD("SYNC: Acquire Index %d", g_ImageIndex);
    }
    return res;
}

VkResult vkCreateRenderPass2KHRReplace(VkDevice device, const VkRenderPassCreateInfo2 *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass) {
    LOG_CALL("vkCreateRenderPass2KHRReplace");
    VkResult res = vkCreateRenderPass2KHROrigin(device, pCreateInfo, pAllocator, pRenderPass);

    /* Save first RenderPass with required size */
    if (res == VK_SUCCESS && g_RenderPass == VK_NULL_HANDLE &&
        pCreateInfo->attachmentCount > 0) {
        g_RenderPass = *pRenderPass;
        g_SurfaceFormat = pCreateInfo->pAttachments[0].format;
        LOGD("RNDPS: Captured RenderPass: %p, format=%d", g_RenderPass, g_SurfaceFormat);
    }
    return res;
}

/* --- Render Hook --- */
VkResult vkQueueSubmitReplace(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    /* Render game */
    VkResult gameResult = vkQueueSubmitOrigin(queue, submitCount, pSubmits, fence);
    if (g_InSubmit) return gameResult;

    if (!g_ImGuiInitialized && !g_InitInProgress) {
        if (g_Device && g_Queue && g_CommandPool && g_Framebuffers.size() >= g_MinImageCount && g_ScreenExtent.width > 0) {
            g_InitInProgress = true;
            if (initializeImGui()) {
                initRenderContexts();
                g_ImGuiInitialized = true;
                LOGD("RENDER: ImGui Initialized. FBs: %zu", g_Framebuffers.size());
            } else {
                LOGE("RENDER: Init failed.");
            }
            g_InitInProgress = false;
        }
    }

    bool shouldRender = false;
    {
        std::lock_guard<std::mutex> lock(g_StateMutex);
        if (g_ImGuiNewFrame && g_ImGuiInitialized && !g_Framebuffers.empty() ) {
            shouldRender = true;
            g_ImGuiNewFrame = false;
        }
    }

    /* Render ImGui */
    if (shouldRender) {
        g_InSubmit = true;
#if DEBUG_LOG
        g_FrameCounter++;
        bool debugFrame = (g_FrameCounter % 60 == 0);
#endif
        /* synchronize queues? */
        //vkQueueWaitIdle(queue);

        RenderContext& ctx = g_RenderContexts[g_CurrentContextIndex];
        vkWaitForFences(g_Device, 1, &ctx.fence, VK_TRUE, UINT64_MAX);
        vkResetFences(g_Device, 1, &ctx.fence);
        vkResetCommandBuffer(ctx.commandBuffer, 0);

        VkCommandBufferBeginInfo bi = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr};
        vkBeginCommandBuffer(ctx.commandBuffer, &bi);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplAndroid_NewFrame(g_ScreenExtent.width, g_ScreenExtent.height);
        ImGui::NewFrame();

        Menu::Render();
#if DEBUG_MENU
        DebugMenu();
#endif

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
#if DEBUG_LOG
        if (debugFrame)
             LOGD("RENDER: Render Info -> CmdLists: %d, TotalVtx: %d, FBs: %zu, FB_Index: %d, Screen: %dx%d",
                  draw_data->CmdListsCount, draw_data->TotalVtxCount, g_Framebuffers.size(), g_ImageIndex, g_ScreenExtent.width, g_ScreenExtent.height);
#endif
        uint32_t idx = g_ImageIndex;
        if (idx >= g_Framebuffers.size()) {
          idx = 0;
          LOGE("RENDER: reseting ImgIdx %d -> 0", g_ImageIndex);
        }

        VkRenderPassBeginInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        rpInfo.renderPass = g_RenderPass;
        rpInfo.framebuffer = g_Framebuffers[idx];
        rpInfo.renderArea.extent = g_ScreenExtent;

        vkCmdBeginRenderPass(ctx.commandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
        ImGui_ImplVulkan_RenderDrawData(draw_data, ctx.commandBuffer);
        vkCmdEndRenderPass(ctx.commandBuffer);

        vkEndCommandBuffer(ctx.commandBuffer);

        VkSubmitInfo si = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
        si.commandBufferCount = 1;
        si.pCommandBuffers = &ctx.commandBuffer;

        VkResult imguiRes = vkQueueSubmitOrigin(queue, 1, &si, ctx.fence);
        if (imguiRes != VK_SUCCESS)
            LOGE("RENDER: ImGui QueueSubmit Failed: %d", imguiRes);

        g_CurrentContextIndex = (g_CurrentContextIndex + 1) % 3;
        g_InSubmit = false;
        g_ImGuiNewFrame = false;
    }

    return gameResult;
}

/* -- Capture Device and Instance -- */
VkResult vkCreateDeviceReplace(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    LOG_CALL("vkCreateDeviceReplace");
    VkResult result = vkCreateDeviceOrigin(physicalDevice, pCreateInfo, pAllocator, pDevice);
    if (result == VK_SUCCESS) {
        g_PhysicalDevice = physicalDevice;
        g_Device = *pDevice;
        uint32_t qFam = findGraphicsQueueFamily();
        ((PFN_vkGetDeviceQueue)vkGetDeviceProcAddrOrigin(g_Device, "vkGetDeviceQueue"))(g_Device, qFam, 0, &g_Queue);
        VkCommandPoolCreateInfo poolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, qFam};
        ((PFN_vkCreateCommandPool)vkGetDeviceProcAddrOrigin(g_Device, "vkCreateCommandPool"))(g_Device, &poolInfo, nullptr, &g_CommandPool);
        LOGD("CRTDV: Device captured: Queue Family: %d", qFam);
    }
    return result;
}

VkResult vkCreateInstanceReplace(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    LOG_CALL("vkCreateInstanceReplace");
    VkResult result = vkCreateInstanceOrigin(pCreateInfo, pAllocator, pInstance);
    if (result == VK_SUCCESS) {
        g_Instance = *pInstance;
        LOGD("CRTIN: Instance captured: %p", g_Instance);
    }
    return result;
}


PFN_vkVoidFunction vkGetDeviceProcAddrReplace(VkDevice device, const char* pName) {
    PFN_vkVoidFunction addr = vkGetDeviceProcAddrOrigin(device, pName);
    if (!addr) return nullptr;

    if (strcmp(pName, "vkQueueSubmit") == 0) {
        LOG_HOOK("GTDEV: %s", pName);
        vkQueueSubmitOrigin = (PFN_vkQueueSubmit)addr;
        return (PFN_vkVoidFunction)vkQueueSubmitReplace;
    }
    if (strcmp(pName, "vkCreateFramebuffer") == 0) {
        LOG_HOOK("GTDEV: %s", pName);
        vkCreateFramebufferOrigin = (PFN_vkCreateFramebuffer)addr;
        return (PFN_vkVoidFunction)vkCreateFramebufferReplace;
    }
    if (strcmp(pName, "vkDestroyFramebuffer") == 0) {
        LOG_HOOK("GTDEV: %s", pName);
        vkDestroyFramebufferOrigin = (PFN_vkDestroyFramebuffer)addr;
        return (PFN_vkVoidFunction)vkDestroyFramebufferReplace;
    }
    if (strcmp(pName, "vkCreateRenderPass2KHR") == 0) {
        LOG_HOOK("GTDEV: %s", pName);
        vkCreateRenderPass2KHROrigin = (PFN_vkCreateRenderPass2KHR)addr;
        return (PFN_vkVoidFunction)vkCreateRenderPass2KHRReplace;
    }
    return addr;
}

PFN_vkVoidFunction vkGetInstanceProcAddrReplace(VkInstance instance, const char* pName) {
    PFN_vkVoidFunction addr = vkGetInstanceProcAddrOrigin(instance, pName);
    if (!addr) return nullptr;
    if (instance) g_Instance = instance;

    if (strcmp(pName, "vkCreateInstance") == 0) {
        LOG_HOOK("GTINS: %s", pName);
        vkCreateInstanceOrigin = (PFN_vkCreateInstance)addr;
        return (PFN_vkVoidFunction)vkCreateInstanceReplace;
    }
    if (strcmp(pName, "vkCreateDevice") == 0) {
        LOG_HOOK("GTINS: %s", pName);
        vkCreateDeviceOrigin = (PFN_vkCreateDevice)addr;
        return (PFN_vkVoidFunction)vkCreateDeviceReplace;
    }
    if (strcmp(pName, "vkGetDeviceProcAddr") == 0) {
        LOG_HOOK("GTINS: %s", pName);
        vkGetDeviceProcAddrOrigin = (PFN_vkGetDeviceProcAddr)addr;
        return (PFN_vkVoidFunction)vkGetDeviceProcAddrReplace;
    }
    if (strcmp(pName, "vkAcquireNextImageKHR") == 0) {
        LOG_HOOK("GTINS: %s", pName);
        vkAcquireNextImageKHROrigin = (PFN_vkAcquireNextImageKHR)addr;
        return (PFN_vkVoidFunction)vkAcquireNextImageKHRReplace;
    }
    return addr;
}

void* (*dlsym_)(void* handle, const char* symbol);
void* dlsym_hook(void* handle, const char* symbol) {
    void *addr = dlsym_(handle, symbol);
    if (!addr) return addr;

    if (symbol && strcmp(symbol, "vkGetInstanceProcAddr") == 0) {
        LOG_HOOK("DLSYM: %s", symbol);
        if(!vkGetInstanceProcAddrOrigin)
            vkGetInstanceProcAddrOrigin = (PFN_vkGetInstanceProcAddr)addr;
        return (void*)vkGetInstanceProcAddrReplace;
    }
    return addr;
}

namespace VK {
    void Hook() {
        void* hndl = xdl_open("libdl.so", XDL_TRY_FORCE_LOAD);
        if (DobbyHook(xdl_sym(hndl, "dlsym", nullptr), (void*)dlsym_hook, (void**)&dlsym_) != 0)
            LOGE("HOOK: FAIL dlsym hook failed");
    }
}

