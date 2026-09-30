#include "stdafx.h"

#include "cipher.h"

#ifdef _IMPROVED_PACKET_ENCRYPTION_

#include <cryptopp/modes.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/osrng.h>

// Diffie-Hellman key agreement
#include <cryptopp/dh.h>
#include <cryptopp/dh2.h>

// AES winner and candidates
//#include <cryptopp/aes.h>
#include <cryptopp/cast.h>
#include <cryptopp/rc6.h>
#include <cryptopp/mars.h>
#include <cryptopp/serpent.h>
#include <cryptopp/twofish.h>
// Other block ciphers
#include <cryptopp/blowfish.h>
#include <cryptopp/camellia.h>
#include <cryptopp/des.h>
#include <cryptopp/idea.h>
#include <cryptopp/rc5.h>
#include <cryptopp/seed.h>
#include <cryptopp/shacal2.h>
#include <cryptopp/skipjack.h>
#include <cryptopp/tea.h>

using namespace CryptoPP;

// Block cipher algorithm selector abstract base class.
struct BlockCipherAlgorithm {
  enum {
    kDefault, // to give more chances to default algorithm
    // AES winner and candidates
//    kAES, // Rijndael
    kRC6,
    kMARS,
    kTwofish,
    kSerpent,
    kCAST256,
    // Other block ciphers
    kIDEA,
    k3DES, // DES-EDE2
    kCamellia,
    kSEED,
    kRC5,
    kBlowfish,
    kTEA,
//    kSKIPJACK,
    kSHACAL2,
    // End sentinel
    kMaxAlgorithms
  };

  BlockCipherAlgorithm() {}
  virtual ~BlockCipherAlgorithm() {}

  static BlockCipherAlgorithm* Pick(int hint);

  virtual int GetBlockSize() const = 0;
  virtual int GetDefaultKeyLength() const = 0;
  virtual int GetIVLength() const = 0;

  virtual SymmetricCipher* CreateEncoder(const byte* key, size_t keylen,
                                         const byte* iv) const = 0;
  virtual SymmetricCipher* CreateDecoder(const byte* key, size_t keylen,
                                         const byte* iv) const = 0;
};

// Block cipher (with CTR mode) algorithm selector template class.
template<class T>
struct BlockCipherDetail : public BlockCipherAlgorithm {
  BlockCipherDetail() {}
  virtual ~BlockCipherDetail() {}

  virtual int GetBlockSize() const { return T::BLOCKSIZE; }
  virtual int GetDefaultKeyLength() const { return T::DEFAULT_KEYLENGTH; }
  virtual int GetIVLength() const { return T::IV_LENGTH; }

  virtual SymmetricCipher* CreateEncoder(const byte* key, size_t keylen,
                                         const byte* iv) const {
    return new typename CTR_Mode<T>::Encryption(key, keylen, iv);
  }
  virtual SymmetricCipher* CreateDecoder(const byte* key, size_t keylen,
                                         const byte* iv) const {
    return new typename CTR_Mode<T>::Decryption(key, keylen, iv);
  }
};

// Key agreement scheme abstract class.
class KeyAgreement {
 public:
  KeyAgreement() {}
  virtual ~KeyAgreement() {}

  virtual size_t Prepare(void* buffer, size_t* length) = 0;
  virtual bool Agree(size_t agreed_length, const void* buffer, size_t length) = 0;

  const SecByteBlock& shared() const { return shared_; }

 protected:
  SecByteBlock shared_;
};

// Crypto++ Unified Diffie-Hellman key agreement scheme implementation.
class DH2KeyAgreement : public KeyAgreement {
 public:
  DH2KeyAgreement();
  virtual ~DH2KeyAgreement();

  virtual size_t Prepare(void* buffer, size_t* length);
  virtual bool Agree(size_t agreed_length, const void* buffer, size_t length);

 private:
  DH* dh_;
  DH2* dh2_;
  SecByteBlock spriv_key_;
  SecByteBlock epriv_key_;
};

Cipher::Cipher()
    : activated_(false), encoder_(NULL), decoder_(NULL), key_agreement_(NULL) {
}

Cipher::~Cipher() {
  CleanUp();
}

void Cipher::CleanUp() {
  if (encoder_ != NULL) {
    delete encoder_;
    encoder_ = NULL;
  }
  if (decoder_ != NULL) {
    delete decoder_;
    decoder_ = NULL;
  }
  if (key_agreement_ != NULL) {
    delete key_agreement_;
    key_agreement_ = NULL;
  }
  activated_ = false;
}

