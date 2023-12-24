#pragma once

#include "BaseParser.h"

class GenderParser : public BaseParser
{
public:
	GenderParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
