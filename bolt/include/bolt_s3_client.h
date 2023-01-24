#ifndef BOLT_S3_CLIENT_H
#define BOLT_S3_CLIENT_H

#include <aws/core/auth/AWSAuthSigner.h>
#include <aws/core/client/AWSClient.h>
#include <aws/core/client/AWSClientAsyncCRTP.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/DNS.h>
#include <aws/core/utils/xml/XmlSerializer.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/S3ServiceClientModel.h>
#include <aws/s3/S3_EXPORTS.h>

#include <string>

namespace ProjectN {
namespace Bolt {

class BoltConfig {
 public:
  std::string region = "";

 private:
  std::array<std::string, 4> read_order_endpoints = {"main_read_endpoints", "main_write_endpoints",
                                                     "failover_read_endpoints", "failover_write_endpoints"};
  std::array<std::string, 2> write_order_endpoints = {"main_write_endpoints", "failover_write_endpoints"};
  std::array<std::string, 2> HttpReadMethodTypes = {
      "GET",
      "HEAD",
  };  // S3 operations get converted to one of the standard HTTP request methods
      // https://docs.aws.amazon.com/apigateway/latest/developerguide/integrating-api-with-aws-services-s3.html
};

class BoltS3Client : public Aws::S3::S3Client {
 public:
  bool disable_read_passthrough = false;

  /**
   * Initializes client to use DefaultCredentialProviderChain, with default http
   * client factory, and optional client config. If client config is not
   * specified, it will be initialized to default values.
   */
  BoltS3Client(const Aws::S3::S3ClientConfiguration& clientConfiguration = Aws::S3::S3ClientConfiguration(),
               std::shared_ptr<Aws::S3::Endpoint::S3EndpointProviderBase> endpointProvider =
                   Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(ALLOCATION_TAG));

  /**
   * Initializes client to use SimpleAWSCredentialsProvider, with default http
   * client factory, and optional client config. If client config is not
   * specified, it will be initialized to default values.
   */
  BoltS3Client(const Aws::Auth::AWSCredentials& credentials,
               std::shared_ptr<Aws::S3::Endpoint::S3EndpointProviderBase> endpointProvider =
                   Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(ALLOCATION_TAG),
               const Aws::S3::S3ClientConfiguration& clientConfiguration = Aws::S3::S3ClientConfiguration());

  /**
   * Initializes client to use specified credentials provider with specified
   * client config. If http client factory is not supplied, the default http
   * client factory will be used
   */
  BoltS3Client(const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider,
               std::shared_ptr<Aws::S3::Endpoint::S3EndpointProviderBase> endpointProvider =
                   Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(ALLOCATION_TAG),
               const Aws::S3::S3ClientConfiguration& clientConfiguration = Aws::S3::S3ClientConfiguration());

  /* Legacy constructors due deprecation */
  /**
   * Initializes client to use DefaultCredentialProviderChain, with default http
   * client factory, and optional client config. If client config is not
   * specified, it will be initialized to default values.
   */
  BoltS3Client(const Aws::Client::ClientConfiguration& clientConfiguration,
               Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signPayloads, bool useVirtualAddressing,
               Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION USEast1RegionalEndPointOption =
                   Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION::NOT_SET);

  /**
   * Initializes client to use SimpleAWSCredentialsProvider, with default http
   * client factory, and optional client config. If client config is not
   * specified, it will be initialized to default values.
   */
  BoltS3Client(const Aws::Auth::AWSCredentials& credentials,
               const Aws::Client::ClientConfiguration& clientConfiguration,
               Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signPayloads, bool useVirtualAddressing,
               Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION USEast1RegionalEndPointOption =
                   Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION::NOT_SET);

  /**
   * Initializes client to use specified credentials provider with specified
   * client config. If http client factory is not supplied, the default http
   * client factory will be used
   */
  BoltS3Client(const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider,
               const Aws::Client::ClientConfiguration& clientConfiguration,
               Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signPayloads, bool useVirtualAddressing,
               Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION USEast1RegionalEndPointOption =
                   Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION::NOT_SET);

 private:
  int GetCustomDomain();
};

}  // namespace Bolt
}  // namespace ProjectN

#endif  // BOLT_S3_CLIENT_H
