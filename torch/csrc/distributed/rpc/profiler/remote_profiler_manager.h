#pragma once
#include <c10/util/Optional.h>
#include <torch/csrc/WindowsTorchApiMacro.h>
#include <torch/csrc/distributed/rpc/types.h>
#include <mutex>
#include <unordered_map>

namespace torch {
namespace distributed {
namespace rpc {
class TORCH_API RemoteProfilerManager {
 public:
  // Retrieves the lazily-initialized RemoteProfilerManager singleton instance.
  static RemoteProfilerManager& getInstance();
  // Sets the current, thread-local profiling key.
  void setCurrentKey(const std::string key);
  // Unsets the current, thread-local profiling key to allow other RPCs to reset
  // it.
  void unsetCurrentKey();
  // inserts a pair (globallyUniqueId, key) to an in-memory map. The
  // corresponding ID is used in RPC deserialization to prefix remotely profiled
  // events with the right key.
  void saveRPCKey(
      GloballyUniqueId globallyUniqueId,
      std::string rpcProfilingKey);
  // Retrieves the profiling key corresponding to the given globallyUniqueId.
  // Throws if it is not found.
  std::string retrieveRPCProfilingKey(GloballyUniqueId globallyUniqueId);
  // Generates the next unique globally unique ID.
  local_id_t getNextLocalId();
  // Retrieves the currently set thread-local profiling key. Throws if it is not
  // set.
  std::string getCurrentProfilingKey();
  // erases the globallyUniqueId from the map. This can help save memory in the
  // case that many RPCs are being profiled.
  void eraseKey(GloballyUniqueId globallyUniqueId);

 private:
  RemoteProfilerManager();
  ~RemoteProfilerManager() = default;
  RemoteProfilerManager(const RemoteProfilerManager& other) = delete;
  RemoteProfilerManager operator=(const RemoteProfilerManager& other) = delete;
  RemoteProfilerManager(RemoteProfilerManager&&) = delete;
  RemoteProfilerManager& operator=(RemoteProfilerManager&&) = delete;
  std::unordered_map<GloballyUniqueId, std::string, GloballyUniqueId::Hash>
      profiledRpcKeys_;
  static thread_local c10::optional<std::string> currentThreadLocalKey_;
  std::mutex mutex_;
  local_id_t currentLocalId_;
};
} // namespace rpc
} // namespace distributed
} // namespace torch