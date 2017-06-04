#include "RectF.h"

RectF::RectF(int left_in, int right_in, int top_in, int bottom_in)
	:
	left(left_in),
	right(right_in),
	top(top_in),
	bottom(bottom_in)
{
}

RectF::RectF(Vec2&  top_left_in, Vec2&  bottom_right_in) 
	:
	RectF(top_left_in.x,bottom_right_in.x,top_left_in.y,bottom_right_in.y)
{
}

RectF::RectF(Vec2 & top_left_in, int width, int height)
	:
	RectF(top_left_in,top_left_in + Vec2(width,height))
{
}

bool RectF::IsOverLappingWith(const RectF & other) const
{
	return right > other.left && left < other.right && bottom > other.top && top < other.bottom;
}

bool RectF::isOverLappTop(const RectF & other) const
{
	return (bottom > other.top && ((right > other.left && right < other.right)||(left < other.right && left > other.left ))) || (top > other.bottom && ((right > other.left && right < other.right) || (left < other.right && left > other.left)));
}

bool RectF::isInsideRect(Mouse & mouse) const
{
	int x_pos = mouse.GetPosX();
	int y_pos = mouse.GetPosY();
	return x_pos > left && x_pos < right && y_pos > top && y_pos < bottom;
}
