#include <aws/core/Aws.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <bolt_s3_client.h>
#include <bolt_s3_config.h>

#include <iostream>

using namespace Aws;

void ListObjects() {
  SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;

  std::string bucketName = "sdk-test-rvh";

  ProjectN::Bolt::BoltConfig::region = "us-east-2";
  ProjectN::Bolt::BoltConfig::customDomain = "rvh.bolt.projectn.co";
  ProjectN::Bolt::BoltConfig::Reset();

  InitAPI(options);
  {
    ProjectN::Bolt::BoltS3Client client;
    // Aws::S3::S3Client client;

    Aws::S3::Model::ListObjectsRequest request;
    request.WithBucket(bucketName);

    auto outcome = client.ListObjects(request);

    if (!outcome.IsSuccess()) {
      std::cerr << "Error: ListObjects: " << outcome.GetError().GetMessage() << std::endl;
    } else {
      Aws::Vector<Aws::S3::Model::Object> objects = outcome.GetResult().GetContents();

      for (Aws::S3::Model::Object &object : objects) {
        std::cout << object.GetKey() << std::endl;
      }
    }
  }

  // Before the application terminates, the SDK must be shut down.
  ShutdownAPI(options);
}

int main() {
  ListObjects();
  return 0;
}