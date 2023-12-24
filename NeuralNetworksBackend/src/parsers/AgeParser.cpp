#include "AgeParser.h"

AgeParser::AgeParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject AgeParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;
	
	result.setInt("age", object["age"].getLong());

	return result;
}
