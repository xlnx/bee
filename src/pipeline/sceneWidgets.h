#pragma once

#include "object.h"

namespace bee
{

namespace scene_impl
{

class Scene;

}

class SelectUtil
{
	friend class scene_impl::Scene;
public:
	SelectUtil() = default;
	virtual ~SelectUtil() = default;

	virtual void onSelect()
	{
	}
	virtual void onHover()
	{
	}
	virtual void onUnselect()
	{
	}
	virtual void onUnhover()
	{
	}
	bool selected() const
	{
		return isSelected;
	}
	bool hovered() const
	{
		return isHover;
	}
protected:
	void select(bool selected)
	{
		if (isSelected = selected)
		{
			onSelect();
		}
		else
		{
			onUnselect();
		}
	}
	void hover(bool hovered)
	{
		if (isHover = hovered)
		{
			onHover();
		}
		else
		{
			onUnhover();
		}
	}
	void filpSelect()
	{
		select(!isSelected);
	}
	void flipHover()
	{
		hover(!isHover);
	}
private:
	bool isSelected = false;
	bool isHover = false;
};

}