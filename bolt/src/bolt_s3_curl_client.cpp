#include "bolt_s3_curl_client.h"

namespace ProjectN {
namespace Bolt {
std::string BoltCurlHttpClient::selectedBoltEndpoint = "";

BoltCurlHttpClient::BoltCurlHttpClient(const Aws::Client::ClientConfiguration &clientConfig) : Aws::Http::CurlHttpClient(clientConfig){};

void BoltCurlHttpClient::OverrideOptionsOnConnectionHandle(CURL *connectionHandle) const {
  std::string boltHostName = BoltConfig::boltHostName;

  struct curl_slist *host = NULL;
  std::string resolve = boltHostName + ":443:" + selectedBoltEndpoint;
  host = curl_slist_append(NULL, resolve.c_str());
  curl_easy_setopt(connectionHandle, CURLOPT_RESOLVE, host);

  std::string url = "https://" + boltHostName;
  curl_easy_setopt(connectionHandle, CURLOPT_URL, url.c_str());
}
}  // namespace Bolt
}  // namespace ProjectN