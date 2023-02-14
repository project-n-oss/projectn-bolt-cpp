#ifndef BOLT_S3_CONFIG_H
#define BOLT_S3_CONFIG_H

#include <aws/core/http/HttpTypes.h>
#include <aws/core/http/URI.h>
#include <aws/core/utils/memory/stl/AWSString.h>

#include <array>
#include <chrono>
#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, std::vector<std::string>> BoltEndpoints;

namespace ProjectN {
namespace Bolt {
class BoltConfig {
 public:
  static std::string region;
  static std::string zoneId;
  static std::string customDomain;
  static std::string authBucket;
  static std::string userAgentPrefix;
  static std::string boltHostName;

  static Aws::Http::URI SelectBoltEndpoints(const Aws::Http::HttpMethod& httpRequestMethod);
  static void Reset();

 private:
  static std::chrono::time_point<std::chrono::system_clock> refreshTime;
  static BoltEndpoints boltEndpoints;
  static std::string quicksilverUrl;
  const static std::array<Aws::Http::HttpMethod, 2> httpReadMethods;
  const static std::vector<std::string> readOrderEndpoints;
  const static std::vector<std::string> writeOrderEndpoints;

  static BoltEndpoints ExecuteRequest(const std::string& getUrl);
  static BoltEndpoints GetBoltEndpoints(const std::string& errIp);
  static void RefreshBoltEndpoints(const std::string& errIp);
};

class BoltException : public std::exception {
 private:
  std::string message;

 public:
  explicit BoltException(const std::string& msg) : message(msg) {}
  const char* what() const noexcept override { return message.c_str(); }
};

}  // namespace Bolt
}  // namespace ProjectN

#endif  // BOLT_S3_CONFIG_H