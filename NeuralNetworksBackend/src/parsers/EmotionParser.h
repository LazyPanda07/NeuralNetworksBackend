#pragma once

#include "BaseParser.h"

class EmotionParser : public BaseParser
{
public:
	EmotionParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
