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
  std::string zoneId = BoltConfig::zoneId == std::string() ? "" : "?az=" + BoltConfig::zoneId;
  return "https://quicksilver." + BoltConfig::region + "." + BoltConfig::customDomain + "/services/bolt" + zoneId;
}

// std::string BoltConfig::region = GetEnvVar("AWS_REGION") == std::string() ? Aws::Internal::EC2MetadataClient().GetCurrentRegion() : GetEnvVar("AWS_REGION");
// std::string BoltConfig::zoneId = GetEnvVar("AWS_ZONE_ID") == std::string() ? Aws::Internal::EC2MetadataClient().GetResource("/placement/availability-zone-id") : GetEnvVar("AWS_ZONE_ID");
std::string BoltConfig::region = GetEnvVar("AWS_REGION");
std::string BoltConfig::zoneId = GetEnvVar("AWS_ZONE_ID");
std::string BoltConfig::customDomain = GetEnvVar("BOLT_CUSTOM_DOMAIN");
std::string BoltConfig::authBucket = GetEnvVar("BOLT_AUTH_BUCKET");
std::string BoltConfig::userAgentPrefix = GetEnvVar("USER_AGENT_PREFIX") == std::string() ? "projectn/" : GetEnvVar("USER_AGENT_PREFIX");
std::string BoltConfig::boltHostName = "bolt." + BoltConfig::region + "." + BoltConfig::customDomain;

std::chrono::time_point<std::chrono::system_clock> BoltConfig::refreshTime = std::chrono::system_clock::now() + std::chrono::seconds(120);
BoltEndpoints BoltConfig::boltEndpoints = {};
std::string BoltConfig::quicksilverUrl = CreateQuicksilverUrl();

const std::array<Aws::Http::HttpMethod, 2> BoltConfig::httpReadMethods = {Aws::Http::HttpMethod::HTTP_GET, Aws::Http::HttpMethod::HTTP_HEAD};
const std::vector<std::string> BoltConfig::readOrderEndpoints = {"main_read_endpoints", "main_write_endpoints", "failover_read_endpoints", "failover_write_endpoints"};
const std::vector<std::string> BoltConfig::writeOrderEndpoints = {"main_write_endpoints", "failover_write_endpoints"};

void BoltConfig::Reset() {
  BoltConfig::quicksilverUrl = CreateQuicksilverUrl();
  BoltConfig::boltHostName = "bolt." + BoltConfig::region + "." + BoltConfig::customDomain;
}

BoltEndpoints BoltConfig::ExecuteRequest(const std::string& get_url) {
  cpr::Response r = cpr::Get(cpr::Url{get_url});
  if (r.error.code != cpr::ErrorCode::OK) {
    throw BoltException(r.error.message);
  }
  auto j = json::parse(r.text);
  return j.get<BoltEndpoints>();
}

BoltEndpoints BoltConfig::GetBoltEndpoints(const std::string& errIp) {
  if (BoltConfig::quicksilverUrl == std::string() || BoltConfig::region == std::string()) {
    return {};
  }

  std::string requestUrl = BoltConfig::quicksilverUrl;
  if (errIp.length() > 0) {
    requestUrl += ("&err=" + errIp);
  }

  return BoltConfig::ExecuteRequest(requestUrl);
}

void BoltConfig::RefreshBoltEndpoints(const std::string& errIp) {
  BoltConfig::boltEndpoints = BoltConfig::GetBoltEndpoints(errIp);
  BoltConfig::refreshTime = std::chrono::system_clock::now() + std::chrono::seconds(60 + (rand() % 120));
}

Aws::Http::URI BoltConfig::SelectBoltEndpoints(const Aws::Http::HttpMethod& httpRequestMethod) {
  if (std::chrono::system_clock::now() > BoltConfig::refreshTime || BoltConfig::boltEndpoints.empty()) {
    BoltConfig::RefreshBoltEndpoints("");
  }

  auto find_request = std::find(std::begin(BoltConfig::httpReadMethods), std::end(BoltConfig::httpReadMethods), httpRequestMethod);
  std::vector<std::string> preferredOrder = (find_request != std::end(BoltConfig::httpReadMethods)) ? BoltConfig::readOrderEndpoints : BoltConfig::writeOrderEndpoints;

  for (std::string endPointKey : preferredOrder) {
    auto pos = BoltConfig::boltEndpoints.find(endPointKey);
    if (pos != BoltConfig::boltEndpoints.end()) {
      std::vector<std::string> value = pos->second;
      if (value.size() > 0) {
        auto selectedEndpoint = value.at(rand() % value.size());
        auto uri = Aws::Http::URI(selectedEndpoint);
        uri.SetScheme(Aws::Http::Scheme::HTTPS);
        return uri;
      }
    }
  }

  return Aws::Http::URI();
}

}  // namespace Bolt
}  // namespace ProjectN
