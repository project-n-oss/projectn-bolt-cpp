#include "bolt_s3_signer.h"

#include <aws/core/http/HttpRequest.h>
#include <aws/core/http/URI.h>
#include <aws/core/http/standard/StandardHttpRequest.h>

#include "bolt_s3_config.h"
#include "bolt_s3_curl_client.h"

std::vector<std::string> splitString(std::string str, const char splitter) {
  std::vector<std::string> result;
  std::string current = "";
  for (unsigned long i = 0; i < str.size(); i++) {
    if (str[i] == splitter) {
      if (current != "") {
        result.push_back(current);
        current = "";
      }
      continue;
    }
    current += str[i];
  }
  if (current.size() != 0) result.push_back(current);
  return result;
}

std::string getRandomString() {
  int len = 4;
  static const char alpha[] = "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);
  for (int i = 0; i < len; ++i) {
    tmp_s += alpha[rand() % (sizeof(alpha) - 1)];
  }
  return tmp_s;
}

namespace ProjectN {
namespace Bolt {

BoltSigner::BoltSigner(const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider, const char* serviceName, const Aws::String& region, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signingPolicy, bool urlEscapePath,
                       Aws::Auth::AWSSigningAlgorithm signingAlgorithm)
    : m_v4Signer(Aws::MakeUnique<Aws::Client::AWSAuthV4Signer>("BOLT_SIGNER", credentialsProvider, serviceName, region, signingPolicy, urlEscapePath, signingAlgorithm)), m_serviceName(serviceName), m_region(region) {}

BoltSigner::~BoltSigner() {}

bool BoltSigner::SignRequest(Aws::Http::HttpRequest& request, const char* region, const char* serviceName, bool signBody) const {
  Aws::Http::URI boltURI = BoltConfig::SelectBoltEndpoints(request.GetMethod());
  Aws::String path = request.GetUri().GetPath();
  Aws::String sourceBucket = splitString(path, '/').size() > 1 ? splitString(path, '/')[1] : "n-auth-dummy";

  if (BoltConfig::authBucket != "") {
    sourceBucket = BoltConfig::authBucket;
  }

  Aws::String prefix = getRandomString();
  Aws::String headObjectURL = "https://s3." + BoltConfig::region + ".amazonaws.com/" + sourceBucket + "/" + prefix + "/auth";

  request.GetUri() = boltURI;

  Aws::Http::Standard::StandardHttpRequest headRequest(Aws::Http::URI(headObjectURL), Aws::Http::HttpMethod::HTTP_HEAD);
  if (request.GetHeaders().count("x-amz-security-token")) {
    headRequest.SetHeaderValue("x-amz-security-token", request.GetHeaders().at("x-amz-security-token"));
  }
  headRequest.SetHeaderValue("X-Amz-Content-Sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

  // Path: /sdk-test-rvh
  // Source Bucket: n-auth-dummy
  // Bolt Uri: http://10.196.135.232
  // Head Object URL: https://s3.us-east-2.amazonaws.com/n-auth-dummy/owfr/auth
  // request Uri: http://10.196.135.232

  //   std::cout << "Path: " << path << "\n";
  //   std::cout << "Source Bucket: " << sourceBucket << "\n";
  //   std::cout << "Bolt Uri: " << boltURI.GetURIString() << "\n";
  //   std::cout << "Head Object URL: " << headObjectURL << "\n";
  //   std::cout << "request Uri: " << request.GetUri().GetURIString() << "\n";
  //   std::cout << "\n";

  this->m_v4Signer->SignRequest(headRequest, region, serviceName, signBody);

  Aws::Map<Aws::String, Aws::String> iamHeaders = headRequest.GetHeaders();

  if (iamHeaders.count("x-amz-security-token")) {
    request.SetHeaderValue("x-amz-security-token", iamHeaders.at("x-amz-security-token"));
  }

  if (iamHeaders.count("x-amz-date")) {
    request.SetHeaderValue("x-amz-date", iamHeaders.at("x-amz-date"));
  }

  if (iamHeaders.count("authorization")) {
    request.SetHeaderValue("authorization", iamHeaders.at("authorization"));
  }

  if (iamHeaders.count("x-amz-content-sha256")) {
    request.SetHeaderValue("x-amz-content-sha256", iamHeaders.at("x-amz-content-sha256"));
  }

  BoltCurlHttpClient::selectedBoltEndpoint = boltURI;
  std::cout << "GET AUTHORITY: " << boltURI.GetAuthority() << " | " << BoltCurlHttpClient::selectedBoltEndpoint.GetAuthority() << "\n";

  request.SetHeaderValue("host", BoltConfig::boltHostName);
  request.SetHeaderValue("x-bolt-auth-prefix", prefix);
  request.SetHeaderValue("user-agent", BoltConfig::userAgentPrefix + request.GetHeaderValue("user-agent"));

  // TODO: add this as a BoltConfig enum;
  //   request.SetHeaderValue("x-Bolt-Passthrough-Read", "disable");

  std::cout << "Request Headers: \n";
  for (const auto& elem : request.GetHeaders()) {
    std::cout << elem.first << ": " << elem.second << "\n";
  }
  std::cout << "\n";

  return true;
}

bool BoltSigner::PresignRequest(Aws::Http::HttpRequest& request, long long expirationTimeInSeconds) const { return PresignRequest(request, m_region.c_str(), expirationTimeInSeconds); }

bool BoltSigner::PresignRequest(Aws::Http::HttpRequest& request, const char* region, long long expirationInSeconds) const { return PresignRequest(request, region, m_serviceName.c_str(), expirationInSeconds); }

bool BoltSigner::PresignRequest(Aws::Http::HttpRequest& request, const char* region, const char* serviceName, long long expirationTimeInSeconds) const {
  std::cout << " PRESIGN \n";
  Aws::Http::URI boltURI = BoltConfig::SelectBoltEndpoints(request.GetMethod());
  Aws::String path = request.GetUri().GetPath();
  Aws::String sourceBucket = splitString(path, '/').size() > 1 ? splitString(path, '/')[1] : "n-auth-dummy";

  if (BoltConfig::authBucket != "") {
    sourceBucket = BoltConfig::authBucket;
  }

  Aws::String prefix = getRandomString();
  Aws::String headObjectURL = "https://s3." + BoltConfig::region + ".amazonaws.com/" + sourceBucket + "/" + prefix + "/auth";

  request.GetUri() = boltURI;

  Aws::Http::Standard::StandardHttpRequest headRequest(Aws::Http::URI(headObjectURL), Aws::Http::HttpMethod::HTTP_HEAD);
  headRequest.SetHeaderValue("X-Amz-Content-Sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

  this->m_v4Signer->PresignRequest(headRequest, region, serviceName, expirationTimeInSeconds);

  Aws::Map<Aws::String, Aws::String> iamHeaders = headRequest.GetHeaders();

  if (iamHeaders.count("X-Amz-Security-Token")) {
    request.SetHeaderValue("X-Amz-Security-Token", iamHeaders.at("X-Amz-Security-Token"));
  }

  if (iamHeaders.count("X-Amz-Date")) {
    request.SetHeaderValue("X-Amz-Date", iamHeaders.at("X-Amz-Date"));
  }

  if (iamHeaders.count("Authorization")) {
    request.SetHeaderValue("Authorization", iamHeaders.at("Authorization"));
  }

  if (iamHeaders.count("x-amz-content-sha256")) {
    request.SetHeaderValue("x-amz-content-sha256", iamHeaders.at("x-amz-content-sha256"));
  }

  request.SetHeaderValue("Host", BoltConfig::boltHostName);
  request.SetHeaderValue("X-Bolt-Auth-Prefix", prefix);
  request.SetHeaderValue("User-Agent", BoltConfig::userAgentPrefix + request.GetHeaderValue("User-Agent"));
  return true;
}

}  // namespace Bolt
}  // namespace ProjectN
