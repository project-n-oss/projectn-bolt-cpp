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

Aws::Http::Standard::StandardHttpRequest BoltSigner::CreateBoltHeadRequest(Aws::Http::HttpRequest& request, Aws::Http::URI boltURI, Aws::String prefix) const {
  Aws::String path = request.GetUri().GetPath();
  Aws::String sourceBucket = splitString(path, '/').size() >= 1 ? splitString(path, '/')[0] : "n-auth-dummy";

  if (BoltConfig::authBucket != "") {
    sourceBucket = BoltConfig::authBucket;
  }

  Aws::String headObjectURL = "https://s3." + BoltConfig::region + ".amazonaws.com/" + sourceBucket + "/" + prefix + "/auth";

  Aws::Http::Standard::StandardHttpRequest headRequest(Aws::Http::URI(headObjectURL), Aws::Http::HttpMethod::HTTP_HEAD);
  if (request.GetHeaders().count("x-amz-security-token")) {
    headRequest.SetHeaderValue("x-amz-security-token", request.GetHeaders().at("x-amz-security-token"));
  }
  headRequest.SetHeaderValue("x-amz-content-sha256", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

  return headRequest;
};

bool BoltSigner::SignBoltRequest(Aws::Http::HttpRequest& request, Aws::Http::HeaderValueCollection signedHeaders, Aws::Http::URI boltURI, Aws::String prefix) const {
  request.GetUri() = boltURI;

  if (signedHeaders.count("x-amz-security-token")) {
    request.SetHeaderValue("x-amz-security-token", signedHeaders.at("x-amz-security-token"));
  }

  if (signedHeaders.count("x-amz-date")) {
    request.SetHeaderValue("x-amz-date", signedHeaders.at("x-amz-date"));
  }

  if (signedHeaders.count("authorization")) {
    request.SetHeaderValue("authorization", signedHeaders.at("authorization"));
  }

  if (signedHeaders.count("x-amz-content-sha256")) {
    request.SetHeaderValue("x-amz-content-sha256", signedHeaders.at("x-amz-content-sha256"));
  }

  // Sets bolt endpoint for curl resolve option in bolt_s3_curl_client.cpp.
  // Same as setting the "Host" header, but curl needs special option to be set.
  // This is needed to resolve SSL certificate from Bolt.
  BoltCurlHttpClient::selectedBoltEndpoint = boltURI;

  request.SetHeaderValue("host", BoltConfig::boltHostName);
  request.SetHeaderValue("x-bolt-auth-prefix", prefix);
  request.SetHeaderValue("user-agent", BoltConfig::userAgentPrefix + request.GetHeaderValue("user-agent"));

  if (BoltConfig::disablePassThroughRead) {
    request.SetHeaderValue("x-bolt-passthrough-Read", "disable");
  }

  return true;
}

bool BoltSigner::SignRequest(Aws::Http::HttpRequest& request, const char* region, const char* serviceName, bool signBody) const {
  Aws::Http::URI boltURI = request.GetUri();
  Aws::Http::URI boltEndpoint = BoltConfig::SelectBoltEndpoints(request.GetMethod());
  boltURI.SetAuthority(boltEndpoint.GetAuthority());

  Aws::String prefix = getRandomString();

  Aws::Http::Standard::StandardHttpRequest headRequest = CreateBoltHeadRequest(request, boltURI, prefix);

  // TODO: skew clock for bolt cache
  this->m_v4Signer->SignRequest(headRequest, region, serviceName, true);

  return SignBoltRequest(request, headRequest.GetHeaders(), boltURI, prefix);
}

bool BoltSigner::PresignRequest(Aws::Http::HttpRequest& request, long long expirationTimeInSeconds) const { return PresignRequest(request, m_region.c_str(), expirationTimeInSeconds); }

bool BoltSigner::PresignRequest(Aws::Http::HttpRequest& request, const char* region, long long expirationInSeconds) const { return PresignRequest(request, region, m_serviceName.c_str(), expirationInSeconds); }

bool BoltSigner::PresignRequest(Aws::Http::HttpRequest& request, const char* region, const char* serviceName, long long expirationTimeInSeconds) const {
  Aws::Http::URI boltURI = request.GetUri();
  Aws::Http::URI boltEndpoint = BoltConfig::SelectBoltEndpoints(request.GetMethod());
  boltURI.SetAuthority(boltEndpoint.GetAuthority());
  Aws::String prefix = getRandomString();

  Aws::Http::Standard::StandardHttpRequest headRequest = CreateBoltHeadRequest(request, boltURI, prefix);

  this->m_v4Signer->PresignRequest(headRequest, region, serviceName, expirationTimeInSeconds);

  return SignBoltRequest(request, headRequest.GetHeaders(), boltURI, prefix);
}

}  // namespace Bolt
}  // namespace ProjectN
