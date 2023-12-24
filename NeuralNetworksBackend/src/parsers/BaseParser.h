#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "JSONBuilder.h"
#include "api/Context.h"

class BaseParser
{
public:
	static std::unordered_map<std::string, std::unique_ptr<BaseParser>> createParsers(const std::vector<std::string>& unitTypes, int imageWidth, int imageHeight);

protected:
	int imageWidth;
	int imageHeight;

protected:
	BaseParser(int imageWidth, int imageHeight);

public:
	virtual json::utility::jsonObject parse(const api::Context& object) const = 0;

	virtual ~BaseParser() = default;
};
