#include "imageProcessing.hpp"

Point2f ImageProcessing::findVanishingPoint(
	Point2f p11, 
	Point2f p12, 
	Point2f p21, 
	Point2f p22)
{
	double m1 = (p12.y - p11.y) / (p12.x - p11.x);
	double b1 = p11.y - m1 * p11.x;
	double m2 = (p22.y - p21.y) / (p22.x - p21.x);
	double b2 = p21.y - m2 * p21.x;

	double vanishingX = (b2 - b1) / (m1 - m2);
	double vanishingY = m1 * vanishingX + b1;

	return Point2f(vanishingX, vanishingY);
}