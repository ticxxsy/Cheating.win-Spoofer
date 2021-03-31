#include <string>
#include "../../Protection/xor.hpp"
#include "../../libs/cryptopp/include/cryptopp/aes.h"
#include "../../libs/cryptopp/include/cryptopp/osrng.h"
#include "../../libs/cryptopp/include/cryptopp/cryptlib.h"
#include "../../libs/cryptopp/include/cryptopp/base64.h"
#include "../../libs/cryptopp/include/cryptopp/filters.h"
#include "../../libs/cryptopp/include/cryptopp/modes.h"
#include "../../libs/cryptopp/include/cryptopp/ccm.h"
#include "../../libs/cryptopp/include/cryptopp/hex.h"

#include "../../libs/curl/include/curl/curl.h"

#include <atlsecurity.h>
#include <vector>
#include <random>
#include <ctime>

#pragma comment(lib, "rpcrt4.lib")

namespace AesEncryption {
	std::string encryption_key = _xors("mvt6JuByn6fq2yAf8dxkbaz2hPN8J6PD");
	inline std::string encrypt_string(std::string plain_text, std::string key, std::string iv) {
		std::string cipher_text;

		try {
			CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption;
			encryption.SetKeyWithIV((CryptoPP::byte*)key.c_str(), key.size(), (CryptoPP::byte*)iv.c_str());

			CryptoPP::StringSource encryptor(plain_text, true,
				new CryptoPP::StreamTransformationFilter(encryption,
					new CryptoPP::Base64Encoder(new CryptoPP::StringSink(cipher_text), false)
				));
		}
		catch (CryptoPP::Exception ex) {
			MessageBoxA(0, ex.what(), nullptr, MB_ICONERROR);
			exit(0);
		}
		return cipher_text;
	}

	static std::string decrypt_string(std::string cipher_text, std::string key, std::string iv) {
		std::string plain_text;

		try {
			CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryption;
			decryption.SetKeyWithIV((CryptoPP::byte*)key.c_str(), key.size(), (CryptoPP::byte*)iv.c_str());

			CryptoPP::StringSource decryptor(cipher_text, true,
				new CryptoPP::Base64Decoder(new CryptoPP::StreamTransformationFilter(decryption,
					new CryptoPP::StringSink(plain_text)))
			);
		}
		catch (CryptoPP::Exception ex) {
			MessageBoxA(0, ex.what(), nullptr, MB_ICONERROR);
			exit(0);
		}
		return plain_text;
	}
}

namespace api {
	std::string api_endpoint = _xors("https://fortknoxspoofer.xyz/api/");
	std::string api_pub_key = _xors("sha256//mMPNvAgvzFhGb2aQR8oUiAVerrjTRfGlppvKAWHNajg=");

	inline std::string get_hwid() {
		ATL::CAccessToken accessToken;
		ATL::CSid currentUserSid;
		if (accessToken.GetProcessToken(TOKEN_READ | TOKEN_QUERY) &&
			accessToken.GetUser(&currentUserSid))
			return std::string(CT2A(currentUserSid.Sid()));
	}

	inline std::vector<std::string> split(const std::string& str, const char separator) {
		std::vector<std::string> output;
		std::string::size_type prev_pos = 0, pos = 0;

		while ((pos = str.find(separator, pos)) != std::string::npos) {
			auto substring(str.substr(prev_pos, pos - prev_pos));
			output.push_back(substring);
			prev_pos = ++pos;
		}

		output.push_back(str.substr(prev_pos, pos - prev_pos));
		return output;
	}

	std::string random_string(std::string::size_type length) {
		static std::string chars = "0123456789"
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		thread_local static std::default_random_engine rg{ std::random_device { }() };
		thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, chars.length() - 2);

		std::string s;

		s.reserve(length);

		while (length--)
			s += chars[pick(rg)];

		return s;
	}

	inline size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	static std::string license = "";
	static std::time_t expiry = 0;

	inline bool login(std::string license) {
		CURL* curl_handle;
		CURLcode code;

		std::string out;

		curl_handle = curl_easy_init();

		if (curl_handle) {
			curl_easy_setopt(curl_handle, CURLOPT_URL, std::string(api_endpoint).c_str());
			curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, _xors("Diverex"));
			curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, _xors("Diverex"));

			curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(curl_handle, CURLOPT_PINNEDPUBLICKEY, api_pub_key.c_str());

			std::string sessiv = random_string(16);

			std::string enc_ssv = AesEncryption::encrypt_string(sessiv, AesEncryption::encryption_key, _xors("bpCLxm99a5xbpVLk"));

			std::string enc_license = AesEncryption::encrypt_string(license, AesEncryption::encryption_key, sessiv);

			std::string enc_hwid = AesEncryption::encrypt_string(get_hwid(), AesEncryption::encryption_key, sessiv);

			std::string values =
				_xors("csv=") + std::string(curl_easy_escape(curl_handle, enc_ssv.c_str(), enc_ssv.length())) +
				_xors("&lcs=") + std::string(curl_easy_escape(curl_handle, enc_license.c_str(), enc_license.length())) +
				_xors("&hwd=") + std::string(curl_easy_escape(curl_handle, enc_hwid.c_str(), enc_hwid.length()));

			curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, values.c_str());
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, values.size());

			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &out);

			code = curl_easy_perform(curl_handle);

			if (code != CURLE_OK)
				MessageBoxA(0, curl_easy_strerror(code), 0, 0);

			curl_easy_cleanup(curl_handle);
			
			out = AesEncryption::decrypt_string(out, AesEncryption::encryption_key, sessiv);

			if (out == _xors("invalid_license")) {
				MessageBoxA(0, _xors("Key is nonexistent").c_str(), _xors("Cheating.win").c_str(), MB_OK);
				for (;;) { exit(0); }
				return false;
			}
			else if (out == _xors("invalid_hwid")) {
				MessageBoxA(0, _xors("HWID mismatch").c_str(), _xors("Cheating.win").c_str(), MB_OK);
				for (;;) { exit(0); }
				return false;
			}
			else if (out == _xors("expired_sub")) {
				MessageBoxA(0, _xors("Key is expired").c_str(), _xors("Cheating.win").c_str(), MB_OK);
				_getch(); for (;;) { exit(0); }
				return false;
			}
			else if (out == _xors("banned_license")) {
				MessageBoxA(0, _xors("Key is banned").c_str(), _xors("Cheating.win").c_str(), MB_OK);
				_getch(); for (;;) { exit(0); }
				return false;
			}
			else if (out.find(_xors("logged_in")) != std::string::npos) {
				auto sp = split(out, '|');
				license = license;
				return true;
			}
			else {
				MessageBoxA(0, _xors("An unknown error occurred").c_str(), _xors("Cheating.win").c_str(), MB_OK);
				_getch(); for (;;) { exit(0); }
				return false;
			}
		}
		return false;
	}
}