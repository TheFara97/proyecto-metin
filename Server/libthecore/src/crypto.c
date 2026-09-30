#include "stdafx.h"
#include <string>
#include <sstream>

// Base64 functions from cryptopp
#include <cryptopp/base64.h>
#include <cryptopp/base32.h>
#include <cryptopp/hex.h>

std::string EncodeBase32(const std::string& sRaw, bool bPadding)
{
	using namespace CryptoPP;
	std::string sEncoded;

	StringSource ss(sRaw, true, new Base32Encoder(new StringSink(sEncoded)));
	if (bPadding && sEncoded.size() % 8 != 0)
		sEncoded.append((8 - sEncoded.size() % 8), '=');

	return sEncoded;
}

std::string EncodeBase64(const std::string& sRaw)
{
	using namespace CryptoPP;
	std::string sEncoded;

	StringSource ss(sRaw, true, new HexEncoder(new Base64Encoder(new StringSink(sEncoded))));
	return sEncoded;
}

std::string DecodeBase64(const std::string& sRaw)
{
	using namespace CryptoPP;
	std::string sDecoded;

	StringSource ss(sRaw, true, new Base64Decoder(new HexDecoder(new StringSink(sDecoded))));
	return sDecoded;
}

