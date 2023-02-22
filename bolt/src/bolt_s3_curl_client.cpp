#include "bolt_s3_curl_client.h"

#include <iostream>

namespace ProjectN {
namespace Bolt {

Aws::Http::URI BoltCurlHttpClient::selectedBoltEndpoint = Aws::Http::URI("127.0.0.1");

BoltCurlHttpClient::BoltCurlHttpClient(const Aws::Client::ClientConfiguration &clientConfig) : Aws::Http::CurlHttpClient(clientConfig){};

void BoltCurlHttpClient::OverrideOptionsOnConnectionHandle(CURL *connectionHandle) const {
  std::string boltHostName = BoltConfig::boltHostName;
  Aws::Http::URI boltEndpoint = BoltCurlHttpClient::selectedBoltEndpoint;

  // Get current CURL URL
  char *request_url = NULL;
  auto code = curl_easy_getinfo(connectionHandle, CURLINFO_EFFECTIVE_URL, &request_url);

  if (CURLE_OK == code && request_url) {
    // if curl url is same as the set boltEndpoint, then we want to change the CURLOPT_RESOLVE option for
    // Bolt ssl certificate.
    // else don't change curl.
    if (boltEndpoint.GetURIString().compare(request_url) == 0) {
      struct curl_slist *host = NULL;
      std::string resolve = boltHostName + ":443:" + boltEndpoint.GetAuthority();
      host = curl_slist_append(NULL, resolve.c_str());
      curl_easy_setopt(connectionHandle, CURLOPT_RESOLVE, host);

      Aws::Http::URI newUri = boltEndpoint;
      newUri.SetAuthority(boltHostName);
      curl_easy_setopt(connectionHandle, CURLOPT_URL, newUri.GetURIString().c_str());
    }
  }
}
}  // namespace Bolt
}  // namespace ProjectN