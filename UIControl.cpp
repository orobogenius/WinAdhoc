#include "stdafx.h"

UIControl::UIControl()
{
}

UIControl::~UIControl()
{
}

HRESULT UIControl::GetSSID(LPCWSTR * psSSID, HWND hwnd)
{
	return E_NOTIMPL;
}

HRESULT UIControl::GetPassword(LPCWSTR * psPass, HWND hwnd)
{
	return E_NOTIMPL;
}

bool UIControl::ConfirmPassword(LPCWSTR psPass, LPCWSTR psConpass)
{
	return false;
}