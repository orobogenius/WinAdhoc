#pragma once
#ifndef UIControl_H
#define UIControl_H
#endif

#include <Windows.h>
#include "ConnProfile.h"

class UIControl
{
public:
	UIControl();
	~UIControl();
	HRESULT GetSSID(LPCWSTR *psSSID, HWND hwnd);
	HRESULT GetPassword(LPCWSTR *psPass, HWND hwnd);
	bool IsStarted() { return isStarted; }
	void setStarted(bool started) { isStarted = started; }
	bool ConfirmPassword(LPCWSTR psPass, LPCWSTR psConpass);
private:
	bool isStarted = false;
};