#ifndef BOLT_S3_HTTP_CLIENT_FACTORY_H
#define BOLT_S3_HTTP_CLIENT_FACTORY_H

#include <aws/core/http/HttpClientFactory.h>

namespace ProjectN {
namespace Bolt {
class BoltS3HttpClientFactory : public Aws::Http::HttpClientFactory {
 public:
  std::shared_ptr<Aws::Http::HttpClient> CreateHttpClient(const Aws::Client::ClientConfiguration& clientConfiguration) const override;

  std::shared_ptr<Aws::Http::HttpRequest> CreateHttpRequest(const Aws::String& uri, Aws::Http::HttpMethod method, const Aws::IOStreamFactory& streamFactory) const override;

  std::shared_ptr<Aws::Http::HttpRequest> CreateHttpRequest(const Aws::Http::URI& uri, Aws::Http::HttpMethod method, const Aws::IOStreamFactory& streamFactory) const override;

  void InitStaticState() override;

  void CleanupStaticState() override;
};
}  // namespace Bolt

}  // namespace ProjectN

#endif  // BOLT_S3_HTTP_CLIENT_FACTORY_H