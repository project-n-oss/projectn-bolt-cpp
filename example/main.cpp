#include <aws/core/Aws.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/s3/S3Client.h>
#include <bolt_s3_client.h>
#include <bolt_s3_config.h>

#include <iostream>

using namespace Aws;

void ListBuckets() {
  SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;

  ProjectN::Bolt::BoltConfig::region = "us-east-1";
  ProjectN::Bolt::BoltConfig::customDomain = "example.com";
  ProjectN::Bolt::BoltConfig::Reset();

  InitAPI(options);
  {
    ProjectN::Bolt::BoltS3Client client;
    // Aws::S3::S3Client client;

    auto outcome = client.ListBuckets();
    if (outcome.IsSuccess()) {
      std::cout << "Found " << outcome.GetResult().GetBuckets().size() << " buckets\n";
      for (auto&& b : outcome.GetResult().GetBuckets()) {
        std::cout << b.GetName() << std::endl;
      }
    } else {
      std::cout << "Failed with error: " << outcome.GetError() << std::endl;
    }
  }

  // Before the application terminates, the SDK must be shut down.
  ShutdownAPI(options);
}

int main() {
  ListBuckets();
  return 0;
}