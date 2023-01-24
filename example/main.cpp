#include <aws/core/Aws.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/s3/S3Client.h>
#include <projectn/bolt/bolt_s3_client.h>

#include <iostream>

using namespace Aws;

int main() {
  SDKOptions options;
  InitAPI(options);
  {
    ProjectN::Bolt::BoltS3Client client;

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
  return 0;
}