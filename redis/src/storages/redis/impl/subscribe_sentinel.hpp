#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <storages/redis/impl/sentinel.hpp>
#include "subscription_storage.hpp"

namespace redis {

class SubscribeSentinel : protected Sentinel {
 public:
  SubscribeSentinel(const std::shared_ptr<ThreadPools>& thread_pools,
                    const std::vector<std::string>& shards,
                    const std::vector<ConnectionInfo>& conns,
                    std::string shard_group_name,
                    const std::string& client_name, const std::string& password,
                    ReadyChangeCallback ready_callback,
                    std::unique_ptr<KeyShard>&& key_shard = nullptr,
                    CommandControl command_control = command_control_init,
                    bool track_masters = true, bool track_slaves = true);
  virtual ~SubscribeSentinel();

  static std::shared_ptr<SubscribeSentinel> Create(
      const std::shared_ptr<ThreadPools>& thread_pools,
      const secdist::RedisSettings& settings, std::string shard_group_name,
      const std::string& client_name);
  static std::shared_ptr<SubscribeSentinel> Create(
      const std::shared_ptr<ThreadPools>& thread_pools,
      const secdist::RedisSettings& settings, std::string shard_group_name,
      const std::string& client_name, ReadyChangeCallback ready_callback);

  SubscriptionToken Subscribe(
      const std::string& channel,
      const Sentinel::UserMessageCallback& message_callback,
      CommandControl control = CommandControl());
  SubscriptionToken Psubscribe(
      const std::string& pattern,
      const Sentinel::UserPmessageCallback& message_callback,
      CommandControl control = CommandControl());

  PubsubClusterStatistics GetSubscriberStatistics() const;

  void RebalanceSubscriptions(size_t shard_idx);

  void SetConfigDefaultCommandControl(
      const std::shared_ptr<CommandControl>& cc) override;

  void SetRebalanceMinInterval(std::chrono::milliseconds interval);

  using Sentinel::Restart;
  using Sentinel::SetConfigDefaultCommandControl;
  using Sentinel::WaitConnectedDebug;

 private:
  struct Stopper {
    std::mutex mutex;
    bool stopped{false};
  };

  std::shared_ptr<SubscriptionStorage> storage_;
  std::shared_ptr<Stopper> stopper_;
};

}  // namespace redis
