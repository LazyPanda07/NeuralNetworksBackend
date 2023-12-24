#pragma once

#include "BaseParser.h"

class EyeOpennessParser : public BaseParser
{
public:
	EyeOpennessParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
