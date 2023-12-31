#pragma once

#include <vector>
#include <string>
#include <future>
#include <unordered_map>
#include <memory>

#include "parsers/BaseParser.h"
#include "JSONUtility.h"
#include "api/Context.h"

static inline constexpr std::string version = "0.4.0";
extern std::string pathToCurrentModule;

class Sample
{
private:
	std::future<void> imageDataProcessing;
	std::vector<uint8_t> imageData;
	api::Service& service;
	std::string depthType;
	int64_t width;
	int64_t height;
	int64_t channels;

private:
	void decodeBase64(const std::string& base64EncodedData);

public:
	Sample(const json::utility::jsonObject& image, api::Service& service);

	Sample(const Sample& other) = delete;

	Sample(Sample&& other) noexcept = default;

	api::Context createDataContext() const;

	std::unordered_map<std::string, std::unique_ptr<BaseParser>> createParsers(const std::vector<std::string>& unitTypes) const;

	const std::vector<uint8_t>& getImageData() const;

	~Sample() = default;
};
