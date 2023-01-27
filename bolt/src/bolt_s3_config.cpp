#include "bolt_s3_config.h"

#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/HttpResponse.h>
#include <aws/core/internal/AWSHttpResourceClient.h>
#include <cpr/cpr.h>

#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string GetEnvVar(std::string const& key) {
  char const* val = getenv(key.c_str());
  return val == NULL ? std::string() : std::string(val);
}

namespace ProjectN {
namespace Bolt {

std::string CreateQuicksilverUrl() {
  std::string zone_id = BoltConfig::zone_id == std::string() ? "" : "?az=" + BoltConfig::zone_id;
  return "https://quicksilver." + BoltConfig::region + "." + BoltConfig::custom_domain + "/services/bolt" + zone_id;
}

// std::string BoltConfig::region = GetEnvVar("AWS_REGION") == std::string() ? Aws::Internal::EC2MetadataClient().GetCurrentRegion() : GetEnvVar("AWS_REGION");
// std::string BoltConfig::zone_id = GetEnvVar("AWS_ZONE_ID") == std::string() ? Aws::Internal::EC2MetadataClient().GetResource("/placement/availability-zone-id") : GetEnvVar("AWS_ZONE_ID");
std::string BoltConfig::region = GetEnvVar("AWS_REGION");
std::string BoltConfig::zone_id = GetEnvVar("AWS_ZONE_ID");
std::string BoltConfig::custom_domain = GetEnvVar("BOLT_CUSTOM_DOMAIN");
std::string BoltConfig::auth_bucket = GetEnvVar("BOLT_AUTH_BUCKET");
std::string BoltConfig::user_agent_prefix = GetEnvVar("USER_AGENT_PREFIX") == std::string() ? "projectn/" : GetEnvVar("USER_AGENT_PREFIX");

std::chrono::time_point<std::chrono::system_clock> BoltConfig::refresh_time = std::chrono::system_clock::now() + std::chrono::seconds(120);
BoltEndpoints BoltConfig::bolt_endpoints = {};
std::string BoltConfig::quicksilver_url = CreateQuicksilverUrl();

const std::array<Aws::Http::HttpMethod, 2> BoltConfig::http_read_methods = {Aws::Http::HttpMethod::HTTP_GET, Aws::Http::HttpMethod::HTTP_HEAD};
const std::vector<std::string> BoltConfig::read_order_endpoints = {"main_read_endpoints", "main_write_endpoints", "failover_read_endpoints", "failover_write_endpoints"};
const std::vector<std::string> BoltConfig::write_order_endpoints = {"main_write_endpoints", "failover_write_endpoints"};

BoltEndpoints BoltConfig::ExecuteRequest(const std::string& get_url) {
  cpr::Response r = cpr::Get(cpr::Url{get_url});
  auto j = json::parse(r.text);
  return j.get<BoltEndpoints>();
}

BoltEndpoints BoltConfig::GetBoltEndpoints(const std::string& err_ip) {
  if (BoltConfig::quicksilver_url == std::string() || BoltConfig::region == std::string()) {
    return {};
  }

  std::string request_url = BoltConfig::quicksilver_url;
  if (err_ip.length() > 0) {
    request_url += ("&err=" + err_ip);
  }

  return BoltConfig::ExecuteRequest(request_url);
}

void BoltConfig::RefreshBoltEndpoints(const std::string& err_ip) {
  BoltConfig::bolt_endpoints = BoltConfig::GetBoltEndpoints(err_ip);
  BoltConfig::refresh_time = std::chrono::system_clock::now() + std::chrono::seconds(60 + (rand() % 120));
}

Aws::Http::URI BoltConfig::SelectBoltEndpoints(const Aws::Http::HttpMethod& http_request_method) {
  if (std::chrono::system_clock::now() > BoltConfig::refresh_time || BoltConfig::bolt_endpoints.empty()) {
    BoltConfig::RefreshBoltEndpoints("");
  }

  auto find_request = std::find(std::begin(BoltConfig::http_read_methods), std::end(BoltConfig::http_read_methods), http_request_method);
  std::vector<std::string> preferred_order = (find_request != std::end(BoltConfig::http_read_methods)) ? BoltConfig::read_order_endpoints : BoltConfig::write_order_endpoints;

  for (std::string end_point_key : preferred_order) {
    auto pos = BoltConfig::bolt_endpoints.find(end_point_key);
    if (pos != BoltConfig::bolt_endpoints.end()) {
      std::vector<std::string> value = pos->second;
      if (value.size() > 0) {
        auto selected_endpoint = value.at(rand() % value.size());
        return Aws::Http::URI(selected_endpoint);
      }
    }
  }

  return Aws::Http::URI();
}

}  // namespace Bolt
}  // namespace ProjectN