size_t Cipher::Prepare(void* buffer, size_t* length) {
  assert(key_agreement_ == NULL);
  try {
    key_agreement_ = new DH2KeyAgreement();
    assert(key_agreement_ != NULL);
    size_t agreed_length = key_agreement_->Prepare(buffer, length);
    if (agreed_length == 0) {
      delete key_agreement_;
      key_agreement_ = NULL;
    }
    return agreed_length;
  }
  catch (const std::exception& e) {
    sys_err("Cipher::Prepare exception: %s", e.what());
    if (key_agreement_) {
      delete key_agreement_;
      key_agreement_ = NULL;
    }
    return 0;
  }
  catch (...) {
    sys_err("Cipher::Prepare unknown exception");
    if (key_agreement_) {
      delete key_agreement_;
      key_agreement_ = NULL;
    }
    return 0;
  }
}

bool Cipher::Activate(bool polarity, size_t agreed_length,
                      const void* buffer, size_t length) {
  assert(activated_ == false);
  assert(key_agreement_ != NULL);
  if (activated_ != false)
	  return false;

  try {
    if (key_agreement_->Agree(agreed_length, buffer, length)) {
      activated_ = SetUp(polarity);
    }
  }
  catch (const std::exception& e) {
    sys_err("Cipher::Activate exception: %s", e.what());
    activated_ = false;
  }
  catch (...) {
    sys_err("Cipher::Activate unknown exception");
    activated_ = false;
  }

  delete key_agreement_;
  key_agreement_ = NULL;
  return activated_;
}

bool Cipher::SetUp(bool polarity) {
  assert(key_agreement_ != NULL);
  try {
    const SecByteBlock& shared = key_agreement_->shared();

    // Pick a block cipher algorithm

    if (shared.size() < 2) {
      return false;
    }
    int hint_0 = shared.BytePtr()[*(shared.BytePtr()) % shared.size()];
    int hint_1 = shared.BytePtr()[*(shared.BytePtr() + 1) % shared.size()];
    BlockCipherAlgorithm* detail_0 = BlockCipherAlgorithm::Pick(hint_0);
    BlockCipherAlgorithm* detail_1 = BlockCipherAlgorithm::Pick(hint_1);
    assert(detail_0 != NULL);
    assert(detail_1 != NULL);
    std::unique_ptr<BlockCipherAlgorithm> algorithm_0(detail_0);
    std::unique_ptr<BlockCipherAlgorithm> algorithm_1(detail_1);

    const size_t key_length_0 = algorithm_0->GetDefaultKeyLength();
    const size_t iv_length_0 = algorithm_0->GetBlockSize();
    if (shared.size() < key_length_0 || shared.size() < iv_length_0) {
      return false;
    }
    const size_t key_length_1 = algorithm_1->GetDefaultKeyLength();
    const size_t iv_length_1 = algorithm_1->GetBlockSize();
    if (shared.size() < key_length_1 || shared.size() < iv_length_1) {
      return false;
    }

    // Pick encryption keys and initial vectors

    SecByteBlock key_0(key_length_0), iv_0(iv_length_0);
    SecByteBlock key_1(key_length_1), iv_1(iv_length_1);

    size_t offset;

    key_0.Assign(shared, key_length_0);
    offset = key_length_0;
    iv_0.Assign(shared.BytePtr() + offset, iv_length_0);
    offset += iv_length_0;

    if (polarity) {
      encoder_ = algorithm_0->CreateEncoder(key_0, key_0.size(), iv_0);
    } else {
      decoder_ = algorithm_0->CreateDecoder(key_0, key_0.size(), iv_0);
    }

    key_1.Assign(shared.BytePtr() + offset, key_length_1);
    offset += key_length_1;
    iv_1.Assign(shared.BytePtr() + offset, iv_length_1);

    if (polarity) {
      decoder_ = algorithm_1->CreateDecoder(key_1, key_1.size(), iv_1);
    } else {
      encoder_ = algorithm_1->CreateEncoder(key_1, key_1.size(), iv_1);
    }

    assert(encoder_ != NULL);
    assert(decoder_ != NULL);
    return true;
  }
  catch (const std::exception& e) {
    sys_err("Cipher::SetUp exception: %s", e.what());
    return false;
  }
  catch (...) {
    sys_err("Cipher::SetUp unknown exception");
    return false;
  }
}

