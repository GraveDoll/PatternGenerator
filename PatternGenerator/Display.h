#pragma once
#include <windows.h>
#include <string>

class Display
{
public:
	Display(void);
	~Display(void);
	void setDisplayArea(long, long, long, long);
	void setDisplayArea(RECT);
	void setName(TCHAR[]);
	long getBottom();
	long getLeft();
	long getRight();
	long getTop();
	RECT getDisplayArea();
	std::string getName();
private:
	int id;
	std::string name;		
	long bottom;			
	long left;			
	long right;				
	long top;				
	RECT rect;
};

