#pragma once
#include "Vec2.h"
#include "Mouse.h"
class RectF
{
public:
	RectF() = default;
	RectF(int left_in, int right_in, int top_in, int bottom_in);
	RectF(Vec2& top_left_in, Vec2& bottom_right_in) ;
	RectF(Vec2& top_left_in, int width, int height);
	bool IsOverLappingWith(const RectF& other) const;
	bool isOverLappTop(const RectF& other) const;
	bool isInsideRect(Mouse& mouse) const;

public:
	int top;
	int bottom;
	int right;
	int left;
};
