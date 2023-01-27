#include "bolt_s3_http_client_factory.h"

#if ENABLE_CURL_CLIENT
#include <aws/core/http/curl/CurlHttpClient.h>
#include <signal.h>

#elif ENABLE_WINDOWS_CLIENT
#include <aws/core/client/ClientConfiguration.h>
#if ENABLE_WINDOWS_IXML_HTTP_REQUEST_2_CLIENT
#include <aws/core/http/windows/IXmlHttpRequest2HttpClient.h>
#if BYPASS_DEFAULT_PROXY
#include <aws/core/http/windows/WinHttpSyncHttpClient.h>
#endif
#else
#include <aws/core/http/windows/WinHttpSyncHttpClient.h>
#include <aws/core/http/windows/WinINetSyncHttpClient.h>
#endif
#endif

#include <aws/core/http/standard/StandardHttpRequest.h>
#include <aws/core/utils/logging/LogMacros.h>

using namespace Aws::Client;
using namespace Aws::Http;
using namespace Aws::Utils::Logging;

static const char* HTTP_CLIENT_FACTORY_ALLOCATION_TAG = "HttpClientFactory";

namespace ProjectN {
namespace Bolt {
std::shared_ptr<HttpClient> BoltS3HttpClientFactory::CreateHttpClient(const Aws::Client::ClientConfiguration& clientConfiguration) const {
  // Figure out whether the selected option is available but fail gracefully and return a default of some type if not
  // Windows clients:  Http and Inet are always options, Curl MIGHT be an option if USE_CURL_CLIENT is on, and http is "default"
  // Other clients: Curl is your default
#if ENABLE_WINDOWS_CLIENT
#if ENABLE_WINDOWS_IXML_HTTP_REQUEST_2_CLIENT
#if BYPASS_DEFAULT_PROXY
  switch (clientConfiguration.httpLibOverride) {
    case TransferLibType::WIN_HTTP_CLIENT:
      AWS_LOGSTREAM_INFO(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "Creating WinHTTP http client.");
      return Aws::MakeShared<WinHttpSyncHttpClient>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, clientConfiguration);
    case TransferLibType::WIN_INET_CLIENT:
      AWS_LOGSTREAM_WARN(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "WinINet http client is not supported with the current build configuration.");
      // fall-through
    default:
      AWS_LOGSTREAM_INFO(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "Creating IXMLHttpRequest http client.");
      return Aws::MakeShared<IXmlHttpRequest2HttpClient>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, clientConfiguration);
  }
#else
  return Aws::MakeShared<IXmlHttpRequest2HttpClient>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, clientConfiguration);
#endif  // BYPASS_DEFAULT_PROXY
#else
  switch (clientConfiguration.httpLibOverride) {
    case TransferLibType::WIN_INET_CLIENT:
      return Aws::MakeShared<WinINetSyncHttpClient>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, clientConfiguration);

    default:
      return Aws::MakeShared<WinHttpSyncHttpClient>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, clientConfiguration);
  }
#endif  // ENABLE_WINDOWS_IXML_HTTP_REQUEST_2_CLIENT
#elif ENABLE_CURL_CLIENT
  return Aws::MakeShared<CurlHttpClient>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, clientConfiguration);
#else
  // When neither of these clients is enabled, gcc gives a warning (converted
  // to error by -Werror) about the unused clientConfiguration parameter. We
  // prevent that warning with AWS_UNREFERENCED_PARAM.
  AWS_UNREFERENCED_PARAM(clientConfiguration);
  AWS_LOGSTREAM_WARN(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "SDK was built without an Http implementation, default http client factory can't create an Http client instance.");
  return nullptr;
#endif
}

std::shared_ptr<HttpRequest> BoltS3HttpClientFactory::CreateHttpRequest(const Aws::String& uri, HttpMethod method, const Aws::IOStreamFactory& streamFactory) const { return CreateHttpRequest(URI(uri), method, streamFactory); }
std::shared_ptr<HttpRequest> BoltS3HttpClientFactory::CreateHttpRequest(const URI& uri, HttpMethod method, const Aws::IOStreamFactory& streamFactory) const {
  auto request = Aws::MakeShared<Standard::StandardHttpRequest>(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, uri, method);
  request->SetResponseStreamFactory(streamFactory);

  return request;
}
void BoltS3HttpClientFactory::InitStaticState() {
  AWS_LOGSTREAM_DEBUG(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "Initializing Http Static State");
#if ENABLE_CURL_CLIENT
  if (s_InitCleanupCurlFlag) {
    AWS_LOGSTREAM_DEBUG(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "Initializing Curl Http Client");
    CurlHttpClient::InitGlobalState();
  }
#if !defined(_WIN32)
  if (s_InstallSigPipeHandler) {
    ::signal(SIGPIPE, LogAndSwallowHandler);
  }
#endif
#elif ENABLE_WINDOWS_IXML_HTTP_REQUEST_2_CLIENT
  IXmlHttpRequest2HttpClient::InitCOM();
#endif
}
void BoltS3HttpClientFactory::CleanupStaticState() {
  AWS_LOGSTREAM_DEBUG(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "Cleanup Http Static State");
#if ENABLE_CURL_CLIENT
  if (s_InitCleanupCurlFlag) {
    AWS_LOGSTREAM_DEBUG(HTTP_CLIENT_FACTORY_ALLOCATION_TAG, "Cleanup Curl Http Client");
    CurlHttpClient::CleanupGlobalState();
  }
#endif
}
}  // namespace Bolt
}  // namespace ProjectN
