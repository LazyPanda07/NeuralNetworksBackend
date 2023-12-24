#pragma once

#include "BaseParser.h"

class PoseParser : public BaseParser
{
public:
	PoseParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
