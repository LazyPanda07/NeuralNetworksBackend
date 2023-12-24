#include "GenderParser.h"

GenderParser::GenderParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject GenderParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;

	result.setString("gender", object["gender"].getString());

	return result;
}
