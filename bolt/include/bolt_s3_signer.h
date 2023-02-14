#ifndef BOLT_S3_SIGNER_H
#define BOLT_S3_SIGNER_H

#include <aws/core/auth/signer/AWSAuthV4Signer.h>

namespace ProjectN {
namespace Bolt {

class BoltSigner : public Aws::Client::AWSAuthSigner {
 public:
  /**
   * credentialsProvider, source of AWS Credentials to sign requests with
   * serviceName,  canonical service name to sign with
   * region, region string to use in signature
   * signPayloads, if Always, the payload will have a sha256 computed on the body of the request. If this is set
   *    to Never, the sha256 will not be computed on the body. This is only useful for Amazon S3 over Https. If
   *    Https is not used then this flag will be ignored. If set to RequestDependent, compute or not is based on
   *    the value from AmazonWebServiceRequest::SignBody()
   */
  BoltSigner(const std::shared_ptr<Aws::Auth::AWSCredentialsProvider>& credentialsProvider, const char* serviceName, const Aws::String& region,
             Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signingPolicy = Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::RequestDependent, bool urlEscapePath = true,
             Aws::Auth::AWSSigningAlgorithm signingAlgorithm = Aws::Auth::AWSSigningAlgorithm::SIGV4);

  virtual ~BoltSigner();

  /**
   * AWSAuthV4signer's implementation of virtual function from base class
   * Return Auth Signer's name, here the value is specified in Aws::Auth::DEFAULT_AUTHV4_SIGNER.
   */
  const char* GetName() const override { return this->m_v4Signer->GetName(); }

  /**
   * Signs the request itself based on info in the request and uri.
   * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm.
   */
  bool SignRequest(Aws::Http::HttpRequest& request) const override { return SignRequest(request, m_region.c_str(), m_serviceName.c_str(), true /*signBody*/); }

  //   /**
  //    * Signs the request itself based on info in the request and uri.
  //    * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm. If signBody is false
  //    * and https is being used then the body of the payload will not be signed.
  //    */
  bool SignRequest(Aws::Http::HttpRequest& request, bool signBody) const override { return SignRequest(request, m_region.c_str(), m_serviceName.c_str(), signBody); }

  //   /**
  //    * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm. If signBody is false
  //    * and https is being used then the body of the payload will not be signed.
  //    * Using m_region by default if parameter region is nullptr.
  //    */
  bool SignRequest(Aws::Http::HttpRequest& request, const char* region, bool signBody) const override { return SignRequest(request, region, m_serviceName.c_str(), signBody); }

  //   /**
  //    * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm. If signBody is false
  //    * and https is being used then the body of the payload will not be signed.
  //    * Using m_region by default if parameter region is nullptr.
  //    */
  bool SignRequest(Aws::Http::HttpRequest& request, const char* region, const char* serviceName, bool signBody) const override;

  /**
   * Takes a request and signs the URI based on the HttpMethod, URI and other info from the request.
   * the region the signer was initialized with will be used for the signature.
   * The URI can then be used in a normal HTTP call until expiration.
   * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm.
   * expirationInSeconds defaults to 0 which provides a URI good for 7 days.
   */
  bool PresignRequest(Aws::Http::HttpRequest& request, long long expirationInSeconds = 0) const override;

  /**
   * Takes a request and signs the URI based on the HttpMethod, URI and other info from the request.
   * The URI can then be used in a normal HTTP call until expiration.
   * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm.
   * expirationInSeconds defaults to 0 which provides a URI good for 7 days.
   * Using m_region by default if parameter region is nullptr.
   */
  bool PresignRequest(Aws::Http::HttpRequest& request, const char* region, long long expirationInSeconds = 0) const override;

  /**
   * Takes a request and signs the URI based on the HttpMethod, URI and other info from the request.
   * The URI can then be used in a normal HTTP call until expiration.
   * Uses AWS Auth V4 signing method with SHA256 HMAC algorithm.
   * expirationInSeconds defaults to 0 which provides a URI good for 7 days.
   * Using m_region by default if parameter region is nullptr.
   * Using m_serviceName by default if parameter serviceName is nullptr.
   */
  bool PresignRequest(Aws::Http::HttpRequest& request, const char* region, const char* serviceName, long long expirationInSeconds = 0) const override;

 private:
  std::shared_ptr<Aws::Client::AWSAuthV4Signer> m_v4Signer;
  const Aws::String m_serviceName;
  const Aws::String m_region;
};

}  // namespace Bolt
}  // namespace ProjectN

#endif  // BOLT_S3_SIGNER_H