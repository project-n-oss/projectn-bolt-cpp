#include <aws/core/Aws.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/s3/S3Client.h>
#include <bolt_s3_client.h>
#include <bolt_s3_config.h>
#include <bolt_s3_http_client_factory.h>

#include <iostream>

using namespace Aws;
static const char ALLOCATION_TAG[] = "OverrideDefaultHttpClient";

std::shared_ptr<Aws::Http::HttpClientFactory> CreateFactory() { return Aws::MakeShared<ProjectN::Bolt::BoltS3HttpClientFactory>(ALLOCATION_TAG); }

void ListBuckets() {
  SDKOptions options;
  options.httpOptions.httpClientFactory_create_fn = CreateFactory;
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
}

int main() {
  std::cout << "hello world"
            << "\n";
  ListBuckets();
  return 0;
}