#pragma once

#include "BaseParser.h"

class BoundingBoxParser : public BaseParser
{
public:
	BoundingBoxParser(int imageWidth, int imageHeight);
	
	json::utility::jsonObject parse(const api::Context& object) const override;
};
