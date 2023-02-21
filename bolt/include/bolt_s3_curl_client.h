#ifndef BOLT_S3_CURLT_CLIENT_H
#define BOLT_S3_CURLT_CLIENT_H

#include <aws/core/http/curl/CurlHttpClient.h>

#include "bolt_s3_config.h"

namespace ProjectN {
namespace Bolt {
class BoltCurlHttpClient : public Aws::Http::CurlHttpClient {
 public:
  BoltCurlHttpClient(const Aws::Client::ClientConfiguration& clientConfig);

  static std::string selectedBoltEndpoint;

 protected:
  void OverrideOptionsOnConnectionHandle(CURL* connectionHandle) const override;
};
}  // namespace Bolt
}  // namespace ProjectN

#endif  // BOLT_S3_CURLT_CLIENT_H