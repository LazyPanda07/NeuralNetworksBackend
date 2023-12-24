#include "LivenessParser.h"

LivenessParser::LivenessParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject LivenessParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;
	const api::Context& liveness = object["liveness"];

	result.setDouble("confidence", liveness["confidence"].getDouble());
	result.setString("value", liveness["value"].getString());

	return result;
}