BlockCipherAlgorithm* BlockCipherAlgorithm::Pick(int hint) {
  BlockCipherAlgorithm* detail;
  int selector = hint % kMaxAlgorithms;
  switch (selector) {
//    case kAES:
//      detail = new BlockCipherDetail<AES>();
      break;
    case kRC6:
      detail = new BlockCipherDetail<RC6>();
      break;
    case kMARS:
      detail = new BlockCipherDetail<MARS>();
      break;
    case kTwofish:
      detail = new BlockCipherDetail<Twofish>();
      break;
    case kSerpent:
      detail = new BlockCipherDetail<Serpent>();
      break;
    case kCAST256:
      detail = new BlockCipherDetail<CAST256>();
      break;
    case kIDEA:
      detail = new BlockCipherDetail<IDEA>();
      break;
    case k3DES:
      detail = new BlockCipherDetail<DES_EDE2>();
      break;
    case kCamellia:
      detail = new BlockCipherDetail<Camellia>();
      break;
    case kSEED:
      detail = new BlockCipherDetail<SEED>();
      break;
    case kRC5:
      detail = new BlockCipherDetail<RC5>();
      break;
    case kBlowfish:
      detail = new BlockCipherDetail<Blowfish>();
      break;
    case kTEA:
      detail = new BlockCipherDetail<TEA>();
      break;
//    case kSKIPJACK:
//      detail = new BlockCipherDetail<SKIPJACK>();
//      break;
    case kSHACAL2:
      detail = new BlockCipherDetail<SHACAL2>();
      break;
    case kDefault:
    default:
      detail = new BlockCipherDetail<Twofish>(); // default algorithm
      break;
  }
  return detail;
}

DH2KeyAgreement::DH2KeyAgreement() : dh_(NULL), dh2_(NULL) {
}

DH2KeyAgreement::~DH2KeyAgreement() {
  if (dh2_) {
    delete dh2_;
    dh2_ = NULL;
  }
  if (dh_) {
    delete dh_;
    dh_ = NULL;
  }
}

