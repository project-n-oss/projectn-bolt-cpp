#include "bolt_s3_curl_client.h"

#include <iostream>

namespace ProjectN {
namespace Bolt {

Aws::Http::URI BoltCurlHttpClient::selectedBoltEndpoint = Aws::Http::URI("127.0.0.1");

BoltCurlHttpClient::BoltCurlHttpClient(const Aws::Client::ClientConfiguration &clientConfig) : Aws::Http::CurlHttpClient(clientConfig){};

void BoltCurlHttpClient::OverrideOptionsOnConnectionHandle(CURL *connectionHandle) const {
  std::string boltHostName = BoltConfig::boltHostName;
  Aws::Http::URI boltEndpoint = BoltCurlHttpClient::selectedBoltEndpoint;

  // Get CURL URL
  char *request_url = NULL;
  auto code = curl_easy_getinfo(connectionHandle, CURLINFO_EFFECTIVE_URL, &request_url);

  if (CURLE_OK == code && request_url) {
    std::cout << "urls: " << boltEndpoint.GetURIString() << " | " << request_url << "\n";
    if (boltEndpoint.GetURIString().compare(request_url) == 0) {
      struct curl_slist *host = NULL;
      std::string resolve = boltHostName + ":443:" + boltEndpoint.GetAuthority();
      host = curl_slist_append(NULL, resolve.c_str());
      curl_easy_setopt(connectionHandle, CURLOPT_RESOLVE, host);

      std::string url = "https://" + boltHostName;
      curl_easy_setopt(connectionHandle, CURLOPT_URL, url.c_str());

      std::cout << "OverrideOptionsOnConnectionHandle:"
                << "\n";
      std::cout << "url: " << url << "\n";
      std::cout << "resolve: " << resolve << "\n";
      std::cout << "\n";
    }
  }
}
}  // namespace Bolt
}  // namespace ProjectN