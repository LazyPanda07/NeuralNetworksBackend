#pragma once

#include "BaseParser.h"

class AgeParser : public BaseParser
{
public:
	AgeParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