size_t DH2KeyAgreement::Prepare(void* buffer, size_t* length) {
  try {
    static const byte s_p[] = {
      0xB1, 0x0B, 0x8F, 0x96, 0xA0, 0x80, 0xE0, 0x1D, 0xDE, 0x92, 0xDE, 0x5E, 0xAE, 0x5D, 0x54, 0xEC,
      0x52, 0xC9, 0x9F, 0xBC, 0xFB, 0x06, 0xA3, 0xC6, 0x9A, 0x6A, 0x9D, 0xCA, 0x52, 0xD2, 0x3B, 0x61,
      0x60, 0x73, 0xE2, 0x86, 0x75, 0xA2, 0x3D, 0x18, 0x98, 0x38, 0xEF, 0x1E, 0x2E, 0xE6, 0x52, 0xC0,
      0x13, 0xEC, 0xB4, 0xAE, 0xA9, 0x06, 0x11, 0x23, 0x24, 0x97, 0x5C, 0x3C, 0xD4, 0x9B, 0x83, 0xBF,
      0xAC, 0xCB, 0xDD, 0x7D, 0x90, 0xC4, 0xBD, 0x70, 0x98, 0x48, 0x8E, 0x9C, 0x21, 0x9A, 0x73, 0x72,
      0x4E, 0xFF, 0xD6, 0xFA, 0xE5, 0x64, 0x47, 0x38, 0xFA, 0xA3, 0x1A, 0x4F, 0xF5, 0x5B, 0xCC, 0xC0,
      0xA1, 0x51, 0xAF, 0x5F, 0x0D, 0xC8, 0xB4, 0xBD, 0x45, 0xBF, 0x37, 0xDF, 0x36, 0x5C, 0x1A, 0x65,
      0xE6, 0x8C, 0xFD, 0xA7, 0x6D, 0x4D, 0xA7, 0x08, 0xDF, 0x1F, 0xB2, 0xBC, 0x2E, 0x4A, 0x43, 0x71
    };

    static const byte s_g[] = {
      0xA4, 0xD1, 0xCB, 0xD5, 0xC3, 0xFD, 0x34, 0x12, 0x67, 0x65, 0xA4, 0x42, 0xEF, 0xB9, 0x99, 0x05,
      0xF8, 0x10, 0x4D, 0xD2, 0x58, 0xAC, 0x50, 0x7F, 0xD6, 0x40, 0x6C, 0xFF, 0x14, 0x26, 0x6D, 0x31,
      0x26, 0x6F, 0xEA, 0x1E, 0x5C, 0x41, 0x56, 0x4B, 0x77, 0x7E, 0x69, 0x0F, 0x55, 0x04, 0xF2, 0x13,
      0x16, 0x02, 0x17, 0xB4, 0xB0, 0x1B, 0x88, 0x6A, 0x5E, 0x91, 0x54, 0x7F, 0x9E, 0x27, 0x49, 0xF4,
      0xD7, 0xFB, 0xD7, 0xD3, 0xB9, 0xA9, 0x2E, 0xE1, 0x90, 0x9D, 0x0D, 0x22, 0x63, 0xF8, 0x0A, 0x76,
      0xA6, 0xA2, 0x4C, 0x08, 0x7A, 0x09, 0x1F, 0x53, 0x1D, 0xBF, 0x0A, 0x01, 0x69, 0xB6, 0xA2, 0x8A,
      0xD6, 0x62, 0xA4, 0xD1, 0x8E, 0x73, 0xAF, 0xA3, 0x2D, 0x77, 0x9D, 0x59, 0x18, 0xD0, 0x8B, 0xC8,
      0x85, 0x8F, 0x4D, 0xCE, 0xF9, 0x7C, 0x2A, 0x24, 0x85, 0x5E, 0x6E, 0xEB, 0x22, 0xB3, 0xB2, 0xE5
    };

    static const byte s_q[] = {
      0xF5, 0x18, 0xAA, 0x87, 0x81, 0xA8, 0xDF, 0x27, 0x8A, 0xBA, 0x4E, 0x7D, 0x64, 0xB7, 0xCB, 0x9D,
      0x49, 0x46, 0x23, 0x53
    };

    Integer p(s_p, sizeof(s_p));
    Integer g(s_g, sizeof(s_g));
    Integer q(s_q, sizeof(s_q));

    AutoSeededRandomPool rnd;

    if (dh2_ || dh_) {
      delete dh2_;
      delete dh_;
      dh2_ = NULL;
      dh_ = NULL;
    }

    dh_ = new DH();
    dh_->AccessGroupParameters().Initialize(p, q, g);
    dh2_ = new DH2(*dh_);

    spriv_key_.New(dh2_->StaticPrivateKeyLength());
    epriv_key_.New(dh2_->EphemeralPrivateKeyLength());
    SecByteBlock spub_key(dh2_->StaticPublicKeyLength());
    SecByteBlock epub_key(dh2_->EphemeralPublicKeyLength());

    dh2_->GenerateStaticKeyPair(rnd, spriv_key_, spub_key);
    dh2_->GenerateEphemeralKeyPair(rnd, epriv_key_, epub_key);

    const size_t spub_key_length = spub_key.size();
    const size_t epub_key_length = epub_key.size();
    const size_t data_length = spub_key_length + epub_key_length;
    if (*length < data_length) {
      return 0;
    }
    *length = data_length;
    byte* buf = (byte*)buffer;
    memcpy(buf, spub_key.BytePtr(), spub_key_length);
    memcpy(buf + spub_key_length, epub_key.BytePtr(), epub_key_length);

    return dh2_->AgreedValueLength();
  }
  catch (const std::exception& e) {
    sys_err("DH2KeyAgreement::Prepare exception: %s", e.what());
    return 0;
  }
  catch (...) {
    sys_err("DH2KeyAgreement::Prepare unknown exception");
    return 0;
  }
}

bool DH2KeyAgreement::Agree(size_t agreed_length, const void* buffer, size_t length) {
  try {
    if (!dh2_) {
      return false;
    }
    if (agreed_length != dh2_->AgreedValueLength()) {
      return false;
    }
    const size_t spub_key_length = dh2_->StaticPublicKeyLength();
    const size_t epub_key_length = dh2_->EphemeralPublicKeyLength();
    if (length != (spub_key_length + epub_key_length)) {
      return false;
    }
    shared_.New(dh2_->AgreedValueLength());
    const byte* buf = (const byte*)buffer;
    if (!dh2_->Agree(shared_, spriv_key_, epriv_key_, buf, buf + spub_key_length)) {
      return false;
    }
    return true;
  }
  catch (const std::exception& e) {
    sys_err("DH2KeyAgreement::Agree exception: %s", e.what());
    return false;
  }
  catch (...) {
    sys_err("DH2KeyAgreement::Agree unknown exception");
    return false;
  }
}

#endif // _IMPROVED_PACKET_ENCRYPTION_

// EOF cipher.cpp
