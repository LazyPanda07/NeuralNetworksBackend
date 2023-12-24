#include "MaskParser.h"

MaskParser::MaskParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject MaskParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;

	result.setBool("hasMask", object["has_medical_mask"]["value"].getBool());

	return result;
}
