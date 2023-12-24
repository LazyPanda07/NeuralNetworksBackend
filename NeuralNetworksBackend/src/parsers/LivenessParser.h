#pragma once

#include "BaseParser.h"

class LivenessParser : public BaseParser
{
public:
	LivenessParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
