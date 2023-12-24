#include "GlassesParser.h"

GlassesParser::GlassesParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject GlassesParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;

	result.setBool("hasGlasses", object["has_glasses"]["value"].getBool());
	result.setDouble("confidence", object["glasses_confidence"].getDouble());

	return result;
}
