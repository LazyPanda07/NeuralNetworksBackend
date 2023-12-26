#include "BoundingBoxParser.h"

BoundingBoxParser::BoundingBoxParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject BoundingBoxParser::parse(const api::Context& object) const
{
	json::utility::jsonObject boundingBox;
	json::utility::jsonObject jsonBbox;
	json::utility::jsonObject topLeft;
	json::utility::jsonObject bottomRight;
	api::Context bbox = object["bbox"];
	topLeft.setInt("x", static_cast<int64_t>(bbox[0].getDouble() * imageWidth));
	topLeft.setInt("y", static_cast<int64_t>(bbox[1].getDouble() * imageHeight));

	bottomRight.setInt("x", static_cast<int64_t>(bbox[2].getDouble() * imageWidth));
	bottomRight.setInt("y", static_cast<int64_t>(bbox[3].getDouble() * imageHeight));

	jsonBbox.setObject("topLeft", std::move(topLeft));
	jsonBbox.setObject("bottomRight", std::move(bottomRight));

	return jsonBbox;
}
