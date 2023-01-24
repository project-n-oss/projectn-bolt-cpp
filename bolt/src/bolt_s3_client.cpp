#include "bolt_s3_client.h"

namespace ProjectN {
namespace Bolt {

BoltS3Client::BoltS3Client(const Aws::S3::S3ClientConfiguration& clientConfiguration,
                           std::shared_ptr<Aws::S3::Endpoint::S3EndpointProviderBase> endpointProvider)
    : Aws::S3::S3Client(clientConfiguration, endpointProvider){};

BoltS3Client::BoltS3Client(const Aws::Auth::AWSCredentials& credentials,
                           std::shared_ptr<Aws::S3::Endpoint::S3EndpointProviderBase> endpointProvider,
                           const Aws::S3::S3ClientConfiguration& clientConfiguration)
    : Aws::S3::S3Client(credentials, endpointProvider, clientConfiguration) {}

BoltS3Client::BoltS3Client(const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider,
                           std::shared_ptr<Aws::S3::Endpoint::S3EndpointProviderBase> endpointProvider,
                           const Aws::S3::S3ClientConfiguration& clientConfiguration)
    : Aws::S3::S3Client(credentialsProvider, endpointProvider, clientConfiguration) {}

BoltS3Client::BoltS3Client(const Aws::Client::ClientConfiguration& clientConfiguration,
                           Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signPayloads, bool useVirtualAddressing,
                           Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION USEast1RegionalEndPointOption)
    : Aws::S3::S3Client(clientConfiguration, signPayloads, useVirtualAddressing, USEast1RegionalEndPointOption) {}

BoltS3Client::BoltS3Client(const Aws::Auth::AWSCredentials& credentials,
                           const Aws::Client::ClientConfiguration& clientConfiguration,
                           Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signPayloads, bool useVirtualAddressing,
                           Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION USEast1RegionalEndPointOption)
    : Aws::S3::S3Client(credentials, clientConfiguration, signPayloads, useVirtualAddressing,
                        USEast1RegionalEndPointOption) {}

BoltS3Client::BoltS3Client(const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider,
                           const Aws::Client::ClientConfiguration& clientConfiguration,
                           Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signPayloads, bool useVirtualAddressing,
                           Aws::S3::US_EAST_1_REGIONAL_ENDPOINT_OPTION USEast1RegionalEndPointOption)
    : Aws::S3::S3Client(credentialsProvider, clientConfiguration, signPayloads, useVirtualAddressing,
                        USEast1RegionalEndPointOption) {}

}  // namespace Bolt
}  // namespace ProjectN
