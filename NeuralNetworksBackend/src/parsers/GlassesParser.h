#pragma once

#include "BaseParser.h"

class GlassesParser : public BaseParser
{
public:
	GlassesParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
