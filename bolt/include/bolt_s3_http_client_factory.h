#ifndef BOLT_S3_HTTP_CLIENT_FACTORY_H
#define BOLT_S3_HTTP_CLIENT_FACTORY_H

#include <aws/core/http/HttpClientFactory.h>

namespace ProjectN {
namespace Bolt {
class BoltS3HttpClientFactory : public Aws::Http::HttpClientFactory {
 public:
  /**
   * Creates a shared_ptr of HttpClient with the relevant settings from clientConfiguration
   */
  std::shared_ptr<Aws::Http::HttpClient> CreateHttpClient(const Aws::Client::ClientConfiguration& clientConfiguration) const override;

  /**
   * Creates a shared_ptr of HttpRequest with uri, method, and closure for how to create a response stream.
   */
  std::shared_ptr<Aws::Http::HttpRequest> CreateHttpRequest(const Aws::String& uri, Aws::Http::HttpMethod method, const Aws::IOStreamFactory& streamFactory) const override;

  /**
   * Creates a shared_ptr of HttpRequest with uri, method, and closure for how to create a response stream.
   */
  std::shared_ptr<Aws::Http::HttpRequest> CreateHttpRequest(const Aws::Http::URI& uri, Aws::Http::HttpMethod method, const Aws::IOStreamFactory& streamFactory) const override;

  void InitStaticState() override;

  void CleanupStaticState() override;
};

AWS_CORE_API void SetInitCleanupCurlFlag(bool initCleanupFlag);
AWS_CORE_API void SetInstallSigPipeHandlerFlag(bool installHandler);
}  // namespace Bolt
}  // namespace ProjectN

#endif  // BOLT_S3_HTTP_CLIENT_FACTORY_H