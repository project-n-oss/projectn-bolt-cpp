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
  static std::string zone_id;
  static std::string custom_domain;
  static std::string auth_bucket;
  static std::string user_agent_prefix;

  static Aws::Http::URI SelectBoltEndpoints(const Aws::Http::HttpMethod& http_request_method);

 private:
  static std::chrono::time_point<std::chrono::system_clock> refresh_time;
  static BoltEndpoints bolt_endpoints;
  static std::string quicksilver_url;
  const static std::array<Aws::Http::HttpMethod, 2> http_read_methods;
  const static std::vector<std::string> read_order_endpoints;
  const static std::vector<std::string> write_order_endpoints;

  static BoltEndpoints ExecuteRequest(const std::string& get_url);
  static BoltEndpoints GetBoltEndpoints(const std::string& err_ip);
  static void RefreshBoltEndpoints(const std::string& err_ip);
};
}  // namespace Bolt
}  // namespace ProjectN

#endif  // BOLT_S3_CONFIG_H