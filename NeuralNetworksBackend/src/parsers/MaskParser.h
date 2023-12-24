#pragma once

#include "BaseParser.h"

class MaskParser : public BaseParser
{
public:
	MaskParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
