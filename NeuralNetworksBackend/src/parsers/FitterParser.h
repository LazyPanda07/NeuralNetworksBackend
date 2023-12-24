#pragma once

#include "BaseParser.h"

class FitterParser : public BaseParser
{
private:
	json::utility::jsonObject getCoordinates(const api::Context& namedPoint) const;

public:
	FitterParser(int imageWidth, int imageHeight);

	json::utility::jsonObject parse(const api::Context& object) const override;
};
